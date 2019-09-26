/*$file${qm_src::receiver.h} ###############################################*/
/*
* Model: balkonowy_rx_ult_.qm
* File:  ${qm_src::receiver.h}
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
/*$endhead${qm_src::receiver.h} ############################################*/
#ifndef receiver_h
#define receiver_h

/* opaque pointers to AOs */
extern QActive * const  AO_Rcv;
extern QActive * const AO_RcvMsgDec;
extern QActive * const AO_RcvBatDec;
extern QActive * const AO_RcvTempDec;

/*$declare${AOs::Rcv_ctor} #################################################*/
/*${AOs::Rcv_ctor} .........................................................*/
void Rcv_ctor(void);
/*$enddecl${AOs::Rcv_ctor} #################################################*/
/*$declare${AOs::RcvMsgDec_ctor} ###########################################*/
/*${AOs::RcvMsgDec_ctor} ...................................................*/
void RcvMsgDec_ctor(void);
/*$enddecl${AOs::RcvMsgDec_ctor} ###########################################*/
/*$declare${AOs::RcvBatDec_ctor} ###########################################*/
/*${AOs::RcvBatDec_ctor} ...................................................*/
void RcvBatDec_ctor(void);
/*$enddecl${AOs::RcvBatDec_ctor} ###########################################*/
/*$declare${AOs::RcvTempDec_ctor} ##########################################*/
/*${AOs::RcvTempDec_ctor} ..................................................*/
void RcvTempDec_ctor(void);
/*$enddecl${AOs::RcvTempDec_ctor} ##########################################*/

#endif /* receiver_h */