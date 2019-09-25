/*
 * link-proto.h
 *
 *  Created on: 20.12.2018
 *      Author: slawek
 */

#ifndef LINK_PROTO_H_
#define LINK_PROTO_H_

#include <stdbool.h>
#include <stdint.h>


/*
 * true jeśli nr komunikatu wskazuje na nową treść komunikatu
 */
bool isNewMsg( uint8_t const * msg, uint8_t const msgLen );
/*
 * Reset nr komunikatu
 */
//void rstMsgId(void);
/*
 * Zwraca błędy przesłane przez nadajnik
 */
uint8_t getErr( uint8_t const * msg, uint8_t const msgLen );
/*
 * true jeśli komunikat ma prawidłową wartość
 */
bool isNotTrunc( uint8_t const * msg, uint8_t const msgLen );
/*
 * true jeśli Id urządzenia w komunikacie jest zgodny z oczekiwanym
 */
bool isIdTx( uint8_t const * msg, uint8_t const msgLen );
/*
 * true jeśli komunikat zeznaje, że urządzenie uległo zatrzymaniu
 */
bool isHalt( uint8_t const * msg, uint8_t const msgLen );
/*
 * true jeśli raportowane błędy wskazują, że przekazane wartości napięcia i temperatury
 * nie mają znaczenia
 */
bool isValErr( uint8_t const * msg, uint8_t const msgLen );
/*
 * Zwraca wartość napięcia baterii nadajnika
 */
void getBatt( uint8_t const * msg, uint8_t const msgLen, int16_t *value, uint8_t *denom );
/*
 * Zwraca temperaturę zmierzoną i przetransmitowaną przez nadajnik
 */
void getTemp( uint8_t const * msg, uint8_t const msgLen, int16_t *value, uint8_t *denom );

#endif /* LINK_PROTO_H_ */
