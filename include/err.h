/*
 * err.h
 *
 *  Created on: 22.07.2019
 *      Author: slawek
 */

#ifndef ERR_H_
#define ERR_H_

#include <stdint.h>

#define ERR_NOERR						((uint8_t)0U)	//none of following errors was cause of last reset
#define ERR_NMI							((uint8_t)1U)	//NMI handler was called
#define ERR_STACK_OVF				((uint8_t)2U)	//stack overflowed
#define ERR_HARD_FAULT			((uint8_t)4U)	//hard fault handler was called
#define ERR_DEFAULT_HANDLER	((uint8_t)8U)	//undefined interrupt handler was called
#define ERR_ASSERT_FAILED		((uint8_t)16U)//various internal errors of application, check it by rerr_getErr(&f, &l)
#define ERR_WD_RESET				((uint8_t)32U)//watchdog timeout occured

//returns code of error which has induced last reset
//as a side effect clears reset flags of RCC->CSR
uint8_t errCheckNGetCode(void);

#endif /* ERR_H_ */
