/*
 * app_conf.h
 *
 *  Created on: 01.07.2019
 *      Author: slawek
 */

#ifndef APP_CONF_H_
#define APP_CONF_H_

/*
 * Hardware, fast timer speed
 */
#define BSP_TICKS_PER_SEC   100U
/*
 * Software timer speed
 */
#define BSP_SOFT_TIMER_TICKS_PER_SEC 1U
/*
 * Timestamp granularity
 * Used in RcvMsgDec_hasIntervalElapsed()
 */
#define BSP_TIMESTAMP_TICKS_PER_SEC 10U

/*
 * Transmitter firmware file
 * For DATA_INTERVAL
 */
#include "power_manage_conf.h"
/*
 * Interval between new messages sent by transmitter
 * in seconds. (Time stretch transmitter processes conversion and transmission
 * is aprox. 30sec)
 * Used in RcvMsgDec_hasIntervalElapsed()
 */
#define BSP_MSG_INTERVAL	(DATA_INTERVAL + 30)
//#define BSP_MSG_INTERVAL	30	//test

/*
 * Tickrate index for software timer
 */
#define SOFT_TICKRATE_0 ((uint_fast8_t)0)
/*
 * Tickrate index for hardware, fast timer
 */
#define HARD_TICKRATE_1 ((uint_fast8_t)1)

/*
 * SerDisp.c
 */
//Czas od otrzymania komunikatu po upłynięciu którego
//dana będzie uznana za nieco przestarzałą, co będzie sygnalizowanie
//miganiem wyświetlacza. W minutach (max 1092)
#define DISPLAY_ASSUME_AGED    4	//trochę dłużej niż interwał między pomiarami
//Czas po otrzymaniu komunikatu po upłynięciu którego
//dana będzie uznana za nieaktualną i przestanie być wyświetlana
//W minutach (max 1092)
#define DISPLAY_ASSUME_OUTDATED  15	//W ciągu 15 minut temp. potrafi się zmienić o 1-2 stopni
/*
 * Częstotliwość mrugania wyświetlacza dla  DISPLAY_ASSUME_AGED
 */
#define DISPLAY_AGED_BLINK_PER_SEC	1U

/*
 * Sygnalizacja błędów - migający LED
 * Czas włączenia diody
 * w ms
 */
#define ERR_NOTICE_ON 10
/*
 * Czas wyłączenia diody
 * w ms
 */
#define ERR_NOTICE_OFF	2500

/*
 * Watchdog kicks timeout (Control task)
 * w ms
 */
#define BSP_WD_TIMEOUT 1500

/*
 * Range of time since when data is saved.
 * in hours
 */
//#define SAVED_DATA_SCOPE 24
#define SAVED_DATA_SCOPE 1

/*
 * Time button is to be pressed for "long press" event
 */
#define LONG_PRESS_STRETCH 500

#endif /* APP_CONF_H_ */
