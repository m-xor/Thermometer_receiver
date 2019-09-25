/*
 * spi.h
 *
 *  Created on: 06.06.2019
 *      Author: slawek
 */

#ifndef SPI_H_
#define SPI_H_

#include "stm32f0xx.h"
#include <stdint.h>

/*
 * SPI clock polarity and phase constants (mode)
 */
#define SPI_CLK_LO_LEADING	0								//Clock idle level is low, sample on rising edge
#define SPI_CLK_LO_TRAILING	(SPI_CR1_CPHA)	//Clock idle level is low, sample on falling edge
#define SPI_CLK_HI_LEADING	(SPI_CR1_CPOL)	//Clock idle level is high, sample on falling edge
#define SPI_CLK_HI_TRAILING	(SPI_CR1_CPHA|SPI_CR1_CPOL)	//Clock idle level is high, sample on rising edge

/*
 * Clock rate select constants
 */
#define SPI_CLK_DIV_2		0
#define SPI_CLK_DIV_4		(SPI_CR1_BR_0)
#define SPI_CLK_DIV_8		(SPI_CR1_BR_1)
#define SPI_CLK_DIV_16	(SPI_CR1_BR_0|SPI_CR1_BR_1)
#define SPI_CLK_DIV_32	(SPI_CR1_BR_2)
#define SPI_CLK_DIV_64	(SPI_CR1_BR_0|SPI_CR1_BR_2)
#define SPI_CLK_DIV_128	(SPI_CR1_BR_1|SPI_CR1_BR_2)
#define SPI_CLK_DIV_256	(SPI_CR1_BR_0|SPI_CR1_BR_1|SPI_CR1_BR_2)

/*
 * Data order constants
 */
#define SPI_LSB	(SPI_CR1_LSBFIRST)	//The LSB of the data word is transmitted first
#define SPI_MSB	0	//The MSB of the data word is transmitted first

/*
 * Modes of transmission. Unidirectional only.
 */
//Both lines active
#define SPI_FULL_DUPLEX	0
//only MOSI line active. MISO can be used as GPIO. Ignore OVF and other events concerning RX
#define SPI_TX_ONLY	0
//only MISO line active. MOSI can be used as GPIO
#define SPI_RX_ONLY	(SPI_CR1_RXONLY)


//extern void spi_master_init(uint8_t mode, uint8_t clkdiv, uint8_t ord);
//extern void spi_set_ss();
//extern void spi_reset_ss();
extern void spi_tx(uint16_t data);
extern void spi_tx_buf(uint16_t *to_send, uint8_t count);

#endif /* SPI_H_ */
