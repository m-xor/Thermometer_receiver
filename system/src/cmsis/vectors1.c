/*
 * vectors.c
 *
 *  Created on: 20.03.2019
 *      Author: slawek
 *
 *	Contains vector table and exception handlers. Doesn't contain
 *	C initialization code. To be used together with GNU-ARM Eclipse plug-in.
 *	For STM32F103xB
 *
 *  Based on Modern Embedded Systems Programming Course lesson 15
 *  http://www.state-machine.com/quickstart/, CMSIS template and CubeMx startup
 *
 */

#include <stdint.h>

/*
 * External symbols
 */
/*
 * Provided by linker script
 */
extern uint32_t _estack;
extern uint32_t __stack_start__;
extern uint32_t __stack_end__;
/*
 * Function assert_failed must not use stack that can be corrupted at the moment
 * of calling
 */
extern void assert_failed(uint8_t * file, uint32_t line);
/*
 * Provided by plugin startup code
 */
extern void Reset_Handler(void);


/*
 * Prototypes
 */

__attribute__ ((section(".after_vectors"),noreturn,naked))
void Default_Handler		(void);

/*
 * Fault exceptions. Can be redefined in other files
 * (beware of CubeMx and GNU-ARM Eclipse plugin code)
 */
__attribute__ ((section(".after_vectors"),noreturn,naked))
void NMI_Handler			(void);
__attribute__ ((section(".after_vectors"),noreturn,naked))
void HardFault_Handler		(void);

/*
 * Not fault exceptions
 */
void SVC_Handler			(void) __attribute__ ((weak, alias("Default_Handler")));
void PendSV_Handler			(void) __attribute__ ((weak, alias("Default_Handler")));
void SysTick_Handler		(void) __attribute__ ((weak, alias("Default_Handler")));

/*
 * Interrupts
 */
void WWDG_IRQHandler		(void) __attribute__ ((weak, alias("Default_Handler")));
void RTC_IRQHandler			(void) __attribute__ ((weak, alias("Default_Handler")));
void FLASH_IRQHandler		(void) __attribute__ ((weak, alias("Default_Handler")));
void RCC_IRQHandler			(void) __attribute__ ((weak, alias("Default_Handler")));
void EXTI0_1_IRQHandler		(void) __attribute__ ((weak, alias("Default_Handler")));
void EXTI2_3_IRQHandler		(void) __attribute__ ((weak, alias("Default_Handler")));
void EXTI4_15_IRQHandler	(void) __attribute__ ((weak, alias("Default_Handler")));
void DMA1_Channel1_IRQHandler	(void) __attribute__ ((weak, alias("Default_Handler")));
void DMA1_Channel2_3_IRQHandler	(void) __attribute__ ((weak, alias("Default_Handler")));
void DMA1_Channel4_5_IRQHandler	(void) __attribute__ ((weak, alias("Default_Handler")));
void ADC1_IRQHandler			(void) __attribute__ ((weak, alias("Default_Handler")));
void TIM1_BRK_UP_TRG_COM_IRQHandler		(void) __attribute__ ((weak, alias("Default_Handler")));
void TIM1_CC_IRQHandler		(void) __attribute__ ((weak, alias("Default_Handler")));
void TIM3_IRQHandler		(void) __attribute__ ((weak, alias("Default_Handler")));
void TIM14_IRQHandler		(void) __attribute__ ((weak, alias("Default_Handler")));
void TIM15_IRQHandler		(void) __attribute__ ((weak, alias("Default_Handler")));
void TIM16_IRQHandler		(void) __attribute__ ((weak, alias("Default_Handler")));
void TIM17_IRQHandler		(void) __attribute__ ((weak, alias("Default_Handler")));
void I2C1_IRQHandler		(void) __attribute__ ((weak, alias("Default_Handler")));
void I2C2_IRQHandler		(void) __attribute__ ((weak, alias("Default_Handler")));
void SPI1_IRQHandler		(void) __attribute__ ((weak, alias("Default_Handler")));
void SPI2_IRQHandler		(void) __attribute__ ((weak, alias("Default_Handler")));
void USART1_IRQHandler		(void) __attribute__ ((weak, alias("Default_Handler")));
void USART2_IRQHandler		(void) __attribute__ ((weak, alias("Default_Handler")));


/*
 * Vector table
 */

union {
	uint32_t *ptr;
	void (*handler)(void);

} const isrVectors[] __attribute__((section(".isr_vector"))) = {

	{.ptr = &_estack},
	{.handler = Reset_Handler},
	{.handler = NMI_Handler},
	{.handler = HardFault_Handler},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{.handler = SVC_Handler},
	{0},
	{0},
	{.handler = PendSV_Handler},
	{.handler = SysTick_Handler},
	// Chip Level - STM32F030
	{.handler = WWDG_IRQHandler},
	{0},
	{.handler = RTC_IRQHandler},
	{.handler = FLASH_IRQHandler},
	{.handler = RCC_IRQHandler},
	{.handler = EXTI0_1_IRQHandler},
	{.handler = EXTI2_3_IRQHandler},
	{.handler = EXTI4_15_IRQHandler},
	{0},
	{.handler = DMA1_Channel1_IRQHandler},
	{.handler = DMA1_Channel2_3_IRQHandler},
	{.handler = DMA1_Channel4_5_IRQHandler},
	{.handler = ADC1_IRQHandler},
	{.handler = TIM1_BRK_UP_TRG_COM_IRQHandler},
	{.handler = TIM1_CC_IRQHandler},
	{0},
	{.handler = TIM3_IRQHandler},
	{0},
	{0},
	{.handler = TIM14_IRQHandler},
	{.handler = TIM15_IRQHandler},
	{.handler = TIM16_IRQHandler},
	{.handler = TIM17_IRQHandler},
	{.handler = I2C1_IRQHandler},
	{.handler = I2C2_IRQHandler},
	{.handler = SPI1_IRQHandler},
	{.handler = SPI2_IRQHandler},
	{.handler = USART1_IRQHandler}, //
	{.handler = USART2_IRQHandler},
	{0}, //dla f30xc USART3_6_IRQHandler,
	{0},
	{0},
	// @0x108. This is for boot in RAM mode for STM32F0xx devices.
	{.ptr=(uint32_t*)0xF108F85F}

};

/*
 * Default implementation of exception handlers assume that error which has occurred
 * might corrupt stack
 */

/*
 * Exception handlers
 */

//__attribute__ ((section(".after_vectors"),noreturn,naked))
void NMI_Handler(void)
{
    __asm volatile (
        "    ldr r0,=str_nmi\n\t"
        "    mov r1,#1\n\t"
        "    bl assert_failed\n\t"
        "str_nmi: .asciz \"NMI\"\n\t"
        "  .align 4\n\t"
    );
}

//__attribute__ ((section(".after_vectors"),noreturn,naked))
void HardFault_Handler(void)
{
	//assert_failed((uint8_t *)"HardFault", __LINE__);
	__asm volatile (
	        "    mov r0,sp\n\t"
	        "    ldr r1,=__stack_start__\n\t"
	        "    cmp r0,r1\n\t"
	        "    bcs stack_ok\n\t"
	//restore stack pointer to safely call C abort function
	        "    ldr r0,=__stack_end__\n\t"
	        "    mov sp,r0\n\t"
	        "    ldr r0,=str_overflow\n\t"
	        "    mov r1,#1\n\t"
	        "    bl assert_failed\n\t"
	        "stack_ok:\n\t"
	        "    ldr r0,=str_hardfault\n\t"
	        "    mov r1,#2\n\t"
	        "    bl assert_failed\n\t"
	        "str_overflow:  .asciz \"StackOverflow\"\n\t"
	        "str_hardfault: .asciz \"HardFault\"\n\t"
			"  .align 4\n\t"
			);
}

/*
 * Handler for unused interrupts
 */

//__attribute__ ((section(".after_vectors"),noreturn,naked))
void Default_Handler(void)
{
    __asm volatile (
        "    ldr r0,=str_dflt\n\t"
        "    mov r1,#1\n\t"
        "    bl assert_failed\n\t"
        "str_dflt: .asciz \"Default\"\n\t"
        "  .align 4\n\t"
    );
}
