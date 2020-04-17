/*
 Código para establecer unicamente la coomunicación entre los dispositivos, en esta etapa se desea solo comunicarse con uno de los elementos,
 extrayendo solo en este momento los siguientes datos:
     
	 & ESTADO DE LOS CONTACTOS.
	 & RE ESCRBIR LA INFORMACION EN UN ARCHIVO DE DATOS PARA SU REGISTRO Y POST-USO.
	 & PROTOCOLO DE ENVIO Y TRACKEABILIDAD DE LOS DATOS PARA ESTABLECER LAS CONEXIONES EXISTENTES, CERCANIA Y STATUS DE LA RED.
	 
 MAX Devices: 5
	 
 */
#include <cstdlib>						 // BIBLIOTECA DE FORMATOS STANDARD 
#include <iostream>
#include <sstream>
#include <fstream>						// CREACION DE DOCUMENTACION 
#include <string>		
#include <unistd.h>
#include <stdbool.h>
#include <RF24/RF24.h>					// BIBLIOTECA DE RF24 POR TMRH20

#define WRITING_MAX_TIME 1000
#define READING_MAX_TIME 200

#define RETRIES_MAX_NUMBER 15			//How many retries before giving up, max 15*
#define RETRIES_MAX_SPACING 15			//0 means 250us, 15 means 4000us, max 15*  

#define NORMALIZATION_TIME 300			//min 300 possible time to send amessage after the reading.
#define RESET_TIME 5000					//MAX Time for stablish communication response.


static const uint8_t pipes[][6] = {"1Node", "2Node","3Node","4Node","5Node"};
uint64_t counter=0;						//message counter
//static std::fstream textfile;
static std::fstream textfile;

typedef struct {
	char CE_pin = 22;
	char CSN_pin = 0;
	char Number = 1;
	int Status_A = 0;
	int Status_F = 0;
    char Contacto_A = 19; 
    char Contacto_F = 26;
	int Status_Comm = 0;
	char Reset_Pin = 20;
	
}Monitor_t;



void Monitor_Init(char gpio1, char gpio2,char gpio3, char gpio4, char gpio5, char device_number,Monitor_t Monitor1,RF24 radio){
	
	//Contactos de Comunicacion
	Monitor1.CE_pin=gpio1;
	Monitor1.CSN_pin=gpio2;
	Monitor1.Number=device_number;
	Monitor1.Contacto_A=gpio3;
	Monitor1.Contacto_F=gpio4;
	Monitor1.Reset_Pin=gpio5;
	
	
	// Setup and configure rf radio
	radio.begin();
   	radio.setRetries(15, 15);			
   	radio.printDetails();
	radio.setChannel(115);
	radio.setPALevel(RF24_PA_MAX);
	
	//Contactos de Monitoreo
	/*pinMode(Monitor1.Contacto_A, INPUT);
	pinMode(Monitor1.Contacto_F, INPUT);
	pinMode(Monitor1.Reset_Pin, OUTPUT);
	*/
	if(Monitor1.Number==1)
	{
		    // This simple sketch opens two pipes for these two nodes to communicate
    		// back and forth.
        radio.openWritingPipe(pipes[1]);			// openWritingPipe(const uint8_t * 	address) address.	The address of the pipe	
													// to open. Coordinate these pipe addresses amongst nodes on the network.
		
        radio.openReadingPipe(1, pipes[0]);			// openReadingPipe	(uint8_t   number,const uint8_t * 	address).	Pipes 1-5 should share the
													// same address, except the first byte. Only the first byte in the array should be unique.
													// number.	Which pipe# to open, 0-5. address The 24, 32 or 40 bit address of the pipe to open.
		//textfile.open("REGISTRO_SISTEMA_ALARMA_INCENDIO.csv");
	}
	textfile.open("REGISTRO_SISTEMA_ALARMA_INCENDIO.csv", std::ofstream::out | std::ofstream::trunc);
	textfile <<"DATA LOG ALARM NOTIFICATION SYSTEM"<< std::endl;
}

void Writing_Data(RF24 radio){
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

bool Read_Status(RF24 radio){
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

static void Normalization_Delay(){
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

void Read_Data (bool timeout, Monitor_t Monitor1,RF24 radio){
	
	// Describe the results
	counter++;
	if (timeout) 
	{
		printf("Failed. response timed out.\n");
		textfile <<counter << "," <<"F"<< std::endl;
    } else
	{
    	
	//	radio.flush_rx();		
	
	// Grab the response, compare, and send to debugging spew
		if (radio.available()) 
		{
	//Codigo para verificar y asegurarque los datos de tiempo 
	//obtenidos sean realmente datos de tiempo y no un dato enviado
	//en el proceso anterior
					
    // Dump the payloads until we've gotten everything
    // Fetch the payload, and see if this was the last one.
			
			while (radio.available()) 
			{
				radio.read(&Monitor1.Status_A, sizeof(int));
			}	
		}
		printf("Status Alarm: %i \n" ,Monitor1.Status_A);
		textfile <<counter <<","<<Monitor1.Status_A<< std::endl;
    }
	Normalization_Delay();	
}
/*
void Read_Data_Slave (bool timeout, Monitor_t Monitor1,RF24 radio){

	while(!radio.available()){                                     // Reset Code if the device staysmore than 5 seconds in waiting mode
		if(millis()-check_time>=RESET_TIME)                               // it will restart and try again
			digitalWrite(resetPin,LOW);
    }	
	
	// Grab the response, compare, and send to debugging spew
		if (radio.available()) 
		{
	//Codigo para verificar y asegurarque los datos de tiempo 
	//obtenidos sean realmente datos de tiempo y no un dato enviado
	//en el proceso anterior
					
    // Dump the payloads until we've gotten everything
    // Fetch the payload, and see if this was the last one.
			
			while (radio.available()) 
			{
				radio.read(&Monitor1.Status_A, sizeof(int));
			}	
		}
    
	Normalization_Delay();	
}


void Writing_Data_Slave(RF24 radio,Monitor_t Monitor1){
	// First, stop listening so we can talk.
	radio.stopListening();
    // Take the time, and send it.  This will block until complete
	unsigned long start_time = millis();                             // Take the time, and send it.  This will block until complete
	stattusA=digitalRead(ContactoA);
	while ((!radio.write( &statusA, sizeof(int)))&&(millis()-start_time<=WRITING_MAX_TIME))
	{
		printf(".");
	}
	printf("\n");
	
	radio.startListening();                                       // Now, resume listening so we catch the next packets.     
    Serial.print(F("Sent response "));
    Serial.print(got_time);  
    Serial.print(F(" Status Alarm: "));
    Serial.println(stattusA);
}
*/
Monitor_t Monitor1;


using namespace std;
bool timeout=false;

int main(int argc, char** argv){
	
	RF24 radio (22,0);
	Monitor_Init(22,0,19,26,20,1,Monitor1,radio);
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
		Writing_Data(radio);
		cout << " Read Status " << endl;
		Read_Data(Read_Status(radio),Monitor1,radio);
	/****** ETAPA 2 ENVIO DE DATOS DE FALLA ****/
	//	Writing_Data(Monitor1);
	//	timeout=Read_Status(Monitor1);
	//	Read_Data(timeout,Monitor1.Status_F);
    }

    return 0;
}
