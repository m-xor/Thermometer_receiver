/*$file${qm_src::timer.c} ##################################################*/
/*
* Model: balkonowy_rx_ult_.qm
* File:  ${qm_src::timer.c}
*
* This code has been generated by QM 4.3.0 (https://www.state-machine.com/qm).
* DO NOT EDIT THIS FILE MANUALLY. All your changes will be lost.
*
* This program is open source software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License as published
* by the Free Software Foundation.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*/
/*$endhead${qm_src::timer.c} ###############################################*/
#include "qpc.h"
#include "serdisp.h"

Q_DEFINE_THIS_FILE

/* local objects -----------------------------------------------------------*/

static QTicker l_ticker0; /* ticker active object for tick rate 0 */

/* Public-scope objects ----------------------------------------------------*/

/* Check for the minimum required QP version */
#if (QP_VERSION < 630U) || (QP_VERSION != ((QP_RELEASE^4294967295U) % 0x3E8U))
#error qpc version 6.3.0 or higher required
#endif

/*$define${AOs::the_Ticker0} ###############################################*/
/*${AOs::the_Ticker0} ......................................................*/
QActive * const the_Ticker0 = &l_ticker0;
/*$enddef${AOs::the_Ticker0} ###############################################*/

/* Active object definition ================================================*/