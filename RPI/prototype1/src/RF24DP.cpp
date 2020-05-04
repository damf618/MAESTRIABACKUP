#include <cstdlib>						 // BIBLIOTECA DE FORMATOS STANDARD 
#include <string>		
#include <unistd.h>
#include <stdbool.h>
#include <RF24/RF24.h>					// BIBLIOTECA DE RF24 POR TMRH20
#include <RF24DP.h>
#include <delay.h>

#define WRITING_MAX_TIME 1000
#define READING_MAX_TIME 200

#define RETRIES_MAX_NUMBER 15			//How many retries before giving up, max 15*
#define RETRIES_MAX_SPACING 15			//0 means 250us, 15 means 4000us, max 15*  

#define NORMALIZATION_TIME 300			//min 300 possible time to send amessage after the reading.
#define RESET_TIME 5000					//MAX Time for stablish communication response.


static const uint8_t pipes[][6] = {"1Node", "2Node","3Node","4Node","5Node"};


void RF24DP:: Init(void){
	Receiver.timeout= read_timeout_default;
}

	void RF24DP:: Init(char CE, char CSN, char Num, char CA, char CF, char SC, char Reset, unsigned int timeout){
		CE_pin = CE; 		//22;
		CSN_pin = CSN; 		//0;
		Number = Num;		//1;
		Contacto_A = CA;	//19; 
		Contacto_F = CF;	//26;
		Status_Comm = SC;	//0;
		Reset_Pin = Reset; 	//20;
		Receiver.timeout= timeout;
		counter=0;
		RF24(CE_pin,CSN_pin);
	}

	void RF24DP:: Set_Up(char intentos, char tiempo, char Canal) {
		begin();
		setRetries(intentos/*15*/, tiempo/*15*/);			
		setChannel(Canal/*115*/);
		setPALevel(RF24_PA_MAX);
		
		openWritingPipe(pipes[1]);
		openReadingPipe(1, pipes[0]);
		
		printDetails();
	}	
	
	void RF24DP:: Writing_Data(void){
	// First, stop listening so we can talk.
		stopListening();
    // Take the time, and send it.  This will block until complete
		
		unsigned long start_time = millis();                             // Take the time, and send it.  This will block until complete
		while ((!write( &start_time, sizeof(unsigned long)))&&(millis()-start_time<=WRITING_MAX_TIME))
		{
		}
	}
	
	void RF24DP::Read_Data (void){
		startListening();
		int Status=0;
		if (delayRead(&(Receiver.delay))) 
		{
			counter++;
			Receiver.state = RECEIVE_TIMEOUT;
		} else
		{
			if (available()) 
			{
				counter++;
				while (available()) 
				{
					read(&Status, sizeof(int));
				}	
			
			Receiver.state = RECEIVE_RECEIVED_OK;
						
			if(Status==ALARM_CODE)
				Code=Alarm_Mode;
			else if(Status==FAIL_CODE)		
				Code=Fail_Mode;
			else if(Status==(int)NORMAL_CODE)
					Code=Normal_Mode;
			
			}
		}
}
	
	ReceiveOrTimeoutState_t RF24DP :: Wait_for_Code (){
		
		switch( Receiver.state ) {
			case RECEIVE_CONFIG:
				delayInit( &(Receiver.delay), Receiver.timeout );
				Receiver.state = RECEIVE_RECEIVING;		
				break;

			case RECEIVE_RECEIVING:
				Read_Data();
				break;

			case RECEIVE_RECEIVED_OK:
				delayInit( &(Receiver.delay), NORMALIZATION_TIME );
				//Escritura en Servidor Web
				//Escritura en Registro Local
				//Transcurrir el tiempo de Normalization 
				if(delayRead(&(Receiver.delay)))
					Receiver.state = RECEIVE_CONFIG;
					
				break;

			case RECEIVE_TIMEOUT:
				// 
				Receiver.state = RECEIVE_CONFIG;
				break;

			default:
				Receiver.state = RECEIVE_CONFIG;
				break;
   }
   return Receiver.state;
}

void RF24DP::RF24DPReset(void){
	Receiver.state = RECEIVE_CONFIG;			// It resets the MEF stage to receive this
    delayInit(&Receiver.delay,Receiver.timeout);		// Configure the UART Listening Process delay for the timeout measuring.
}

void RF24DP::RF24DPUpdate(void){
	if( Receiver.state != RECEIVE_RECEIVED_OK &&  //Asks if none of the timeout or received message events were reached.
		             Receiver.state != RECEIVE_TIMEOUT )
		Receiver.state = Wait_for_Code( ); //Updates the MEF to receive the next byte.
}

char RF24DP::RF24DPRead(void){
		if( Receiver.state == RECEIVE_TIMEOUT ){  // Asks for the timeout flag. Must be called after
			return Comm_timeout;									// the "Update" (UARTUpdate).
		}
		else if( Receiver.state == RECEIVE_RECEIVED_OK){  //Asks for the received code flag. Must be
			return Comm_received;									// called after the "Update" (UARTUpdate).
		}
		return 	Comm_receiving;
}


int RF24DP::Get_Code(void){
	return Code;
}

		
