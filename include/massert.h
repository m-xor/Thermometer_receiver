/*
 * massert.h
 *
 *  Created on: 20.07.2019
 *      Author: slawek
 */

#ifndef MASSERT_H_
#define MASSERT_H_

#include <stdint.h>





#ifdef NDEBUG

#define DEFINE_THIS_FILE
#define THIS_FILE
#define ASSERT(a) ((void)0)

#else

extern void assert_failed(uint8_t * file, uint32_t line);

#define DEFINE_THIS_FILE static uint8_t const * const this_file = (uint8_t const *)__FILE__;
#define THIS_FILE this_file
#define ASSERT(a) ((a) ? (void)0 : assert_failed((uint8_t *)this_file, __LINE__))

#endif

#endif /* MASSERT_H_ */
