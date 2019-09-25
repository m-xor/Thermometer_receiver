/*****************************************************************************
* Product: DPP example
* Last Updated for Version: 5.9.9
* Date of the Last Update:  2017-10-09
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) Quantum Leaps, LLC. All rights reserved.
*
* This program is open source software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License as published
* by the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* Alternatively, this program may be distributed and modified under the
* terms of Quantum Leaps commercial licenses, which expressly supersede
* the GNU General Public License and are specifically designed for
* licensees interested in retaining the proprietary status of their code.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*
* Contact information:
* https://state-machine.com
* mailto:info@state-machine.com
*****************************************************************************/
#ifndef bsp_h
#define bsp_h

#include "app_conf.h"

#ifdef Q_SPY
    enum AppRecords { /* application-specific trace records */
        APP_STAT = QS_USER,
        COMMAND_STAT,
				LOG_STAT,
				SYSTICK_STAT,
				VW_STAT,
				DUP_STAT,
				VALERR_STAT,
				HALT_STAT
    };
#endif

void BSP_init(void);
uint32_t BSP_getTimestamp(void);

//extern QActive * const the_Ticker0;

#endif /* bsp_h */
