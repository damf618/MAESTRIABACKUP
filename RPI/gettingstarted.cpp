/*
 Código para establecer unicamente la coomunicación entre los dispositivos, en esta etapa se desea solo comunicarse con uno de los elementos,
 extrayendo solo en este momento los siguientes datos:
     
	 & ESTADO DE LOS CONTACTOS.
	 & RE ESCRBIR LA INFORMACION EN UN ARCHIVO DE DATOS PARA SU REGISTRO Y POST-USO.
	 & PROTOCOLO DE ENVIO Y TRACKEABILIDAD DE LOS DATOS PARA ESTABLECER LAS CONEXIONES EXISTENTES, CERCANIA Y STATUS DE LA RED.
	 
 MAX Devices: 5
	 
 */
#include <cstdlib>		// BIBLIOTECA DE FORMATOS STANDARD 
#include <iostream>
#include <sstream>
#include <fstream>		// CREACION DE DOCUMENTACION 
#include <string>		
#include <unistd.h>
#include <stdbool.h>
#include <RF24/RF24.h>	// BIBLIOTECA DE RF24 POR TMRH20

//**********************

#define WRITING_MAX_TIME 1000
#define READING_MAX_TIME 200

#define RETRIES_MAX_NUMBER 15			//How many retries before giving up, max 15*
#define RETRIES_MAX_SPACING 15			//0 means 250us, 15 means 4000us, max 15*  

#define NORMALIZATION_TIME 300			//min 300 possible time to send amessage after the reading.

//*********************

using namespace std;
/**************************************
**************************************/

typedef enum {TRANSMITTER,RECEIVER} Working_Mode_t;

typedef struct
{
	char CE_pin= 22;
	char CSN_pin= 0;
	Working_Mode_t mode;
	char Number= 1;
	int Status_A=0;
	int Status_F=0;
	int Status_Comm=0;
	
}Monitor_t;

/*********
*********/

RF24 radio(22,0);
const uint8_t pipes[][6] = {"1Node", "2Node","3Node","4Node","5Node"};
uint64_t counter=0;						//message counter
fstream textfile;

void Monitor_Init(Monitor_t Monitor1){
	RF24 radio(22,0);
	bool Radio_Number = Monitor1.Number;															//DEVICE NUMBER 1
	// Setup and configure rf radio
    radio.begin();
    
    radio.setRetries(15, 15);			
    radio.printDetails();
	if(Radio_Number==1)
	{
		    // This simple sketch opens two pipes for these two nodes to communicate
    		// back and forth.
        radio.openWritingPipe(pipes[1]);			// openWritingPipe(const uint8_t * 	address) address.	The address of the pipe	
													// to open. Coordinate these pipe addresses amongst nodes on the network.
		
        radio.openReadingPipe(1, pipes[0]);			// openReadingPipe	(uint8_t   number,const uint8_t * 	address).	Pipes 1-5 should share the
													// same address, except the first byte. Only the first byte in the array should be unique.
													// number.	Which pipe# to open, 0-5. address The 24, 32 or 40 bit address of the pipe to open.
		Monitor1.mode=TRANSMITTER;
		fstream textfile;
		textfile.open("REGISTRO-SISTEMA-ALARMA_INCENDIO.csv");
	
	}
}

void Writing_Data(Monitor_t Monitor1){
	RF24 radio(22,0);
	// First, stop listening so we can talk.
	radio.stopListening();
    // Take the time, and send it.  This will block until complete
	printf("Now sending...");
	unsigned long start_time = millis();                             // Take the time, and send it.  This will block until complete
	while ((!radio.write( &start_time, sizeof(unsigned long)))&&(millis()-start_time<=WRITING_MAX_TIME))
	{
		printf(".");
	}
	printf("\n");
}

bool Read_Status(){
	RF24 radio(22,0);
	bool timeout = false;	
	unsigned long started_waiting_at = millis();
	// Now, continue listening
	radio.startListening();
   // Wait here until we get a response, or timeout (200ms)	
	while (!radio.available() && !timeout) 
	{
		if (millis() - started_waiting_at > READING_MAX_TIME) 
		{
			return true;
        }
    }
	return false;
}

void Normalization_Delay(){
	//Delay for normalization for 0,3 seg
	unsigned long started_waiting_at = millis();
	bool timeout=false;
	while (!timeout) 
	{
		if (millis() - started_waiting_at > NORMALIZATION_TIME) 
		{
			timeout = true;
		}
	}
}

void Read_Data (bool timeout, int statusA){
    RF24 radio(22,0);
	// Describe the results
	counter++;
	if (timeout) 
	{
		printf("Failed. response timed out.\n");
		textfile <<counter << "," <<"F,"<<"0"<<"0"<< endl;
    } else
	{
    	
	//	radio.flush_rx();		
	
	// Grab the response, compare, and send to debugging spew
		if (radio.available()) 
		{
	/*Codigo para verificar y asegurarque los datos de tiempo 
	obtenidos sean realmente datos de tiempo y no un dato enviado
	en el proceso anterior*/
					
    // Dump the payloads until we've gotten everything
    // Fetch the payload, and see if this was the last one.
			
			while (radio.available()) 
			{
				radio.read(&statusA, sizeof(int));
			}	
		}
		printf("Status Alarm: %i \n" ,statusA);
		textfile <<","<<statusA<< endl;
    }
	Normalization_Delay();	
}

/***************************************
****************************************/
bool timeout=false;


int main(int argc, char** argv){
	Monitor_t Monitor1;
	void Monitor_Init(Monitor_t Monitor1);
    cout << "Developed by DEEPCONTROL Algorithm's \n";
	sleep(1);
    cout << "\n";
	cout << "\n";
    //TODO VERIFICAR DIFERENTES CANALES,CUAL ES EL VALOR DEFAULT?
	//TODO radio.setChannel(55); 

    printf("\n ****** Isolse SRL Fire Alarm Notification Platform *****\n");
    string input = "";
	cout << "Press 'Enter' to continue or (CTRL+C) to exit \n>";
	getline(cin, input);
		
    // Forever loop
    while (1)
	{
	/****** ETAPA 1 ENVIO DE DATOS DE FALLA ****/		
		cout << " Ping Out, Starting Transmission " << endl << endl;
		Writing_Data(Monitor1);			
		cout << " Read Status " << endl;
		Read_Data(Read_Status(),Monitor1.Status_A);
	/****** ETAPA 2 ENVIO DE DATOS DE FALLA ****/
	//	Writing_Data(Monitor1);	
	//	timeout=Read_Status(Monitor1);
	//	Read_Data(timeout,Monitor1.Status_F);
    }

    return 0;
}


