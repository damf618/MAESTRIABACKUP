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
#include <RF24DC.h>

#define WRITING_MAX_TIME 1000
#define READING_MAX_TIME 200

using namespace std;

Monitor_t Monitor1;



void Monitor_Init(Monitor_t Monitor1);
//main(main(int argc, char** argv));
int main(int argc, char** argv){
    bool role_ping_out = true, role_pong_back = false;
    bool role = role_pong_back;

    cout << "Welcome to the DEEPCONTROL Algorithm\n";
    //string lectura[30];
    fstream textfile;
    textfile.open("DATOS1.csv");
    textfile << "DEEPCONTROL DATA ALGORITHM ENCODING\n"<<endl;
    
    
    //TODO VERIFICAR DIFERENTES CANALES,CUAL ES EL VALOR DEFAULT?
	//TODO radio.setChannel(55); 
	//printing the entire configuration

    /********* Role chooser ***********/

    printf("\n ************ Role Setup ***********\n");
    string input = "";
    char myChar = {0}; 
	char aux=0;
	
	//SELECCION DE FORMA DE TRABAJO
    while(aux == 0){
		cout << "Choose a role: Enter 0 for Master, 1 for Slave (CTRL+C to exit) \n>";
		getline(cin, input);
		if (input.length() == 1) 
		{
		aux=1;
		}
		else
		{
		cout << "Please write a valid option " << endl << endl;
		}
	}
	//ASIGNACION DE MODO DE TRABAJO
        myChar = input[0];
        if (myChar == '0') 
		{
            cout << "Role: Pong Back, awaiting transmission " << endl << endl;
        } else
		{
            cout << "Role: Ping Out, starting transmission " << endl << endl;
            role = role_ping_out;
        }
	
    /***********************************/

    // Forever loop
    while (1) {
		if (role == role_ping_out)
		{
			
			Writing_Data(Monitor1);
			Read_Data(Monitor1);
			
//...................................................................added layer
            
			// First, stop listening so we can talk.
            radio.stopListening();

            // Take the time, and send it.  This will block until complete

            printf("Now sending...");
			int stattusA=0;

			while ((!radio.write( &stattusA, sizeof(int)))&&(millis()-start_time<=1000)){
			printf(".");
			}
			printf("\n");
			
            // Now, continue listening
            radio.startListening();

            // Wait here until we get a response, or timeout (200ms)
            started_waiting_at = millis();
            timeout = false;
            while (!radio.available() && !timeout) {
                if (millis() - started_waiting_at > 200) {
                    timeout = true;
                }
            }
            // Describe the results
			if (timeout) {
                printf("Failed. response timed out.\n");
				textfile <<","<<"0"<< endl;
            } else {
                // Grab the response, compare, and send to debugging spew
				if (radio.available()) {
                // Dump the payloads until we've gotten everything
                // Fetch the payload, and see if this was the last one.
					while (radio.available()) {
						radio.read(&stattusA, sizeof(int));
					}
				}
                // Spew it
                printf("Status Alarm: %i \n" ,stattusA);
				textfile <<","<<stattusA<< endl;
            }				
            //Delay for 0,3 seg
			started_waiting_at = millis();
			while (!timeout) {
                if (millis() - started_waiting_at > 300) {
                    timeout = true;
                }
            }
			
			sleep(1);
        }

    } // forever loop
	textfile.close();

    return 0;
}


