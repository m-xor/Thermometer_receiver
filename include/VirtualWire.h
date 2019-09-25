/*
 * VirtualWire.h
 *
 *  Created on: 1 sie 2014
 *      Author: slawek
 */

#ifndef VIRTUALWIRE_H_
#define VIRTUALWIRE_H_

#include <stdint.h>


/// The maximum payload length (część użytkowa)
#define VW_MAX_PAYLOAD 5

/// Maximum number of bytes in a message, counting the byte count and FCS
#define VW_MAX_MESSAGE_LEN (VW_MAX_PAYLOAD+3)

/// The size of the receiver ramp. Ramp wraps modulu this number
#define VW_RX_RAMP_LEN 160

/// Number of samples per bit
#define VW_RX_SAMPLES_PER_BIT 8

// Ramp adjustment parameters
// Standard is if a transition occurs before VW_RAMP_TRANSITION (80) in the ramp,
// the ramp is retarded by adding VW_RAMP_INC_RETARD (11)
// else by adding VW_RAMP_INC_ADVANCE (29)
// If there is no transition it is adjusted by VW_RAMP_INC (20)
/// Internal ramp adjustment parameter
#define VW_RAMP_INC (VW_RX_RAMP_LEN/VW_RX_SAMPLES_PER_BIT)
/// Internal ramp adjustment parameter
#define VW_RAMP_TRANSITION VW_RX_RAMP_LEN/2
/// Internal ramp adjustment parameter
#define VW_RAMP_ADJUST 9
/// Internal ramp adjustment parameter
#define VW_RAMP_INC_RETARD (VW_RAMP_INC-VW_RAMP_ADJUST)
/// Internal ramp adjustment parameter
#define VW_RAMP_INC_ADVANCE (VW_RAMP_INC+VW_RAMP_ADJUST)

/// Outgoing message bits grouped as 6-bit words
/// 36 alternating 1/0 bits, followed by 12 bits of start symbol
/// Followed immediately by the 4-6 bit encoded byte count,
/// message buffer and 2 byte FCS
/// Each byte from the byte count on is translated into 2x6-bit words
/// Caution, each symbol is transmitted LSBit first,
/// but each byte is transmitted high nybble first
#define VW_HEADER_LEN 8

    /// Start the Phase Locked Loop listening to the receiver
    /// Must do this before you can receive any messages
    /// When a message is available (good checksum or not), vw_have_message();
    /// will return true.
    extern void vw_rx_start(void);

    /// Stop the Phase Locked Loop listening to the receiver
    /// No messages will be received until vw_rx_start() is called again
    /// Saves interrupt processing cycles
    extern void vw_rx_stop(void);

    /// Block until a message is available
    /// then returns
    extern void vw_wait_rx(void);

    // Returns true if an unread message is available
    /// \return true if a message is available to read
    extern uint8_t vw_have_message(void);

    // If a message is available (good checksum or not), copies
    // up to *len octets to buf.
    /// \param[in] buf Pointer to location to save the read data (must be at least *len bytes.
    /// \param[in,out] len Available space in buf. Will be set to the actual number of octets read
    /// \return true if there was a message and the checksum was good
    extern uint8_t vw_get_message(uint8_t* buf, uint8_t* len);

    //wywoływana periodycznie w ISR bps*8/sek.
    //zwraca true jeśli jest nowy msg
    extern uint8_t vw_isr(uint16_t pin_state);

    //zwraca ilość serii danych w których symbol startu został niby wykryty ale
    //bajt długości komunikatu nie miał sensownej wartości (<4 lub >MAX)
    extern uint8_t  vw_get_rx_bad(void);
    //zwraca ilość odebranych komunikatów o prawidłowej długości przed
    //sprawdzeniem FCS. Ostatecznie sprawdzenie komunikatu odbywa się w funkcji
    //vw_get_message, która w razie błędu FCS zwraca false.
    extern uint8_t vw_get_rx_good(void);

#endif /* VIRTUALWIRE_H_ */
