/*
 * rerror.h
 *
 *  Created on: 21.07.2019
 *      Author: slawek
 */

#ifndef RERROR_H_
#define RERROR_H_

#include <stdint.h>
#include <stdbool.h>

extern void rerr_setErr(uint8_t const * restrict file, uint32_t line);
extern bool rerr_getErr(uint8_t const ** restrict file, uint32_t * restrict line);
extern void rerr_reset(void);

#endif /* RERROR_H_ */
