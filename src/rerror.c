/*
 * rerror.c
 *
 *  Created on: 21.07.2019
 *      Author: slawek
 */


#include "rerror.h"
#include <stddef.h>

#define ERR_MARK 0xdefec700

typedef struct RError_T {

	uint32_t mark;
	uint8_t const *file;
	uint32_t line;

} RError_T;

__attribute__((section(".noinit")))
static RError_T rerror;


void rerr_setErr(uint8_t const * restrict file, uint32_t line)
{
	rerror.mark=ERR_MARK;
	rerror.file=file;
	rerror.line=line;
}

bool rerr_getErr(uint8_t const ** restrict file, uint32_t * restrict line)
{
	bool res = false;

	if(rerror.mark==ERR_MARK)
	{
		res = true;
		*file = rerror.file;
		*line = rerror.line;
	}

	return res;
}

void rerr_reset(void)
{
	rerror.mark = 0U;
}
