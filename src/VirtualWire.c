/*
 * VirtualWire.c
 *
 *  Created on: 1 sie 2014
 *      Author: slawek
 */

// Virtual Wire implementation for Arduino
// See the README file in this directory fdor documentation
// See also
// ASH Transceiver Software Designer's Guide of 2002.08.07
//   http://www.rfm.com/products/apnotes/tr_swg05.pdf
//
// Changes:
// 1.5 2008-05-25: fixed a bug that could prevent messages with certain
//  bytes sequences being received (false message start detected)
// 1.6 2011-09-10: Patch from David Bath to prevent unconditional reenabling of the receiver
//  at end of transmission.
//
// Author: Mike McCauley (mikem@airspayce.com)
// Copyright (C) 2008 Mike McCauley
// $Id: VirtualWire.cpp,v 1.9 2013/02/14 22:02:11 mikem Exp mikem $

#include <string.h>
#include <stdint.h>
#include "util/crc16.h"
#include "VirtualWire.h"

#define true 1
#define false 0


// Current receiver sample
//static uint16_t vw_rx_sample;
//static uint8_t vw_rx_sample; // = 0;

// Last receiver sample
static uint16_t vw_rx_last_sample;
//static uint8_t vw_rx_last_sample; // = 0;

// PLL ramp, varies between 0 and VW_RX_RAMP_LEN-1 (159) over
// VW_RX_SAMPLES_PER_BIT (8) samples per nominal bit time.
// When the PLL is synchronised, bit transitions happen at about the
// 0 mark.
static uint8_t vw_rx_pll_ramp; // = 0;

// This is the integrate and dump integral. If there are <5 0 samples in the PLL cycle
// the bit is declared a 0, else a 1
static uint8_t vw_rx_integrator; // = 0;

// Flag indictate if we have seen the start symbol of a new message and are
// in the processes of reading and decoding it
static uint8_t vw_rx_active; // = 0;

// Flag to indicate that a new message is available
static volatile uint8_t vw_rx_done; // = 0;

// Flag to indicate the receiver PLL is to run
static uint8_t vw_rx_enabled; // = 0;

// Last 12 bits received, so we can look for the start symbol
static uint16_t vw_rx_bits; // = 0;

// How many bits of message we have received. Ranges from 0 to 12
static uint8_t vw_rx_bit_count; // = 0;

// The incoming message buffer
static uint8_t vw_rx_buf[VW_MAX_MESSAGE_LEN];

// The incoming message expected length
static uint8_t vw_rx_count; // = 0;

// The incoming message buffer length received so far
static volatile uint8_t vw_rx_len; // = 0;

// Number of bad messages received and dropped due to bad lengths
static uint8_t vw_rx_bad; // = 0;

// Number of good messages received
static uint8_t vw_rx_good; // = 0;

//błedny FCS
//static uint8_t vw_rx_bad_crc;

// 4 bit to 6 bit symbol converter table
// Used to convert the high and low nybbles of the transmitted data
// into 6 bit symbols for transmission. Each 6-bit symbol has 3 1s and 3 0s
// with at most 3 consecutive identical bits
static const uint8_t symbols[] =
{
    0xd,  0xe,  0x13, 0x15, 0x16, 0x19, 0x1a, 0x1c,
    0x23, 0x25, 0x26, 0x29, 0x2a, 0x2c, 0x32, 0x34
};

// Compute CRC over count bytes.
// This should only be ever called at user level, not interrupt level
static uint16_t vw_crc(uint8_t *ptr, uint8_t count)
{
    uint16_t crc = 0xffff;

    while (count-- > 0)
    	crc = _crc_ccitt_update(crc, *ptr++);
    return crc;
}

// Convert a 6 bit encoded symbol into its 4 bit decoded equivalent
static uint8_t vw_symbol_6to4(uint8_t symbol)
{
    uint8_t i;

    // Linear search :-( Could have a 64 byte reverse lookup table?
    for (i = 0; i < 16; i++)
    	if (symbol == symbols[i]) return i;
    return 0; // Not found
}


// Called 8 times per bit period
// Phase locked loop tries to synchronise with the transmitter so that bit
// transitions occur at about the time vw_rx_pll_ramp is 0;
// Then the average is computed over each bit period to deduce the bit value
//void vw_pll()
static uint8_t vw_pll(uint16_t vw_rx_sample)
{
	uint8_t rsl = false;

    // Integrate each sample
    if (vw_rx_sample)
    	vw_rx_integrator++;

    if (vw_rx_sample != vw_rx_last_sample)
    {
    	// Transition, advance if ramp > 80, retard if < 80
    	vw_rx_pll_ramp += ((vw_rx_pll_ramp < VW_RAMP_TRANSITION)
    				? VW_RAMP_INC_RETARD
    				: VW_RAMP_INC_ADVANCE);
    	vw_rx_last_sample = vw_rx_sample;
    }
    else
    {
    	// No transition
    	// Advance ramp by standard 20 (== 160/8 samples)
    	vw_rx_pll_ramp += VW_RAMP_INC;
    }

    if (vw_rx_pll_ramp >= VW_RX_RAMP_LEN)
    {
    	// Add this to the 12th bit of vw_rx_bits, LSB first
    	// The last 12 bits are kept
    	vw_rx_bits >>= 1;

    	// Check the integrator to see how many samples in this cycle were high.
    	// If < 5 out of 8, then its declared a 0 bit, else a 1;
    	if (vw_rx_integrator >= 5)
    		vw_rx_bits |= 0x800;

    	vw_rx_pll_ramp -= VW_RX_RAMP_LEN;
    	vw_rx_integrator = 0; // Clear the integral for the next cycle

    	if (vw_rx_active)
    	{
    		// We have the start symbol and now we are collecting message bits,
    		// 6 per symbol, each which has to be decoded to 4 bits
    		if (++vw_rx_bit_count >= 12)
    		{
    			// Have 12 bits of encoded message == 1 byte encoded
    			// Decode as 2 lots of 6 bits into 2 lots of 4 bits
    			// The 6 lsbits are the high nybble
    			uint8_t this_byte =
    					(vw_symbol_6to4(vw_rx_bits & 0x3f)) << 4
    					| vw_symbol_6to4(vw_rx_bits >> 6);

    			// The first decoded byte is the byte count of the following message
    			// the count includes the byte count and the 2 trailing FCS bytes
    			// REVISIT: may also include the ACK flag at 0x40
    			if (vw_rx_len == 0)
    			{
    				// The first byte is the byte count
    				// Check it for sensibility. It cant be less than 4, since it
    				// includes the bytes count itself and the 2 byte FCS
    				vw_rx_count = this_byte;
    				if (vw_rx_count < 4 || vw_rx_count > VW_MAX_MESSAGE_LEN)
    				{
    					// Stupid message length, drop the whole thing
    					vw_rx_active = false;
    					vw_rx_bad++;
                        return false;
    				}
    			}
    			vw_rx_buf[vw_rx_len++] = this_byte;

    			if (vw_rx_len >= vw_rx_count)
    			{
    				// Got all the bytes now
    				vw_rx_active = false;
    				vw_rx_good++;
    				vw_rx_done = true; // Better come get it before the next one starts
    				rsl = true;
    			}
    			vw_rx_bit_count = 0;
    		}
    	}
    	// Not in a message, see if we have a start symbol
    	else if (vw_rx_bits == 0xb38)
    	{
    		// Have start symbol, start collecting message
    		vw_rx_active = true;
    		vw_rx_bit_count = 0;
    		vw_rx_len = 0;
    		vw_rx_done = false; // Too bad if you missed the last message
    	}
    }

    return (rsl);
}

// Enable the receiver. When a message becomes available, vw_rx_done flag
// is set, and vw_wait_rx() will return.
void vw_rx_start(void)
{
    if (!vw_rx_enabled)
    {
    	vw_rx_enabled = true;
    	vw_rx_active = false; // Never restart a partial message
    }
}

// Disable the receiver
void vw_rx_stop(void)
{
    vw_rx_enabled = false;
}


// Wait for the receiver to get a message
// Busy-wait loop until the ISR says a message is available
// can then call vw_get_message()
void vw_wait_rx(void)
{
    while (!vw_rx_done)
    	;
}


// Return true if there is a message available
uint8_t vw_have_message(void)
{
    return vw_rx_done;
}

// Get the last message received (without byte count or FCS)
// Copy at most *len bytes, set *len to the actual number copied
// Return true if there is a message and the FCS is OK
uint8_t vw_get_message(uint8_t* buf, uint8_t* len)
{
    uint8_t rxlen;

    // Message available?
    if (!vw_rx_done)
    	return false;

//uint8_t msg_copy[vw_rx_len];
//uint8_t len_copy = vw_rx_len;
//memcpy(msg_copy,vw_rx_buf,len_copy);

    // Wait until vw_rx_done is set before reading vw_rx_len
    // then remove bytecount and FCS
    rxlen = vw_rx_len - 3;

    // Copy message (good or bad)
    if (*len > rxlen)
    	*len = rxlen;
    memcpy(buf, vw_rx_buf + 1, *len);

    vw_rx_done = false; // OK, got that message thanks

    // Check the FCS, return goodness
    return (vw_crc(vw_rx_buf, vw_rx_len) == 0xf0b8); // FCS OK?
}


uint8_t vw_isr(uint16_t pin_state)
{
  if (vw_rx_enabled)
 	{
 		return vw_pll(pin_state);
 	}
  return 0;
}


uint8_t  vw_get_rx_bad(void)
{
	return vw_rx_bad;
}

uint8_t vw_get_rx_good(void)
{
	return vw_rx_good;
}

