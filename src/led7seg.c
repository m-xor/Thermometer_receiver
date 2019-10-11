/*
 * test_sct2024/led7seg.c
 *
 *  Created on: 1 cze 2019
 *      Author: slawek
 */

#include "led7seg.h"
#include "spi.h"


//****************** definicje **********************

#define _BV(arg)	(1<<(arg))

//maska tylko segmentów, bez punktu dziesiętnego (chyba jednak niepotrzebne)
#define SEG_MASK	(_BV(A_SEG) | _BV(B_SEG) | _BV(C_SEG) | _BV(D_SEG) | _BV(E_SEG) | _BV(F_SEG) | _BV(G_SEG))


//polaryzacja robocza pinów segmentów
#if ( LED_ACTIVE == 1 )
//stan wysoki włącza segmenty
#define PATTERN_MOD
#else
//stan niski włącza segmenty
#define PATTERN_MOD ~
#endif


//wyświetlanie liczb dziesiętnych czy heksadecymalnych
#ifdef HEX
#define DIV 16
#else
#define DIV 10
#endif
//indeksy znaków specjalnych
#define MINUS_SIGN	(DIV)
#define DIGIT_OFF	(DIV+1)
#define DIGIT_ON	8
#define ERROR_SIGN	(MINUS_SIGN)
#define QUESTION_SIGN (DIV+2)


//******************** stałe i zmienne **************************
//------------- mapa bitowa znaków ----------------------
static const uint8_t led_pattern[] = {
	(uint8_t)((PATTERN_MOD(_BV(A_SEG) | _BV(B_SEG) | _BV(C_SEG) | _BV(D_SEG) | _BV(E_SEG) | _BV(F_SEG) )) & SEG_MASK), //0
	(uint8_t)((PATTERN_MOD(_BV(B_SEG) | _BV(C_SEG) )) & SEG_MASK), //1
	(uint8_t)((PATTERN_MOD(_BV(A_SEG) | _BV(B_SEG) | _BV(D_SEG) | _BV(E_SEG) | _BV(G_SEG) )) & SEG_MASK), //2
	(uint8_t)((PATTERN_MOD(_BV(A_SEG) | _BV(B_SEG) | _BV(C_SEG) | _BV(D_SEG) | _BV(G_SEG) )) & SEG_MASK), //3
	(uint8_t)((PATTERN_MOD(_BV(B_SEG) | _BV(C_SEG) | _BV(F_SEG) | _BV(G_SEG) )) & SEG_MASK), //4
	(uint8_t)((PATTERN_MOD(_BV(A_SEG) | _BV(C_SEG) | _BV(D_SEG) | _BV(F_SEG) | _BV(G_SEG) )) & SEG_MASK), //5
	(uint8_t)((PATTERN_MOD(_BV(A_SEG) | _BV(C_SEG) | _BV(D_SEG) | _BV(E_SEG) | _BV(F_SEG) | _BV(G_SEG) )) & SEG_MASK), //6
	(uint8_t)((PATTERN_MOD(_BV(A_SEG) | _BV(B_SEG) | _BV(C_SEG) )) & SEG_MASK), //7
	(uint8_t)((PATTERN_MOD(_BV(A_SEG) | _BV(B_SEG) | _BV(C_SEG) | _BV(D_SEG) | _BV(E_SEG) | _BV(F_SEG) | _BV(G_SEG) )) & SEG_MASK), //8
	(uint8_t)((PATTERN_MOD(_BV(A_SEG) | _BV(B_SEG) | _BV(C_SEG) | _BV(D_SEG) | _BV(F_SEG) | _BV(G_SEG) )) & SEG_MASK),//9
#ifdef HEX
	(uint8_t)((PATTERN_MOD(_BV(A_SEG) | _BV(B_SEG) | _BV(C_SEG) | _BV(E_SEG) | _BV(F_SEG) | _BV(G_SEG) )) & SEG_MASK), //A
	(uint8_t)((PATTERN_MOD(_BV(C_SEG) | _BV(D_SEG) | _BV(E_SEG) | _BV(F_SEG) | _BV(G_SEG) )) & SEG_MASK), //B
	(uint8_t)((PATTERN_MOD(_BV(A_SEG) | _BV(D_SEG) | _BV(E_SEG) | _BV(F_SEG) )) & SEG_MASK), //C
	(uint8_t)((PATTERN_MOD(_BV(B_SEG) | _BV(C_SEG) | _BV(D_SEG) | _BV(E_SEG) | _BV(G_SEG) )) & SEG_MASK), //D
	(uint8_t)((PATTERN_MOD(_BV(A_SEG) | _BV(D_SEG) | _BV(E_SEG) | _BV(F_SEG) | _BV(G_SEG) )) & SEG_MASK), //E
	(uint8_t)((PATTERN_MOD(_BV(A_SEG) | _BV(E_SEG) | _BV(F_SEG) | _BV(G_SEG) )) & SEG_MASK),//F
#endif
	(uint8_t)((PATTERN_MOD(_BV(G_SEG) )) & SEG_MASK),	//znak minus
	(uint8_t)((PATTERN_MOD(~(SEG_MASK) )) & SEG_MASK), 	//wszystkie segmenty wygaszone
	//(uint8_t)((PATTERN_MOD(SEG_MASK)) & SEG_MASK)	//wszystkie segmenty włączone
	(uint8_t)((PATTERN_MOD(_BV(A_SEG) | _BV(B_SEG) | _BV(E_SEG) | _BV(G_SEG) )) & SEG_MASK) //? - przepełnienie
};

//---------------- zmienne --------------------

//__attribute__((aligned(2))) //dla STM32 wyrównanie do uint16_t
_Alignas(uint16_t)
static union {
	uint8_t array[NUM_DIGITS];	//bufor wyświetlania
	uint16_t data2send;
} buf;

//******************** funkcje ******************
//----------- zainicjowanie pinów, i SPI  ------------
//void led_init()
//{
//	//sct2014: częstotliwość zegara SPI do 25Mhz
//	spi_master_init(SPI_CLK_LO_LEADING, SPI_CLK_DIV2, SPI_MSB);
//	spi_set_ss();
//}

//--------- wypełnienie bufora jednym znakiem -----------
static void fill_buffer(uint8_t pattern)
{
	for( uint8_t i = 0; i<NUM_DIGITS; i++ )
	{
		buf.array[i] = pattern;
	}
}
//--------- test wyświetlacza ---------------------------
void led_test(void)
{
	fill_buffer( led_pattern[ DIGIT_ON ] | _BV(SPEC_SEG) );
}

//--------- wypełnia bufor wyświetlania ------------------
void led_display_number(int16_t number, uint8_t base)
{
	uint8_t i, minus, digits, /*denominator,*/ error;

	led_clear_buff();

	//zachowujemy stan errora
	error = buf.array[ERROR_SEG] & _BV(SPEC_SEG);

	if(number > SPEC_SIGNS_GUARD)
	{
		if(number==ONE_MINUS)
		{
			buf.array[0] = led_pattern[ MINUS_SIGN ];
		}
		else if(number==ONE_QUESTION)
		{
			buf.array[0] = led_pattern[ QUESTION_SIGN ];
		}
		else if(number==ALL_MINUS)
		{
			fill_buffer( led_pattern[ MINUS_SIGN ] );
		}
		else
		{
			fill_buffer( led_pattern[ QUESTION_SIGN ] );
		}
	}
	else
	{
		//denominator = 0;

		if(number<0)
		{
			minus = 1;
	//		digits = NUM_DIGITS - 1;
			number *= -1;
		}
		else
		{
			minus = 0;
	//		digits = NUM_DIGITS;
		}
		digits = NUM_DIGITS;

	//	if(denominator>=digits)	//przepełnienie wyświetlacza
	//	{
	//		fill_buffer( led_pattern[ ERROR_SIGN ] );
	//		return;
	//	}

		//dla liczby równej zero trzeba wyświetlić zero
		//dla liczb mniejszych od 1 trzeba dodać zero przed kropką dziesiętną oraz ewentualnie po kropce
		//załatwia to -> ( || denominator>=i)
	//	for( i = 0; (number!=0 || denominator>=i) && i<digits; i++ )
		for( i = 0; (number!=0 || 0>=i) && i<digits; i++ )
		{
			buf.array[i] = led_pattern[ number%base ];
			number /= base;
		}

		//jeżeli number != 0 to nastąpiło przepełnienie wyświetlacza
		if(number)
		{
			fill_buffer( led_pattern[ ERROR_SIGN ] );
			return;
		}

		//dopisz znak minus jeśli trzeba
		if(minus)
		{
			buf.array[MINUS_SEG] |= _BV(SPEC_SEG);
		}
	}

	//odtwarzamy sygnalizację error
	buf.array[ERROR_SEG] |= error;
}

void led_notify_error(bool error)
{
	if(error)
		buf.array[ERROR_SEG] |= _BV(SPEC_SEG);
	else
		buf.array[ERROR_SEG] &= ~_BV(SPEC_SEG);
}
//---------------
void led_msg(uint8_t msgno)
{
	static uint8_t const msg[][NUM_DIGITS] = {

				{	/* HA */
					((uint8_t)((PATTERN_MOD(_BV(B_SEG) | _BV(C_SEG) | _BV(E_SEG) | _BV(F_SEG) | _BV(G_SEG) )) & SEG_MASK)), //H
					((uint8_t)((PATTERN_MOD(_BV(A_SEG) | _BV(B_SEG) | _BV(C_SEG) | _BV(E_SEG) | _BV(F_SEG) | _BV(G_SEG) )) & SEG_MASK))	//A
				},
				{	/* __ */
					((uint8_t)((PATTERN_MOD(_BV(D_SEG))) & SEG_MASK)),
						((uint8_t)((PATTERN_MOD(0)) & SEG_MASK))
//					((uint8_t)((PATTERN_MOD(_BV(D_SEG))) & SEG_MASK))
				},
				{	/* ^^ */
					((uint8_t)((PATTERN_MOD(_BV(A_SEG))) & SEG_MASK)),
					((uint8_t)((PATTERN_MOD(0)) & SEG_MASK))
//					((uint8_t)((PATTERN_MOD(_BV(A_SEG))) & SEG_MASK))
				},
				{	/* -- */
					((uint8_t)((PATTERN_MOD(0)) & SEG_MASK)),
					((uint8_t)((PATTERN_MOD(0)) & SEG_MASK))
				}


	};

	uint8_t error = buf.array[ERROR_SEG] & _BV(SPEC_SEG); 	//zachowujemy stan errora

	if(msgno < MAX_MSG )
	{
			buf.array[0] = msg[msgno][1];
			buf.array[1] = msg[msgno][0];
	}

	buf.array[ERROR_SEG] |= error;
}
//--------- ustawia jasność ------------------
void led_set_brigthness(uint8_t brightness)
{
	(void)brightness;
//	OCR0A = 255-brightness;
//	OCR0A = brightness;
}

//--------- czyści bufor wyświetlania ------------------
void led_clear_buff(void)
{
	uint8_t error;

	//zachowujemy stan errora
	error = buf.array[ERROR_SEG] & _BV(SPEC_SEG);

	fill_buffer( led_pattern[ DIGIT_OFF ] );

	//odtwarzamy sygnalizację error
	buf.array[ERROR_SEG] |= error;
}

void led_show(void)
{

//	uint8_t tmp[NUM_DIGITS];

//	spi_reset_ss();
	spi_tx(buf.data2send);
	//laczkowanie wyjść
//	spi_set_ss();

}
