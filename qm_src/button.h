/*$file${qm_src::button.h} #################################################*/
/*
* Model: balkonowy_rx_ult_.qm
* File:  ${qm_src::button.h}
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
/*$endhead${qm_src::button.h} ##############################################*/
#ifndef button_h
#define button_h


/* symbols definitions */

/* active objects' "constructors" */

/*$declare${AOs::Button_ctor} ##############################################*/
/*${AOs::Button_ctor} ......................................................*/
void Button_ctor(void);
/*$enddecl${AOs::Button_ctor} ##############################################*/

/* instantiation of the orthogonal components */


/* opaque pointer to AOs */
/*$declare${AOs::AO_Button} ################################################*/
extern QMActive * const AO_Button;
/*$enddecl${AOs::AO_Button} ################################################*/

#endif /* button_h */