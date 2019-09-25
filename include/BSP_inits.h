/*
 * BSP_inits.h
 *
 *  Created on: 23.07.2019
 *      Author: slawek
 */

#ifndef BSP_INITS_H_
#define BSP_INITS_H_

#include <stdint.h>

void BSP_resetClock(void);
void BSP_buttonInit(void);
void BSP_adcInit(void);
void BSP_adcStart(uint32_t prio);
//void BSP_TIM1Init(void);
void BSP_adcTriggerInit(void);
//void BSP_TIM3Init(void);
void BSP_pwmTimerInit(void);
void BSP_vwTimerInit(void);
void BSP_vwTimerStart(uint32_t prio);
void BSP_spiInit(void);
void BSP_uartInit(void);

#endif /* BSP_INITS_H_ */
