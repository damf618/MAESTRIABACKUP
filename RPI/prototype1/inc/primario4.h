/*=============================================================================
 * Copyright (c) 2020, DANIEL MARQUEZ <DAMF618@GMAIL.COM>
 * All rights reserved.
 * License: bsd-3-clause (see LICENSE.txt)
 * Date: 2020/04/12
 * Version: 1
 *===========================================================================*/

/*=====[Avoid multiple inclusion - begin]====================================*/

#ifndef __PRIMARIO_H__
#define __PRIMARIO_H__

/*=====[Inclusions of public function dependencies]==========================*/

#include <stdint.h>
#include <stddef.h>
#include <delay.h>
#include "antirebote.h"
#include "Primario_LEDS.h"
#include <cstdbool>

/*=====[C++ - begin]=========================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*=====[Definition macros of public constants]===============================*/

/*=====[Public function-like macros]=========================================*/

/*=====[Definitions of public data types]====================================*/

// Possible states for the MEFs
typedef enum{ NORMAL , ALARM, FAIL, PRENORMAL, PREALARM, PREFAIL} dprim_state_t;

// Structure with the different data types to generate an independent Monitor
typedef struct{
	tick_t timeout;
	delay_t delay;
	dprim_state_t state;
	antirebote_t boton1;	//Buttons with  debounce pattern
	antirebote_t boton2;
	antirebote_t boton3;
	//uart_prim_t uart1;		//Uart interface
	int8_t count;			//Count of Cycles.
	bool COMMFLAG;		//Flag for UART interaction 1 ON, 0 OFF
	bool TEST_MODE;		//Flag for defining TEST mode

}dprimario_t;


/*=====[Prototypes (declarations) of public functions]=======================*/
/** It sets initial conditions for the entire program, it establish the default values
 *  of timeout and #cycles for Alarm and Fail UART code verification.

	@param pPrimario element of type *dprimario_t* with the Monitor data types needed
	@note It must be called before primControl.
	@see primControl.

**/
bool primInit(dprimario_t * pPrimario);

/** It identifies the state we are currently and generates the corresponding
 *  outputs verifying if for the current state exists any possible transition.

	@param pPrimario element of type *dprimario_t* with the Monitor data types needed
	@note If the current state is not define it returns FALSE.
	@see primInit.

**/
bool primControl(dprimario_t * pPrimario);

/** It updates the state of all the existent MEFs and also checks the TESTMODE.
	@param pPrimario element of type *dprimario_t* with the Monitor data types needed
	@note Must be called after primInit.
	@see primInit.

**/
void primUpdates(dprimario_t * pPrimario);

/*=====[Prototypes (declarations) of public interrupt functions]=============*/

/*=====[C++ - end]===========================================================*/

#ifdef __cplusplus
}
#endif

/*=====[Avoid multiple inclusion - end]======================================*/

#endif /* __PRIMARIO_H__ */
