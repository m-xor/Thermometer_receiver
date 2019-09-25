/*
 * test_sct2024/led7seg.h
 *
 *  Created on: 1 cze 2019
 *      Author: slawek
 */

#ifndef LED7SEG_H_
#define LED7SEG_H_

#include <stdint.h>
#include <stdbool.h>

//  Ułożenie segmentów
//	wspólna anoda lub wspólna katoda
//
//	/-- a --/
//	/		/
//	f		b
//	/		/
//	/-- g --/
//	/		/
//	e		c
//	/		/
//	/-- d --/	o dp
//


//---------- przypisanie segment-bit -----
#define A_SEG	1
#define B_SEG	3
#define C_SEG	6
#define D_SEG	4
#define E_SEG	5
#define F_SEG	2
#define G_SEG	0

//specjalne segmenty
#define SPEC_SEG 7
//położenie specjalnych segmentów w bajtach
#define MINUS_SEG 0
#define ERROR_SEG 1

//wartości specjalne funkcji led_display_number
#define ALL_MINUS INT16_MAX
#define ALL_QUESTION (INT16_MAX-1)

//0 - stani niski włącza segment, 1 - stan wysoki włącza segment (sct2024 włącza wysokim)
#define LED_ACTIVE 1

//---------- piny cyfr -----------
#define NUM_DIGITS	2

//odkomentuj jeśli wyświetlanie liczb szesnastkowych
#define HEX

//numery predefiniowanych komunikatów
enum {
	HALT_MSG,
	MAX_MSG	//zawsze ostatni
};
//************************************************************************************
/*
 * led_init - inicjalizuje porty (wg definicji powyżej), timer0 wraz z przerwaniami
 * oraz ustawia początkową jasność wyświetlacza na 0
 */
extern void led_init(void);
/*
 * led_test - wyświetla testowo wszystkie segmenty wszystkich cyfr oraz punkty dziesiętne
 * (jeśli są zdefinowane)
 */
extern void led_test(void);
/*
 * led_display_number - wypełnia bufor wyświetlacza, w razie przepełnienia wyświetlacza
 * 		wyświetla symbol błędu ("-")
 * number - liczba do wyświetlenia
 * denominator (zakres od 0 do NUM_DIGITS-1) - ilość cyfr po przecinku dziesiętnym.
 * 		dla konfiguracji bez przecinka dziesiętnego parametr jest bez znaczenia.
 */
extern void led_display_number(int16_t number, uint8_t base);

/*
 * Sygnalizuje wystąpienie błędu zapaleniem LED
 */
extern void led_notify_error(bool error);
/*
 * Wyświetla predefiniowane komunikaty
 */
extern void led_msg(uint8_t msgno);
/*
 * led_set_brightness - ustawia jasność wyświetlacza
 * brightnes (zakres od 0 do 255) - jasność (0 najmniejsza, 255 maksymalna)
 */
extern void led_set_brigthness(uint8_t brightness);
/*
 * led_clear_buff - wygasza wyświetlacz (czyści bufor wyświetlania), ponowne wyświetlenie
 * 	po użyciu funkcji led_display_number
 */
extern void led_clear_buff(void);

//wyświetla bufor
extern void led_show(void);

#endif /* LED7SEG_H_ */
