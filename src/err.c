/*
 * err.c
 *
 *  Created on: 22.07.2019
 *      Author: slawek
 */

#include "stm32f0xx.h"
#include "rerror.h"
#include "err.h"
#include <string.h>

//exception handlers in vector1.c set these
#define ERR_NMI_STR	"NMI"
#define ERR_STACK_OVF_STR	"StackOverflow"
#define ERR_HARD_FAULT_STR	"HardFault"
#define ERR_DEFAULT_HANDLER_STR	"Default"

//check both independent and window watchdogs
#define ERR_WD_MSK (RCC_CSR_IWDGRSTF|RCC_CSR_WWDGRSTF)

uint8_t errCheckNGetCode(void)
{

  uint8_t const *f;
  uint32_t l;
	uint8_t res = ERR_NOERR;
	uint32_t rcc_csr = RCC->CSR & RCC_CSR_LSION;	//rest of the flags are RO

	//check watchdog reset first
	if(RCC->CSR & ERR_WD_MSK)
	{
		res = ERR_WD_RESET;
	}

	if(rerr_getErr(&f, &l))
	{

		if(!strcmp(ERR_NMI_STR,(char const *)f))
		{
				res |= ERR_NMI;
		}
		else if(!strcmp(ERR_STACK_OVF_STR,(char const *)f))
		{
				res |= ERR_STACK_OVF;
		}
		else if(!strcmp(ERR_HARD_FAULT_STR,(char const *)f))
		{
				res |= ERR_HARD_FAULT;
		}
		else if(!strcmp(ERR_DEFAULT_HANDLER_STR,(char const *)f))
		{
				res |= ERR_DEFAULT_HANDLER;
		}
		else
		{
				res |= ERR_ASSERT_FAILED;
		}

	}

	//clear reset source flags
	RCC->CSR = rcc_csr | RCC_CSR_RMVF;

	return res;
}
