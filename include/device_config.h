/*
 * device_config.h
 *
 *  Created on: 23.07.2019
 *      Author: slawek
 */

#ifndef DEVICE_CONFIG_H_
#define DEVICE_CONFIG_H_

//Piny dostępne w TSSOP20
//....


//wejście ADC
#define LDR_GPIO			A
//#define LDR_PIN				0	//ADC0 ----- UWAGA ------
#define LDR_PIN				1	//obwód testowy
//dla pinów A0-A7 nr kanału równy nr pinu, na PB1 kanał ADC 9
#define ADC_CHANNEL	LDR_PIN
//długość średniej kroczącej odczutu LDR
#define ADC_OVS 16
//częstość pomiarów ADC
#define ADC_READS_PER_SEC	25U
//rozdzielczość ADC (symbol podany informacyjnie)
#define ADC_GRAN	64

//linie danych wyświetlacza
#define SPI_GPIO			A
#define SPI_NSS_PIN		4
#define SPI_SCK_PIN		5
#define SPI_MOSI_PIN	7
//sterowanie jasnością wyświetlacza
//dla TIM3 tylko PA6, PA7, PB1
#define OE_GPIO			A
#define OE_PIN			6	//wyjście TIM3
//Częstotliwość PWM wyświetlacza
#define OE_TICKS_PER_SEC	100U
//rozdzielczość licznika
#define TIM_GRAN	4096U

//wejście z RFM210L
//wskazane by było to wejście FT, w TSOP20 są to tylko PA9, 10, 13, 14
#define VW_GPIO			B
#define VW_PIN			1
#define VW_SPEED	2000

#define BUTTON_GPIO			A
//#define BUTTON_PIN			10 // ------ UWAGA -----
#define BUTTON_PIN			0	//obwód testowy

//opcjonalnie, config lub wyjście QSPY
#define UART_GPIO			A
#define UART_RX_PIN		3
#define UART_TX_PIN		2

////opcjonalnie, wyjście QSPY
//#define I2C_GPIO			A
//#define I2C_SCL				9
//#define I2C_SDA				10 //zajęte przez button

//Zostawić w stanie domyślnym
#define SW_GPIO			A
#define SWDIO_PIN		13
#define SWCLK_PIN		14

//pomocnicy
#define GPIO(b_) GPIO_(b_)
#define GPIO_(b_) (GPIO##b_)

#define AHBGPIOxEN(b_) AHBEN_(b_)
#define AHBEN_(b_) (RCC_AHBENR_GPIO##b_##EN)


#endif /* DEVICE_CONFIG_H_ */
