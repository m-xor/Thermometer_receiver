/*
 * link_proto.c
 *
 *  Created on: 20.12.2018
 *      Author: slawek
 */

#include "ds18x20.h"
#include "ds18x20rx.h"
#include "protocol.h"
#include "link-proto.h"

#define _BV(par) (1<<par)
#include "debug_conf.h"	//for E_VAL_MSK
#define VAL_ERR_MSK (E_VAL_MSK)

//Zmierzony bandgap µC nadajnika *1000
#define BG_V_KVAL 1085

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

//nr ostatnio odebranego komunikatu
static uint8_t current = XDATA_MSG_NO_MASK+1;

/*
 * true jeśli nr komunikatu wskazuje na nową treść komunikatu
 */
bool isNewMsg( uint8_t const * msg, uint8_t const msgLen )
{

	uint8_t tmp = current;

	return (tmp != (current=(msg[XDATA_MSG_NO]&XDATA_MSG_NO_MASK)));
}

//void rstMsgId(void)
//{
//	current = XDATA_MSG_NO_MASK+1;
//}

uint8_t getErr( uint8_t const * msg, uint8_t const msgLen )
{
	return msg[XDATA_ERRORS];
}

/*
 * true jeśli komunikat ma prawidłową wartość
 */
bool isNotTrunc( uint8_t const * msg, uint8_t const msgLen )
{
	return (msgLen==XDATA_PACKET_SIZE);
}

/*
 * true jeśli Id urządzenia w komunikacie jest zgodny z oczekiwanym
 */
bool isIdTx( uint8_t const * msg, uint8_t const msgLen )
{
	uint8_t id = (msg[XDATA_DEVICE_ID] & XDATA_DEVICE_ID_MASK)>>DEVICE_ID_POS;

	return (id==DEVICE_ID);
}

/*
 * true jeśli komunikat zeznaje, że urządzenie uległo zatrzymaniu
 */
bool isHalt( uint8_t const * msg, uint8_t const msgLen )
{
	return (msg[XDATA_HALT] & XDATA_HALT_MASK);
}

/*
 * true jeśli raportowane błędy wskazują, że przekazane wartości napięcia i temperatury
 * nie mają znaczenia
 */
bool isValErr( uint8_t const * const msg, uint8_t const msgLen )
{
	return (msg[XDATA_ERRORS] & VAL_ERR_MSK);
}

/*
 * Zwraca wartość napięcia baterii nadajnika
 */
void getBatt( uint8_t const * const msg, uint8_t const msgLen, int16_t *value, uint8_t *denom )
{
	uint32_t tmp = BG_V_KVAL*256UL;
	tmp /= msg[XDATA_BATT]*10;

	*value = tmp/10 + (((tmp%10)>=5)?1:0);
	*denom = 1;
}

/*
 * Zwraca temperaturę zmierzoną i przetransmitowaną przez nadajnik
 */
void getTemp( uint8_t const * const msg, uint8_t const msgLen, int16_t *value, uint8_t *denom )
{
	//wyekstrahować dokładność i odtworzyć najstarsze bity (znaku)
	uint8_t resolution = msg[XDATA_RESL] & XDATA_RESL_MASK;
	uint16_t temp_raw = (uint16_t)(msg[XDATA_TEMP_HIGH]<<8 | msg[XDATA_TEMP_LOW]) & XDATA_TEMP_MASK;
	//odtworzenie bitu znaku
	if( temp_raw & _BV(11) )
		temp_raw |= 0xf000;

	*value = DS18X20_raw_to_decicelsius( resolution, temp_raw );
	*denom = 1;

}

#pragma GCC diagnostic pop
