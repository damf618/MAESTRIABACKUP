/*=============================================================================
 * Copyright (c) 2020, DANIEL MARQUEZ <DAMF618@GMAIL.COM>
 * All rights reserved.
 * License: bsd-3-clause (see LICENSE.txt)
 * Date: 2020/04/12
 * Version: 1
 *===========================================================================*/

/*=====[Inclusions of function dependencies]=================================*/

#include "primario4.h"
#include "antirebote.h"
#include "Primario_LEDS.h"
#include <cstdbool>
#include <cstdio>
#include <RF24DP.h>

/*=====[Definition macros of private constants]==============================*/
#define INITIAL_DEFAULT_STATE NORMAL

#define DEF_TIMEOUT 3500			   //Timeout limit between transitions
RF24DP FireComm(22,0);

/*=====[Definitions of extern global variables]==============================*/

/*=====[Definitions of public global variables]==============================*/

/*=====[Definitions of private global variables]=============================*/

/*=====[Functions, program entry point after power on or reset]==========*/

// To turn on or off the LEDs according to the current state
void LEDsON(char x,char y,char z){
	if(x==1)					// if x==1, then the Yellow LED is turned on,
		turnOn(YELLOW_LIGHT);	// is turned off
	else if(x==0)
		turnOff(YELLOW_LIGHT);

	if(y==1)					// if y==1, then the Red LED is turned on,
		turnOn(RED_LIGHT);		// is turned off
	else if(y==0)
		turnOff(RED_LIGHT);

	if(z==1)					// if z==1, then the Green LED is turned on,
		turnOn(GREEN_LIGHT);	// is turned off
	else if(z==0)
		turnOff(GREEN_LIGHT);

}

// Whenever a change of state is made, it resets the transition conditions
static void ResetChange(dprimario_t * prim){
	delayInit( &prim->delay,prim->timeout);  //Reset the timeout transition
	FireComm.RF24DPReset();						 //Reset the UART Listening Process
	prim->count=0;								 //Reset the count of number of cycles
	prim->COMMFLAG=0;								 //Reset the UART flag
}

// To verify if we are stuck in the middle of a transition (PRE-STATE) or in a
// waiting in a principal state (ALARM, NORMAL ,FAIL).
static void PRESTUCK(dprimario_t * prim){
	if((prim->state==PREALARM)||
		(prim->state==PREFAIL)||
		(prim->state==PRENORMAL)){
		prim->state = FAIL;
		if(prim->COMMFLAG){
			ResetChange(prim);
			prim->COMMFLAG=1;
		}
		else
					ResetChange(prim);
	}
	else
		ResetChange(prim);
	
	/*
	if(prim->COMMFLAG==1){						//I got here from an UART request?
		if((prim->state==PREALARM)||
			(prim->state==PREFAIL)||
			(prim->state==PRENORMAL)){			//Go to Fail state
			prim->state = FAIL;
			ResetChange(prim);
			prim->COMMFLAG=1;					//Set the UART Flag interaction
		}										//the reset statement clears COMMFLAG
	}
	else{
		if((prim->state==PREALARM)||
			(prim->state==PREFAIL)||
			(prim->state==PRENORMAL)){ 	//Am i stuck in PRESTATE?
			prim->state = FAIL;			//to Fail state.
			ResetChange(prim);
		}
	}
	*/
}

// Verify the transition conditions related to pushbuttons
static void ButtonCheck(dprimario_t * prim, dprim_state_t casea, dprim_state_t casef,dprim_state_t casen ){
	if(!delayRead(&prim->delay)){		//Verify if the Timeout transition limit
		if(get_State(&prim->boton1)){			//Button pressed?
			prim->state=casea;			//The New state is the Alarm related state
			ResetChange(prim);
		}
		else if(get_State(&prim->boton2)){	//Button pressed?
			prim->state=casef;			//The New state is the Fail related state
			ResetChange(prim);
		}
		else if(get_State(&prim->boton3)){	//Button pressed?
			prim->state=casen;			//The New state is the Normal related state
			ResetChange(prim);
		}
	}
	else								//Timeout transition limit reached
		PRESTUCK(prim);
}



// Verify the transition conditions related to uart codes
static void CommCheck(dprimario_t * prim,dprim_state_t casea, dprim_state_t casef,dprim_state_t casen  ){
		if((FireComm.RF24DPRead()==Comm_received)){  //Was an Alarm code received?
			int AUX=FireComm.Get_Code();
			if(AUX==Alarm_Mode){
				prim->state=casea;
				prim->COMMFLAG=1;
			}	
			else if(AUX==Fail_Mode){
					prim->state=casef;
					prim->COMMFLAG=1;
			}		
			else if(AUX==Normal_Mode){
				prim->state=casen;
				prim->COMMFLAG=1;
			}
			ResetChange(prim);
			prim->COMMFLAG=1;							//Set the UART Flag interaction
		}
		else if(FireComm.RF24DPRead()==Comm_timeout){//The UART Listening timeout was reached?
			PRESTUCK(prim);										//monitor Fail codes
		}
}		
////////////

// Verify the 3 Uart Codes and the button state transitions
static void FullCheck(dprimario_t * prim,dprim_state_t casea, dprim_state_t casef,dprim_state_t casen){
	ButtonCheck(prim,casea,casef,casen);
	CommCheck(prim,casea,casef,casen);
}



//update the MEFSs,
void primUpdates(dprimario_t * pPrimario){

    fsmUpdate(&pPrimario->boton1);			//Update of all the MEFSs involved
	fsmUpdate(&pPrimario->boton2);
	fsmUpdate(&pPrimario->boton3);
	FireComm.RF24DPUpdate();
}

// It sets initial conditions for the entire program
bool primInit(dprimario_t * pPrimario)
{

	if(NULL==pPrimario)
		return 0;

	pPrimario->state=INITIAL_DEFAULT_STATE;
	pPrimario->timeout= DEF_TIMEOUT;
	delayInit( &pPrimario->delay,pPrimario->timeout);
	gpioSet();
	LEDsON(0,0,0);
	fsmInit(&pPrimario->boton1,ALARM_BUTTON);	//Initialize buttons with "antirebote" protocol
	fsmInit(&pPrimario->boton2,FAIL_BUTTON);
	fsmInit(&pPrimario->boton3,NORMAL_BUTTON);
	FireComm.Init(); //Initialize uart1 through UART_USB
	FireComm.Set_Up(15,15,115); //Initialize uart1 through UART_USB
	return 1;
}

// The MEFS logic, execute the actions related to the state
// and verifies if there is any possible transition.
bool primControl(dprimario_t * pPrimario){

	if(NULL==pPrimario)
		return 0;

	switch( pPrimario->state ) {

		case NORMAL:
			LEDsON(0,0,1);
			FullCheck (pPrimario,PREALARM,PREFAIL,NORMAL);
			break;
		case ALARM:
			LEDsON(0,1,0);
			FullCheck (pPrimario,ALARM,PREFAIL,PRENORMAL);
			break;
		case FAIL:
			LEDsON(1,0,0);
			FullCheck (pPrimario,PREALARM,FAIL,PRENORMAL);
			break;
		case PRENORMAL:
			FullCheck (pPrimario,PREALARM,PREFAIL,NORMAL);
			break;
		case PREALARM:
			FullCheck (pPrimario,ALARM,PREFAIL,PRENORMAL);
			break;
		case PREFAIL:
			FullCheck (pPrimario,PREALARM,FAIL,PRENORMAL);
			break;
		default:
			pPrimario->state=FAIL;
		}
	return 1;
}

