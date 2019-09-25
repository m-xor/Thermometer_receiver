/*
 * gpio.h
 *
 *  Created on: 11.07.2019
 *      Author: slawek
 *
 *  Macros for configuration and using GPIO for STM32F030 and alike
 *
 */

#ifndef GPIO_H_
#define GPIO_H_

#include "stm32f0xx.h"
#include <stdint.h>

#define INLINE	inline __attribute__((always_inline))

typedef enum
{
	//mod							//może wystąpować z:
	INPUT 		= 	0U,			//podciąganie
	GP_OUTPUT	=	(1U<<9U),	//typ, podciąganie, szybkość
	ALTERNATE	=	(2U<<9U),	//typ, podciąganie, szybkość, alt
	ANALOG		=	(3U<<9U),	//-

	//typ
	PUSH_PULL	=	(0U),
	OPEN_DRAIN	=	(1U<<8U),

	//podciąganie
	NO_PULL_UP_DOWN	= (0U),
	PULL_UP		=	(1U<<4U),
	PULL_DOWN	=	(2U<<4U),

	//szybkość
	LOW_SPEED		=	0U,
	MEDIUM_SPEED	=	(1U<<6U),
	HIGH_SPEED		=	(3U<<6U),

	//alt
	AF0			=	0U,
	AF1			=	1U,
	AF2			=	2U,
	AF3			=	3U,
	AF4			=	4U,
	AF5			=	5U,
	AF6			=	6U,
	AF7			=	7U,

	//Alternate mod+alt. Może występować razem z typ, podciąganie, szybkość
	ALT_0	=	ALTERNATE + AF0,
	ALT_1	=	ALTERNATE + AF1,
	ALT_2	=	ALTERNATE + AF2,
	ALT_3	=	ALTERNATE + AF3,
	ALT_4	=	ALTERNATE + AF4,
	ALT_5	=	ALTERNATE + AF5,
	ALT_6	=	ALTERNATE + AF6,
	ALT_7	=	ALTERNATE + AF7

} GPIO_Mode_T;


typedef enum {

	GPIO_LO,
	GPIO_HI

} GPIO_Level_T;


static INLINE void GPIO_Config(GPIO_TypeDef * const __restrict__ gpio, int pin, GPIO_Mode_T mode)
{
	//require DEFINE_THIS_FILE from massert.h
	//ASSERT(pin<16);

	if (mode & 0x100u)
		gpio->OTYPER |= (uint16_t)(1<<pin);
	else
		gpio->OTYPER &= (uint16_t)~(1<<pin);

	pin *= 2;

	uint32_t reset_mask = ~(0x03u << pin);

	gpio->MODER &= reset_mask;
	gpio->MODER |= (((mode & 0x600u) >> 9u) << pin );

	gpio->PUPDR &= reset_mask;
	gpio->PUPDR |= (((mode & 0x30u) >> 4u) << pin );

	gpio->OSPEEDR &= reset_mask;
	gpio->OSPEEDR |= (((mode & 0xC0u) >> 6u) << pin);

	volatile uint32_t * reg_adr;
	pin *= 2;
	if (pin < 32)
	{
			reg_adr = &gpio -> AFR[0];
	}
	else
	{
			reg_adr = &gpio -> AFR[1];
			pin -= 32;
	}

	*reg_adr &= ~(0x0fu << pin);
	*reg_adr |= (uint32_t)(mode & 0x0ful) << pin;

}

static INLINE void GPIO_Set(GPIO_TypeDef * const __restrict__ gpio, int pin, GPIO_Level_T level)
{
	gpio -> BSRR = (uint32_t)(1 << (pin + !(level) * 16));
}

static INLINE void GPIO_Hi(GPIO_TypeDef * const __restrict__ gpio, int pin)
{
	gpio -> BSRR = (uint32_t)(1 << pin);
}

static INLINE void GPIO_Low(GPIO_TypeDef * const __restrict__ gpio, int pin)
{
	gpio -> BRR = (uint16_t)(1 << pin);
}

static INLINE int GPIO_Get(GPIO_TypeDef * const __restrict__ gpio, int pin)
{
	return (gpio -> IDR & (1 << pin));
}

static INLINE int GPIO_Get01(GPIO_TypeDef * const __restrict__ gpio, int pin)
{
	return !!(gpio -> IDR & (1 << pin));
}


#endif /* GPIO_H_ */
