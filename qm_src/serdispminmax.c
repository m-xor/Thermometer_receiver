/*$file${qm_src::serdispminmax.c} ##########################################*/
/*
* Model: balkonowy_rx_ult_.qm
* File:  ${qm_src::serdispminmax.c}
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
/*$endhead${qm_src::serdispminmax.c} #######################################*/
#include "qpc.h"
#include "bsp.h"
#include "project.h"
#include "serdispminmax.h"
//#include "link-proto.h"
#include "led7seg.h"
#include "err.h"
#include "rerror.h"

Q_DEFINE_THIS_FILE

#define NAN ONE_QUESTION

#define MIN_DESC_TICKS    (MIN_DESC_MS*BSP_TICKS_PER_SEC/1000)
#define MIN_TICKS        (MIN_MS*BSP_TICKS_PER_SEC/1000)
#define MAX_DESC_TICKS    (MAX_DESC_MS*BSP_TICKS_PER_SEC/1000)
#define MAX_TICKS        (MAX_MS*BSP_TICKS_PER_SEC/1000)
#define TREND_TICKS    (TREND_MS*BSP_TICKS_PER_SEC/1000)

/* local objects -----------------------------------------------------------*/

/*$declare${AOs::SerDispMinMax} ############################################*/
/*${AOs::SerDispMinMax} ....................................................*/
typedef struct {
/* protected: */
    QHsm super;

/* private: */
    int16_t minRound;
    int16_t maxRound;
    QTimeEvt timeoutEvt;
    QActive  * superAO;
} SerDispMinMax;

/* protected: */
static QState SerDispMinMax_initial(SerDispMinMax * const me, QEvt const * const e);
static QState SerDispMinMax_container(SerDispMinMax * const me, QEvt const * const e);
static QState SerDispMinMax_idle(SerDispMinMax * const me, QEvt const * const e);
static QState SerDispMinMax_floor(SerDispMinMax * const me, QEvt const * const e);
static QState SerDispMinMax_min(SerDispMinMax * const me, QEvt const * const e);
static QState SerDispMinMax_ceil(SerDispMinMax * const me, QEvt const * const e);
static QState SerDispMinMax_max(SerDispMinMax * const me, QEvt const * const e);
static QState SerDispMinMax_trend(SerDispMinMax * const me, QEvt const * const e);
/*$enddecl${AOs::SerDispMinMax} ############################################*/
static SerDispMinMax l_serDispMinMax; /* the sole instance of the SerDisp active object */


/* Public-scope objects ----------------------------------------------------*/

/* Active object definition ================================================*/

/* Check for the minimum required QP version */
#if (QP_VERSION < 630U) || (QP_VERSION != ((QP_RELEASE^4294967295U) % 0x3E8U))
#error qpc version 6.3.0 or higher required
#endif

/*$define${AOs::SerDispMinMax_ctor} ########################################*/
/*${AOs::SerDispMinMax_ctor} ...............................................*/
QHsm * SerDispMinMax_ctor(QActive * const superAO) {
    SerDispMinMax *me = &l_serDispMinMax;

    /* superclass' ctor */
    QHsm_ctor(&me->super, Q_STATE_CAST(&SerDispMinMax_initial));
    /* superAO will dispatch event down to me */
    QTimeEvt_ctorX(&me->timeoutEvt, superAO, MINMAX_TIMEOUT_SIG, HARD_TICKRATE_1);

    me->minRound = NAN;
    me->maxRound = NAN;
    me->superAO = superAO;

    return (QHsm *)me;
}
/*$enddef${AOs::SerDispMinMax_ctor} ########################################*/
/*$define${AOs::SerDispMinMax} #############################################*/
/*${AOs::SerDispMinMax} ....................................................*/
/*${AOs::SerDispMinMax::SM} ................................................*/
static QState SerDispMinMax_initial(SerDispMinMax * const me, QEvt const * const e) {
    /*${AOs::SerDispMinMax::SM::initial} */
    (void)e;

    QS_OBJ_DICTIONARY(&l_serDispMinMax);

    QS_FUN_DICTIONARY(&SerDispMinMax_container);
    QS_FUN_DICTIONARY(&SerDispMinMax_idle);
    QS_FUN_DICTIONARY(&SerDispMinMax_floor);
    QS_FUN_DICTIONARY(&SerDispMinMax_min);
    QS_FUN_DICTIONARY(&SerDispMinMax_ceil);
    QS_FUN_DICTIONARY(&SerDispMinMax_max);
    QS_FUN_DICTIONARY(&SerDispMinMax_trend);

    return Q_TRAN(&SerDispMinMax_idle);
}
/*${AOs::SerDispMinMax::SM::container} .....................................*/
static QState SerDispMinMax_container(SerDispMinMax * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /*${AOs::SerDispMinMax::SM::container::MIN_TEMP} */
        case MIN_TEMP_SIG: {
            me->minRound = Q_EVT_CAST(ValEvt)->value;
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::SerDispMinMax::SM::container::MAX_TEMP} */
        case MAX_TEMP_SIG: {
            me->maxRound = Q_EVT_CAST(ValEvt)->value;
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::SerDispMinMax::SM::container::BTN_PRSS} */
        case BTN_PRSS_SIG: {
            status_ = Q_TRAN(&SerDispMinMax_idle);
            break;
        }
        default: {
            status_ = Q_SUPER(&QHsm_top);
            break;
        }
    }
    return status_;
}
/*${AOs::SerDispMinMax::SM::container::idle} ...............................*/
static QState SerDispMinMax_idle(SerDispMinMax * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /*${AOs::SerDispMinMax::SM::container::idle} */
        case Q_ENTRY_SIG: {
            QACTIVE_POST(me->superAO,Q_NEW(QEvt, RET_ORTHO_SIG), me);
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::SerDispMinMax::SM::container::idle::BTN_SHORT} */
        case BTN_SHORT_SIG: {
            status_ = Q_TRAN(&SerDispMinMax_floor);
            break;
        }
        default: {
            status_ = Q_SUPER(&SerDispMinMax_container);
            break;
        }
    }
    return status_;
}
/*${AOs::SerDispMinMax::SM::container::floor} ..............................*/
static QState SerDispMinMax_floor(SerDispMinMax * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /*${AOs::SerDispMinMax::SM::container::floor} */
        case Q_ENTRY_SIG: {
            QTimeEvt_armX(&me->timeoutEvt,  MIN_DESC_TICKS, 0U);

            led_msg(FLOOR_MSG);
            led_show();
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::SerDispMinMax::SM::container::floor::MINMAX_TIMEOUT} */
        case MINMAX_TIMEOUT_SIG: {
            status_ = Q_TRAN(&SerDispMinMax_min);
            break;
        }
        default: {
            status_ = Q_SUPER(&SerDispMinMax_container);
            break;
        }
    }
    return status_;
}
/*${AOs::SerDispMinMax::SM::container::min} ................................*/
static QState SerDispMinMax_min(SerDispMinMax * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /*${AOs::SerDispMinMax::SM::container::min} */
        case Q_ENTRY_SIG: {
            QTimeEvt_armX(&me->timeoutEvt,  MIN_TICKS, 0U);

            led_display_number(me->minRound, 10);
            led_show();
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::SerDispMinMax::SM::container::min::MINMAX_TIMEOUT} */
        case MINMAX_TIMEOUT_SIG: {
            status_ = Q_TRAN(&SerDispMinMax_ceil);
            break;
        }
        default: {
            status_ = Q_SUPER(&SerDispMinMax_container);
            break;
        }
    }
    return status_;
}
/*${AOs::SerDispMinMax::SM::container::ceil} ...............................*/
static QState SerDispMinMax_ceil(SerDispMinMax * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /*${AOs::SerDispMinMax::SM::container::ceil} */
        case Q_ENTRY_SIG: {
            QTimeEvt_armX(&me->timeoutEvt,  MAX_DESC_TICKS, 0U);

            led_msg(CEIL_MSG);
            led_show();
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::SerDispMinMax::SM::container::ceil::MINMAX_TIMEOUT} */
        case MINMAX_TIMEOUT_SIG: {
            status_ = Q_TRAN(&SerDispMinMax_max);
            break;
        }
        default: {
            status_ = Q_SUPER(&SerDispMinMax_container);
            break;
        }
    }
    return status_;
}
/*${AOs::SerDispMinMax::SM::container::max} ................................*/
static QState SerDispMinMax_max(SerDispMinMax * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /*${AOs::SerDispMinMax::SM::container::max} */
        case Q_ENTRY_SIG: {
            QTimeEvt_armX(&me->timeoutEvt,  MAX_TICKS, 0U);

            led_display_number(me->maxRound, 10);
            led_show();
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::SerDispMinMax::SM::container::max::MINMAX_TIMEOUT} */
        case MINMAX_TIMEOUT_SIG: {
            status_ = Q_TRAN(&SerDispMinMax_trend);
            break;
        }
        default: {
            status_ = Q_SUPER(&SerDispMinMax_container);
            break;
        }
    }
    return status_;
}
/*${AOs::SerDispMinMax::SM::container::trend} ..............................*/
static QState SerDispMinMax_trend(SerDispMinMax * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /*${AOs::SerDispMinMax::SM::container::trend} */
        case Q_ENTRY_SIG: {
            QTimeEvt_armX(&me->timeoutEvt,  TREND_TICKS, 0U);

            led_msg(FLAT_TREND_MSG);
            led_show();
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::SerDispMinMax::SM::container::trend::MINMAX_TIMEOUT} */
        case MINMAX_TIMEOUT_SIG: {
            status_ = Q_TRAN(&SerDispMinMax_idle);
            break;
        }
        default: {
            status_ = Q_SUPER(&SerDispMinMax_container);
            break;
        }
    }
    return status_;
}
/*$enddef${AOs::SerDispMinMax} #############################################*/
