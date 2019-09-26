/*$file${qm_src::control.c} ################################################*/
/*
* Model: balkonowy_rx_ult_.qm
* File:  ${qm_src::control.c}
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
/*$endhead${qm_src::control.c} #############################################*/
#include "qpc.h"
#include "bsp.h"
#include "project.h"
#include "iwdg.h"
#include "control.h"

Q_DEFINE_THIS_FILE

//definicje symboli (poddefinicje są zdefinowane w app_conf.h)
#define WD_TIMEOUT_TICKS   (BSP_TICKS_PER_SEC*BSP_WD_TIMEOUT/1000)

/* local objects -----------------------------------------------------------*/

/*$declare${AOs::Control} ##################################################*/
/*${AOs::Control} ..........................................................*/
typedef struct {
/* protected: */
    QActive super;

/* private: */
    QTimeEvt timeout;
} Control;

/* protected: */
static QState Control_initial(Control * const me, QEvt const * const e);
static QState Control_ctrl(Control * const me, QEvt const * const e);
/*$enddecl${AOs::Control} ##################################################*/
static Control l_control; /* the sole instance of the SerDisp active object */


/* Public-scope objects ----------------------------------------------------*/

/* Check for the minimum required QP version */
#if (QP_VERSION < 630U) || (QP_VERSION != ((QP_RELEASE^4294967295U) % 0x3E8U))
#error qpc version 6.3.0 or higher required
#endif

/*$define${AOs::AO_Control} ################################################*/
/*${AOs::AO_Control} .......................................................*/
QActive * const AO_Control = &l_control.super;
/*$enddef${AOs::AO_Control} ################################################*/
//$define(AOs::the_Ticker0)

/* Active object definition ================================================*/

/*$define${AOs::Control_ctor} ##############################################*/
/*${AOs::Control_ctor} .....................................................*/
void Control_ctor(void) {
    Control * const me = &l_control;

    QActive_ctor(&me->super, Q_STATE_CAST(&Control_initial));

    //inicjacja timera
    QTimeEvt_ctorX(&me->timeout, &me->super, TIMEOUT_SIG, HARD_TICKRATE_1);
}
/*$enddef${AOs::Control_ctor} ##############################################*/
/*$define${AOs::Control} ###################################################*/
/*${AOs::Control} ..........................................................*/
/*${AOs::Control::SM} ......................................................*/
static QState Control_initial(Control * const me, QEvt const * const e) {
    /*${AOs::Control::SM::initial} */
    QS_OBJ_DICTIONARY(AO_Control);
    (void)e;

    QS_FUN_DICTIONARY(&Control_ctrl);

    return Q_TRAN(&Control_ctrl);
}
/*${AOs::Control::SM::ctrl} ................................................*/
static QState Control_ctrl(Control * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /*${AOs::Control::SM::ctrl} */
        case Q_ENTRY_SIG: {
            QTimeEvt_armX(&me->timeout,  WD_TIMEOUT_TICKS, WD_TIMEOUT_TICKS);
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::Control::SM::ctrl::TIMEOUT} */
        case TIMEOUT_SIG: {
            iwdg_reset();
            status_ = Q_HANDLED();
            break;
        }
        default: {
            status_ = Q_SUPER(&QHsm_top);
            break;
        }
    }
    return status_;
}
/*$enddef${AOs::Control} ###################################################*/