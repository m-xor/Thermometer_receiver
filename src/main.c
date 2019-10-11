/*****************************************************************************
* Product: DPP example
* Last Updated for Version: 6.2.0
* Date of the Last Update:  2018-03-10
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
* https://www.state-machine.com
* mailto:info@state-machine.com
*****************************************************************************/

#include "qpc.h"
#include "protocol.h"
#include "project.h"
#include "bsp.h"
#include "receiver.h"
#include "serdisp.h"
#include "timer.h"
#include "control.h"
#include "data.h"
#include "button.h"


Q_DEFINE_THIS_FILE

/* todo: tune the size of queues and pools which are way too large now (see
 *    QEQueueCtr and QMPoolCtr)
 */
enum {
	dummy_PRIO,	//placeholder
	//priorytety zadań rosnąco
	AO_Control_PRIO,
	theTicker0_PRIO,
	AO_Data_PRIO,
	AO_SerDisp_PRIO,
	AO_RcvBatDec_PRIO,
	AO_RcvTempDec_PRIO,
	AO_RcvMsgDec_PRIO,
	AO_Button_PRIO,
	AO_Rcv_PRIO

};

/*..........................................................................*/
int main(int argc, char *argv[]) {
	(void)argc; (void)argv;

		//kolejki dla AOs
		static QEvt const *displayQueueSto[10];
    static QEvt const *rcvQueueSto[10];
    static QEvt const *rcvMsgDecQueueSto[10];
    static QEvt const *rcvBatDecQueueSto[10];
    static QEvt const *rcvTempDecQueueSto[10];
    static QEvt const *controlQueueSto[10];
    static QEvt const *dataQueueSto[10];
    static QEvt const *buttonQueueSto[10];


    static QSubscrList subscrSto[MAX_PUB_SIG];

    //pula pamięci dla eventów
    static QF_MPOOL_EL(ValEvt) smlPoolSto[10]; /* small pool */
    static QF_MPOOL_EL(MsgEvt) bigPoolSto[MSG_REPEAT_NUM];	//tyle przewidzianych powtórek komunikatu

    /* instantiate active objects */

    QTicker_ctor(the_Ticker0, SOFT_TICKRATE_0);
    Rcv_ctor();
    RcvMsgDec_ctor();
    RcvBatDec_ctor();
    RcvTempDec_ctor();
    SerDisp_ctor();
    Control_ctor();
    Data_ctor();
    Button_ctor();

    QF_init();    /* initialize the framework and the underlying RT kernel */
    BSP_init(); /* initialize the Board Support Package */

    /* object dictionaries... */
    QS_OBJ_DICTIONARY(smlPoolSto);
    QS_OBJ_DICTIONARY(bigPoolSto);

    QS_OBJ_DICTIONARY(displayQueueSto);
    QS_OBJ_DICTIONARY(rcvQueueSto);
    QS_OBJ_DICTIONARY(rcvMsgDecQueueSto);
    QS_OBJ_DICTIONARY(rcvBatDecQueueSto);
    QS_OBJ_DICTIONARY(rcvTempDecQueueSto);


    //signal dictionaries
    QS_SIG_DICTIONARY(OTEMP_SIG,  (void *)0);
    QS_SIG_DICTIONARY(BATT_SIG,  (void *)0);
    QS_SIG_DICTIONARY(TX_ERR_SIG,  (void *)0);
    QS_SIG_DICTIONARY(ERR_SIG,  (void *)0);
    QS_SIG_DICTIONARY(TX_HALT_SIG,  (void *)0);

    QS_SIG_DICTIONARY(VW_SIG,  (void *)0);
    QS_SIG_DICTIONARY(MSG_SIG,  (void *)0);

    QS_SIG_DICTIONARY(TIMEOUT_SIG,  (void *)0);
    QS_SIG_DICTIONARY(DISP_BLINK_SIG,  (void *)0);
    QS_SIG_DICTIONARY(ORTHO_BLINK_SIG,  (void *)0);

    QS_SIG_DICTIONARY(BTN_PRSS_SIG,  (void *)0);
    QS_SIG_DICTIONARY(BTN_REL_SIG,  (void *)0);
    QS_SIG_DICTIONARY(BTN_SHORT_SIG,  (void *)0);
    QS_SIG_DICTIONARY(BTN_EVT_SIG,  (void *)0);

    QS_SIG_DICTIONARY(DELAY_TIMEOUT_SIG,  (void *)0);

    QS_SIG_DICTIONARY(MIN_TEMP_SIG,  (void *)0);
    QS_SIG_DICTIONARY(MAX_TEMP_SIG,  (void *)0);
    QS_SIG_DICTIONARY(UPDATE_MINMAX_SIG,  (void *)0);

    QS_SIG_DICTIONARY(RET_ORTHO_SIG,  (void *)0);
    QS_SIG_DICTIONARY(MINMAX_TIMEOUT_SIG,  (void *)0);



    /* initialize publish-subscribe... */
    QF_psInit(subscrSto, Q_DIM(subscrSto));

    /* initialize event pools... max QF_MAX_EPOOL in *ascending* order of the event size */
    QF_poolInit(smlPoolSto, sizeof(smlPoolSto), sizeof(smlPoolSto[0]));
    QF_poolInit(bigPoolSto, sizeof(bigPoolSto), sizeof(bigPoolSto[0]));

    /* start the active objects... */

    //-------- software timer

    QACTIVE_START(the_Ticker0,
									(uint_fast8_t)(theTicker0_PRIO),
                      0, 0, 0, 0, 0);    /* no queue, no stack , no init. event */

    //------- serial display

    QACTIVE_START(AO_SerDisp,                  /* AO to start */
                          (uint_fast8_t)(AO_SerDisp_PRIO), /* QP priority of the AO */
                          displayQueueSto,             /* event queue storage */
                          Q_DIM(displayQueueSto),      /* queue length [events] */
                          (void *)0,                 /* stack storage (not used) */
                          0U,                        /* size of the stack [bytes] */
                          (QEvt *)0);                /* initialization event */

    //------- receiver

    QACTIVE_START(AO_Rcv,                  /* AO to start */
                  (uint_fast8_t)(AO_Rcv_PRIO), /* QP priority of the AO */
                  rcvQueueSto,             /* event queue storage */
                  Q_DIM(rcvQueueSto),      /* queue length [events] */
                  (void *)0,                 /* stack storage (not used) */
                  0U,                        /* size of the stack [bytes] */
                  (QEvt *)0);                /* initialization event */


    QACTIVE_START(AO_RcvMsgDec,                  /* AO to start */
                  (uint_fast8_t)(AO_RcvMsgDec_PRIO), /* QP priority of the AO */
									rcvMsgDecQueueSto,             /* event queue storage */
                  Q_DIM(rcvMsgDecQueueSto),      /* queue length [events] */
                  (void *)0,                 /* stack storage (not used) */
                  0U,                        /* size of the stack [bytes] */
                  (QEvt *)0);                /* initialization event */

    QACTIVE_START(AO_RcvBatDec,                  /* AO to start */
                  (uint_fast8_t)(AO_RcvBatDec_PRIO), /* QP priority of the AO */
									rcvBatDecQueueSto,             /* event queue storage */
                  Q_DIM(rcvBatDecQueueSto),      /* queue length [events] */
                  (void *)0,                 /* stack storage (not used) */
                  0U,                        /* size of the stack [bytes] */
                  (QEvt *)0);                /* initialization event */

    QACTIVE_START(AO_RcvTempDec,                  /* AO to start */
                  (uint_fast8_t)(AO_RcvTempDec_PRIO), /* QP priority of the AO */
									rcvTempDecQueueSto,             /* event queue storage */
                  Q_DIM(rcvTempDecQueueSto),      /* queue length [events] */
                  (void *)0,                 /* stack storage (not used) */
                  0U,                        /* size of the stack [bytes] */
                  (QEvt *)0);                /* initialization event */


    //-------------------
    QACTIVE_START(AO_Control,                  /* AO to start */
                      (uint_fast8_t)(AO_Control_PRIO), /* QP priority of the AO */
											controlQueueSto,             /* event queue storage */
                      Q_DIM(controlQueueSto),      /* queue length [events] */
                      (void *)0,                 /* stack storage (not used) */
                      0U,                        /* size of the stack [bytes] */
                      (QEvt *)0);                /* initialization event */

    //-------------------
    QACTIVE_START(AO_Data,                  /* AO to start */
                      (uint_fast8_t)(AO_Data_PRIO), /* QP priority of the AO */
											dataQueueSto,             /* event queue storage */
                      Q_DIM(dataQueueSto),      /* queue length [events] */
                      (void *)0,                 /* stack storage (not used) */
                      0U,                        /* size of the stack [bytes] */
                      (QEvt *)0);                /* initialization event */
    //-------------------
    QACTIVE_START(AO_Button,                  /* AO to start */
                      (uint_fast8_t)(AO_Button_PRIO), /* QP priority of the AO */
											buttonQueueSto,             /* event queue storage */
                      Q_DIM(buttonQueueSto),      /* queue length [events] */
                      (void *)0,                 /* stack storage (not used) */
                      0U,                        /* size of the stack [bytes] */
                      (QEvt *)0);                /* initialization event */

    return QF_run(); /* run the QF application */
}

