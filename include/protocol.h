/*
 * protocol.h
 *
 *  Created on: 13 lis 2018
 *      Author: slawek
 */

#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#include <stdint.h>

//3 bitowy id urządzenia
#define DEVICE_ID	0x5
//przesunięcie id urządzenia w bajcie (w nadajniku w lewo)
#define DEVICE_ID_POS	5

/*
 * struktura komunikatu
 *
 * ----- bajt 0 -----
 * 3 bity DEVICE_ID
 * 5 bitów nr kolejny komunikatu
 * ----- bajt 1 -----
 * 8 bitów ERRORS
 * ----- bajt 2 -----
 * 8 bitów ADC pomiar baterii
 * ----- bajt 3 -----
 * 8 bitów - młodszy bajt odczytu z czujnika
 * ----- bajt 4 -----
 * 1 bit HALT
 * 2 bity rozdzielczość czujnika
 * 1 bit niewykorzystany
 * 4 bity - młodsza tetrada starszego bajta odczytu z czujnika
 * (w odczycie z ds18b20 najstarsza piątka bitów jest zawsze taka sama)
 */

enum xdata {
	XDATA_DEVICE_ID,	//Identyfikator urządzenia, trzy najstarsze bity - wymaga maskowania
	XDATA_MSG_NO = XDATA_DEVICE_ID,	//nr kolejny komunikatu, 5 młodszych bitów - wymaga maskowania
	XDATA_ERRORS, //rejestr błędów, uint8_t
	XDATA_BATT,	//ADC baterii (odczyt dosunięty do lewej), uint8_t
	XDATA_TEMP,	//zamaskowane surowe dane z ds18b20, 12 bitów - wymaga maskowania i odtworzenia brakujących bitów z bitu 11.
	XDATA_TEMP_LOW =  XDATA_TEMP,	//młodszy bajt XDATA_TEMP
	XDATA_TEMP_HIGH, 	//młodsza tetrada starszego bajtu odczytu
	XDATA_RESL = XDATA_TEMP_HIGH,	//rozdzielczość czujnika (na starszych 4 bitach) - wymaga maskowania
	XDATA_HALT = XDATA_TEMP_HIGH,	//flaga zatrzymania urządzenia (na starszych 4 bitach) - wymaga maskowania
//	XDATA_RESL,	//rozdzielczość czujnika (na starszych 4 bitach) - wymaga maskowania
//	XDATA_HALT = XDATA_RESL,	//flaga zatrzymania urządzenia (na starszych 4 bitach) - wymaga maskowania
	XDATA_PACKET_SIZE
};

#define XDATA_DEVICE_ID_MASK	((uint8_t)(0x7<<DEVICE_ID_POS))
#define XDATA_MSG_NO_MASK		((uint8_t)((1<<DEVICE_ID_POS)-1))
#define XDATA_TEMP_MASK			((uint16_t)(0x0fff))
#define XDATA_TEMP_HIGH_MASK	((uint8_t)(XDATA_TEMP_MASK>>8))
#define XDATA_RESL_MASK			((uint8_t)(((1<<6)|(1<<5)))) //ds18x20.h:DS18B20_RES_MASK
#define XDATA_HALT_MASK			((uint8_t)(1<<7))

#define MSG_REPEAT_NUM	3	//ile razy przesłanie komunikatu będzie powtórzone

#endif /* PROTOCOL_H_ */
