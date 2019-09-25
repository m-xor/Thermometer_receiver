/*
 * spi.c
 *
 *  Created on: 06.06.2019
 *      Author: slawek
 */


#include "spi.h"
#include "stm32f0xx.h"

#define SPI_CLK_DIV_Msk	(SPI_CR1_BR)
#define SPI_CLK_Msk					(SPI_CR1_CPHA|SPI_CR1_CPOL)
#define SPI_DATA_ORDER_Msk	(SPI_CR1_LSBFIRST)

//void spi_master_init(uint8_t mode, uint8_t clkdiv, uint8_t ord)
//{
////	DDRB|=(_BV(PB2) | _BV(PB3) | _BV(PB5));	//SS, MOSI, SCK as output
////	SPSR = (clkdiv & 4)>>2;	//set double speed bit
////	SPCR = _BV(SPE) | _BV(MSTR) | ord | mode | (clkdiv & 3);
////	SPSR; SPDR; //Set off SPIF
//}
//
//void spi_set_ss()
//{
//// PORTB |= _BV(PB2);
//}
//
//void spi_reset_ss()
//{
//// PORTB &= ~(_BV(PB2));
//}

void spi_tx(uint16_t data)
{
	while((SPI1->SR & SPI_SR_TXE) != SPI_SR_TXE); /* Test Tx empty */
	*(uint16_t *)&(SPI1->DR) = data;
}

void spi_tx_buf(uint16_t *to_send, uint8_t count)
{
	for(;count;--count)
		spi_tx(*to_send++);
}

