/*
 * datahist.h
 *
 *  Created on: 06.10.2019
 *      Author: slawek
 */

#ifndef DATAHIST_H_
#define DATAHIST_H_

#include <stdint.h>

#define DROP_MARK (INT16_MIN+1)

extern void data_init(void);
extern void data_save(int16_t value);
extern void data_get(int16_t *min, int16_t *max);

#endif /* DATAHIST_H_ */
