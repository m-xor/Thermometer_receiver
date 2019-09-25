/*
 * debug_conf.h
 *
 *  Created on: 23 gru 2018
 *      Author: slawek
 */

#ifndef DEBUG_CONF_H_
#define DEBUG_CONF_H_

//ilość powtórzeń trasmisji błędu
#define DEBUG_REPEAT_NUM	3

typedef enum error_en {
	E_OK		= 0,		//0
							// flagi zdef. w <avr/io.h> wskazujące na
							//  przyczynę resetu (odczytywane w .init3 z MCUSR):
	E_PORF 		= _BV(0),	//1:PORF
	E_EXTRF 	= _BV(1),	//2:EXTRF
	E_BORF 		= _BV(2),	//4:BORF (BOD jest nieużywany)
	E_WDRF 		= _BV(3),	//8:WDRF
							// wystąpienie poniższych flag wskazuje, że
							//  wartość w polu temperatury jest przypadkowa
	E_PRESENCE 	= _BV(4),	//16:Błąd resetu - brak czujnika 1wire lub zwarcie magistrali
	E_SENSOR 	= _BV(5),	//32:Brak czujnika typu 18B20 lub więcej niż jeden
	E_BUS 		= _BV(6),	//64:Błąd magistrali (przekłamanie z powodu zakłóceń?)
	E_CRC	 	= _BV(7)	//128:Błąd CRC odczytanych danych
} error_t_;

#define E_RST_MSK (E_PORF|E_EXTRF|E_BORF|E_WDRF)
#define E_VAL_MSK (E_PRESENCE|E_SENSOR|E_BUS|E_CRC)

#endif /* DEBUG_CONF_H_ */
