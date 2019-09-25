/*
 * BSP_inits.c
 *
 *  Created on: 23.07.2019
 *      Author: slawek
 */

#include "BSP_inits.h"
#include "stm32f0xx.h"
//#include "massert.h"
#pragma GCC diagnostic push
//DEFINE_THIS_FILE
#include "gpio.h"
#include "device_config.h"


void BSP_resetClock(void)
{

	//W przypadku zamontowania rezonatora zresetować zegar SYSCLK do 8 MHz
	//przez wyłączenie PLL, ustawienie preskalerów, FLASH waitstates
	//oraz update zmiennej SystemCoreClock przez wywołanie SystemCoreClockUpdate()

  SystemCoreClockUpdate();

	//jeśli zegar chodzi na HSI to pomiar temperatury rdzenia i trymowanie
	//przez HSITRIM wg przygotowanej tablicy ?

}


void BSP_buttonInit(void)
{
	RCC->AHBENR |= AHBGPIOxEN(BUTTON_GPIO);
	GPIO_Config(GPIO(BUTTON_GPIO), BUTTON_PIN, INPUT|PULL_UP);
}

void BSP_adcInit(void)
{
	//pin
	RCC->AHBENR |= AHBGPIOxEN(LDR_GPIO);
	GPIO_Config(GPIO(LDR_GPIO), LDR_PIN, ANALOG);

	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
	//zegar synchroniczny z podzielnikiem 2 (PCLK/2) -> 4MHz
	ADC1->CFGR2 |= ADC_CFGR2_CKMODE_0;

	//kalibracja (powtarzać przy każdym wyłączeniu peryferia np. w STANDBY mode
	/* (1) Ensure that ADEN = 0 */
	/* (2) Clear ADEN by setting ADDIS*/
	/* (3) Clear DMAEN */
	/* (4) Launch the calibration by setting ADCAL */
	/* (5) Wait until ADCAL=0 */
	if ((ADC1->CR & ADC_CR_ADEN) != 0) /* (1) */
	{
			ADC1->CR |= ADC_CR_ADDIS; /* (2) */
	}
	while ((ADC1->CR & ADC_CR_ADEN) != 0)
	{
			/* For robust implementation, add here time-out management */
	}
	ADC1->CFGR1 &= ~ADC_CFGR1_DMAEN; /* (3) */
	ADC1->CR |= ADC_CR_ADCAL; /* (4) */
	while ((ADC1->CR & ADC_CR_ADCAL) != 0) /* (5) */
	{
			/* For robust implementation, add here time-out management */
	}


	//aktywowanie
	/* (1) Ensure that ADRDY = 0 */
	/* (2) Clear ADRDY */
	/* (3) Enable the ADC */
	/* (4) Wait until ADC ready */
	if ((ADC1->ISR & ADC_ISR_ADRDY) != 0) /* (1) */
	{
			ADC1->ISR |= ADC_ISR_ADRDY; /* (2) */
	}
	ADC1->CR |= ADC_CR_ADEN; /* (3) */
	while ((ADC1->ISR & ADC_ISR_ADRDY) == 0) /* (4) */
	{
			/* For robust implementation, add here time-out management */
	}

	//konfiguracja
	/* (2) Select the external trigger on rising edge and external trigger on
	TIM3_TRGO */
	//ADC1->CFGR1 |= ADC_CFGR1_EXTEN_0 | ADC_CFGR1_EXTSEL_0 | ADC_CFGR1_EXTSEL_1; /* (2) */
	/* TIM1_TRGO */
	ADC1->CFGR1 |= ADC_CFGR1_EXTEN_0;
	ADC1->CFGR1 &= ~ADC_CFGR1_EXTSEL;
	//select ADC channel
	ADC1->CHSELR = (1<<ADC_CHANNEL); // | ADC_CHSELR_CHSEL16; //kanał 16 czujnik temp.
	//6-bit resolution
	ADC1->CFGR1 |= ADC_CFGR1_RES;
	//sampling time 7.5cykla + 12.5c + 2.75c (trig latency) -> 5,7µs przy 4MHz fadc
	ADC1->SMPR = ADC_SMPR1_SMPR_0;
	//wait mode
	ADC1->CFGR1 |= ADC_CFGR1_WAIT;
	//aktywacja przerwania od zakończenia konwersji i zakończenia sekwencji
	ADC1->IER = ADC_IER_EOCIE; //| ADC_IER_EOSEQIE;

}

void BSP_adcStart(uint32_t prio)
{
	/* Configure NVIC for ADC */
	/* (1) Enable Interrupt on ADC */
	/* (2) Set priority for ADC */
	NVIC_EnableIRQ(ADC1_IRQn); /* (1) */
	NVIC_SetPriority(ADC1_IRQn, prio ); /* (2) */

	//Start pomiarów
	ADC1->CR |= ADC_CR_ADSTART;
}


//Wyzwalacz dla ADC
//void BSP_TIM1Init(void)
void BSP_adcTriggerInit(void)
{

	RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
	/* (2) Configure TIMy in slave mode using ITR2 (TIM3) as internal trigger
	by writing TS = 010 in TIMy_SMCR
	Configure TIMy in external clock mode 1, by writing SMS=111 in the
	TIMy_SMCR register. */
	TIM1->SMCR |= TIM_SMCR_TS_1 |
			TIM_SMCR_SMS_2 | TIM_SMCR_SMS_1 | TIM_SMCR_SMS_0; /* (2) */
	//TRGO on update -> ADC trigger
	TIM1->CR2 |= TIM_CR2_MMS_1;

	TIM1->ARR =  (uint16_t)((OE_TICKS_PER_SEC / ADC_READS_PER_SEC) - 1);

	/* Enable counter (CEN = 1)*/
	TIM1->CR1 |= TIM_CR1_CEN;

}


//PWM wyświetlacza oraz trigger dla TIM1
//void BSP_TIM3Init(void)
void BSP_pwmTimerInit(void)
{

	RCC->AHBENR |= AHBGPIOxEN(OE_GPIO);
	GPIO_Config(GPIO(OE_GPIO), OE_PIN, ALT_1);

	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	TIM3->PSC = (uint16_t)((SystemCoreClock / OE_TICKS_PER_SEC / TIM_GRAN) - 1);
	TIM3->ARR = TIM_GRAN - 1;
	//początkowa jasność
	//TIM3->CCR1 = TIM_GRAN/5;
	TIM3->CCR1 = 0;
	/* (4) Select PWM mode 1 on OC1 (OC1M = 110),
	enable preload register on OC1 (OC1PE = 1) */
	/* (5) Select active low polarity on OC1 (CC1P = 1),
	enable the output on OC1 (CC1E = 1)*/
	/* (6) Enable output (MOE = 1) unrelevant for TIM3*/
	TIM3->CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1
					| TIM_CCMR1_OC1PE; /* (4) */
	TIM3->CCER |= TIM_CCER_CC1E | TIM_CCER_CC1P; /* (5) */
	//Wyzwalacz dla TIM1
	//TIM3 Compare Pulse TRGO
	TIM3->CR2 |= TIM_CR2_MMS_0 | TIM_CR2_MMS_1;
	/* (7) Enable counter (CEN = 1)
	select edge aligned mode (CMS = 00, reset value)
	select direction as upcounter (DIR = 0, reset value) */
	/* (8) Force update generation (UG = 1) */
	TIM3->CR1 |= TIM_CR1_CEN; /* (7) */
	TIM3->EGR |= TIM_EGR_UG; /* (8) */

}


void BSP_spiInit(void)
{

	RCC->AHBENR |= AHBGPIOxEN(SPI_GPIO);
	GPIO_Config(GPIO(SPI_GPIO), SPI_NSS_PIN, ALT_0);
	GPIO_Config(GPIO(SPI_GPIO), SPI_SCK_PIN, ALT_0);
	GPIO_Config(GPIO(SPI_GPIO), SPI_MOSI_PIN, ALT_0);

	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
	/* (1) Master selection, BR: Fpclk/2
	 * CPOL and CPHA at zero (rising first edge) */
	/* (2) Slave select output enabled, pulse between data in continous tranmission
	 * RXNE IT, 16 bit data size */
	/* (3) Enable SPI1 */
	SPI1->CR1 = SPI_CR1_MSTR /*| SPI_CR1_BR*/; /* (1) */
	SPI1->CR2 = SPI_CR2_SSOE | SPI_CR2_DS | SPI_CR2_NSSP; /* (2) */
	SPI1->CR1 |= SPI_CR1_SPE; /* (3) */
}

void BSP_vwTimerInit(void)
{
	//konfiguracja pinu wejściowego
	RCC->AHBENR |= AHBGPIOxEN(VW_GPIO);
	GPIO_Config(GPIO(VW_GPIO), VW_PIN, INPUT);

	RCC->APB1ENR |= RCC_APB1ENR_TIM14EN;

	TIM14->ARR = ((SystemCoreClock / VW_SPEED / 8) - 1); //8 sampli na bit
	TIM14->DIER = TIM_DIER_UIE;

}

void BSP_vwTimerStart(uint32_t prio)
{
	NVIC_EnableIRQ(TIM14_IRQn); /* (1) */
	NVIC_SetPriority(TIM14_IRQn, prio ); /* (2) */

	TIM14->CR1 = TIM_CR1_CEN;
}

//UART1, przepisane żywcem z przykładów QP/C

#define __DIV(__PCLK, __BAUD)       (((__PCLK / 4) *25)/(__BAUD))
#define __DIVMANT(__PCLK, __BAUD)   (__DIV(__PCLK, __BAUD)/100)
#define __DIVFRAQ(__PCLK, __BAUD)   \
    (((__DIV(__PCLK, __BAUD) - (__DIVMANT(__PCLK, __BAUD) * 100)) \
        * 16 + 50) / 100)
#define __USART_BRR(__PCLK, __BAUD) \
    ((__DIVMANT(__PCLK, __BAUD) << 4)|(__DIVFRAQ(__PCLK, __BAUD) & 0x0F))

void BSP_uartInit(void)
{
	RCC->AHBENR |= AHBGPIOxEN(UART_GPIO);
	/* Configure PA3 to USART2_RX, PA2 to USART2_TX */
	GPIO_Config(GPIO(UART_GPIO), UART_RX_PIN, ALT_1);
	GPIO_Config(GPIO(UART_GPIO), UART_TX_PIN, ALT_1);

  /* enable peripheral clock for USART1 */
   RCC->APB2ENR |= RCC_APB2ENR_USART1EN;   /* Enable USART#1 clock */

   USART1->BRR  = __USART_BRR(SystemCoreClock, 115200ul);  /* baud rate */
   /* enable RX       *//* enable TX       *//* enable USART    */
   USART1->CR1  = USART_CR1_RE | USART_CR1_TE | USART_CR1_UE;

}
