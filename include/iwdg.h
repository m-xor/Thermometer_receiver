/*
 * iwdg.h
 *
 *  Created on: 24.07.2019
 *      Author: slawek
 */

#ifndef IWDG_H_
#define IWDG_H_

#include <stdint.h>
#include "stm32f0xx.h"

#define INLINE	inline __attribute__((always_inline))

#define IWDG_START 					0x0000cccc
#define IWDG_WRITE_ACCESS 	0x00005555
#define IWDG_REFRESH 				0x0000aaaa

#define IWDG_MAX IWDG_RLR_RL

typedef enum {
	IWDG_4,
	IWDG_8,
	IWDG_16,
	IWDG_32,
	IWDG_64,
	IWDG_128,
	IWDG_256
} IWDG_Presc_T;


//To call once at startup. Once running, the IWDG cannot be stopped.
//timer 40kHz thus approximate reset time equals 1/40000 * prescaler * reload
//window > 0 activates window option.
//prescaler -
//reload - 12 bit downcounter (0-4095)
//window - 12 bit, reset occurs if refreshed while counter is greater
//than window
static INLINE void iwdg_enable(IWDG_Presc_T prescaler, uint16_t reload, uint16_t window)
{
	/* (1) Activate IWDG (not needed if done in option bytes) */
	/* (2) Enable write access to IWDG registers */
	/* (3) Set prescaler by 8 */
	/* (4) Set reload value to have a rollover each 100ms */
	/* (5) Check if flags are reset */
	/* (6) Set a 50ms window, this will refresh the IWDG */
	/* (6) Refresh counter */
	IWDG->KR = IWDG_START; /* (1) */
	IWDG->KR = IWDG_WRITE_ACCESS; /* (2) */
	IWDG->PR = IWDG_PR_PR & prescaler; /* (3) */
	IWDG->RLR = reload & IWDG_RLR_RL; /* (4) */
	while (IWDG->SR) /* (5) */
	{
	/* add time out here for a robust application */
	}
	if(window)
		IWDG->WINR = window & IWDG_WINR_WIN; /* (6) */
	else
		IWDG->KR = IWDG_REFRESH; /* (6) */
}

//reload the counter preventing watchdog reset
static INLINE void iwdg_reset(void)
{
	IWDG->KR = IWDG_REFRESH;
}


#endif /* IWDG_H_ */
