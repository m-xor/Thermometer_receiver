/*
 * datahist.c
 *
 *  Created on: 06.10.2019
 *      Author: slawek
 */

#include "app_conf.h"
#include "datahist.h"


#define DATA_STOR_SIZE    (((SAVED_DATA_SCOPE*60)*60/BSP_MSG_INTERVAL))
//#define DATA_STOR_SIZE    (5) //test

static int16_t storage[DATA_STOR_SIZE];
static uint16_t current_index;


void data_init(void)
{

	for(int16_t i=0; i<DATA_STOR_SIZE; i++)
	{
	    storage[i] = DROP_MARK;
	}

}

void data_save(int16_t value)
{
	storage[current_index++] =  value;
	if(current_index >= DATA_STOR_SIZE)
	    current_index = 0;
}

void data_get(int16_t *min, int16_t *max)
{
	int16_t tmp_min = INT16_MAX, tmp_max = INT16_MIN;

	for(uint16_t i=0; i<DATA_STOR_SIZE; i++)
	{
	    if( storage[i] < tmp_min && storage[i] !=DROP_MARK )
	        tmp_min = storage[i];
	    if( storage[i] > tmp_max && storage[i] !=DROP_MARK )
	        tmp_max = storage[i];
	}

	*min = tmp_min==INT16_MAX ? DROP_MARK : tmp_min;
	*max = tmp_max==INT16_MIN ? DROP_MARK : tmp_max;

}
