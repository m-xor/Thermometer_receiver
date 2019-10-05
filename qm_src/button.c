/*$file${qm_src::button.c} #################################################*/
/*
* Model: balkonowy_rx_ult_.qm
* File:  ${qm_src::button.c}
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
/*$endhead${qm_src::button.c} ##############################################*/
#include "qpc.h"
#include "bsp.h"
#include "app_conf.h"
#include "protocol.h"
#include "project.h"
#include "button.h"

Q_DEFINE_THIS_FILE

//definicje symboli (poddefinicje są zdefinowane w app_conf.h)

#define LONG_PRESS_TICKS (LONG_PRESS_STRETCH * BSP_TICKS_PER_SEC/1000)

/* local objects -----------------------------------------------------------*/

/*$declare${AOs::Button} ###################################################*/
/*${AOs::Button} ...........................................................*/
typedef struct {
/* protected: */
    QActive super;

/* private: */
    QTimeEvt timer;
} Button;

/* protected: */
static QState Button_initial(Button * const me, QEvt const * const e);
static QState Button_idle(Button * const me, QEvt const * const e);
static QState Button_pressed(Button * const me, QEvt const * const e);
static QState Button_longPressed(Button * const me, QEvt const * const e);
/*$enddecl${AOs::Button} ###################################################*/

static Button l_button; /* the sole instance of this active object */


/* Public-scope objects ----------------------------------------------------*/

/* Check for the minimum required QP version */
#if (QP_VERSION < 630U) || (QP_VERSION != ((QP_RELEASE^4294967295U) % 0x3E8U))
#error qpc version 6.3.0 or higher required
#endif

/*$define${AOs::AO_Button} #################################################*/
/*${AOs::AO_Button} ........................................................*/
QMActive * const AO_Button = &l_button.super;
/*$enddef${AOs::AO_Button} #################################################*/

/* Active object definition ================================================*/

/*$define${AOs::Button_ctor} ###############################################*/
/*${AOs::Button_ctor} ......................................................*/
void Button_ctor(void) {
    Button * const me = &l_button;

    QActive_ctor(&me->super, Q_STATE_CAST(&Button_initial));

    //inicjacja timera
    QTimeEvt_ctorX(&me->timer, &me->super, TIMEOUT_SIG, HARD_TICKRATE_1);
}
/*$enddef${AOs::Button_ctor} ###############################################*/
/*$define${AOs::Button} ####################################################*/
/*${AOs::Button} ...........................................................*/
/*${AOs::Button::SM} .......................................................*/
static QState Button_initial(Button * const me, QEvt const * const e) {
    /*${AOs::Button::SM::initial} */
    QActive_subscribe(&me->super, BTN_EVT_SIG);

    QS_OBJ_DICTIONARY(AO_Button);

    (void)e;

    QS_FUN_DICTIONARY(&Button_idle);
    QS_FUN_DICTIONARY(&Button_pressed);
    QS_FUN_DICTIONARY(&Button_longPressed);

    return Q_TRAN(&Button_idle);
}
/*${AOs::Button::SM::idle} .................................................*/
static QState Button_idle(Button * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /*${AOs::Button::SM::idle::BTN_EVT} */
        case BTN_EVT_SIG: {
            QTimeEvt_rearm(&me->timer,  LONG_PRESS_TICKS);
            //QTimeEvt_armX(&me->timer,  LONG_PRESS_TICKS, 0);
            status_ = Q_TRAN(&Button_pressed);
            break;
        }
        default: {
            status_ = Q_SUPER(&QHsm_top);
            break;
        }
    }
    return status_;
}
/*${AOs::Button::SM::pressed} ..............................................*/
static QState Button_pressed(Button * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /*${AOs::Button::SM::pressed::TIMEOUT} */
        case TIMEOUT_SIG: {
            QF_PUBLISH(Q_NEW(QEvt, BTN_PRSS_SIG), me);
            status_ = Q_TRAN(&Button_longPressed);
            break;
        }
        /*${AOs::Button::SM::pressed::BTN_EVT} */
        case BTN_EVT_SIG: {
            QF_PUBLISH(Q_NEW(QEvt, BTN_SHORT_SIG), me);
            status_ = Q_TRAN(&Button_idle);
            break;
        }
        default: {
            status_ = Q_SUPER(&QHsm_top);
            break;
        }
    }
    return status_;
}
/*${AOs::Button::SM::longPressed} ..........................................*/
static QState Button_longPressed(Button * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /*${AOs::Button::SM::longPressed::BTN_EVT} */
        case BTN_EVT_SIG: {
            QF_PUBLISH(Q_NEW(QEvt, BTN_REL_SIG), me);
            status_ = Q_TRAN(&Button_idle);
            break;
        }
        default: {
            status_ = Q_SUPER(&QHsm_top);
            break;
        }
    }
    return status_;
}
/*$enddef${AOs::Button} ####################################################*/