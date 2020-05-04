#include "primario4.h"
#include "cstdio"
#include <delay.h>

#define CHECK_TIME 1000
#define UPDATE_TIME 10
using namespace std;

unsigned int start_time = 0;
unsigned int start_time2 = 0;
bool timeout=false;


void CurrentState(dprimario_t *prim)
{
	switch( prim->state ) {

		case PRENORMAL:
			printf("\r\n CURRENT STATE: PRE-NORMAL \r\n");
			break;
		case PREALARM:
			printf("\r\n CURRENT STATE: PRE-ALARM \r\n");
			break;
		case PREFAIL:
			printf("\r\n CURRENT STATE: PRE-FAIL \r\n");
			break;
		case NORMAL:
			printf("\r\n CURRENT STATE: NORMAL \r\n");
			break;
		case FAIL:
			printf("\r\n CURRENT STATE: FAIL\r\n");
			break;
		case ALARM:
			printf("\r\n CURRENT STATE: ALARM\r\n");
			break;
		default:
			printf("\r\n CURRENT STATE: PLEASE RESTART\r\n");
	}
}

int main(int argc, char** argv)
{
	dprimario_t prim;
	printf("Inicio");
	primInit(&prim);
	start_time = tickRead();
	start_time2 = tickRead();
	while( true ){
		
		if((tickRead()-start_time>=UPDATE_TIME)){
			start_time = tickRead();
			primUpdates(&prim);
			primControl(&prim);
		}
				
		if((tickRead()-start_time2>=CHECK_TIME)){
			start_time2 = tickRead();
			CurrentState(&prim);	
			printf("Comm Flag: ");
			printf(" %d",prim.COMMFLAG);
		}
	}

   // YOU NEVER REACH HERE, because this program runs directly or on a
   // microcontroller and is not called by any Operating System, as in the 
   // case of a PC program.
	return 0;
}
