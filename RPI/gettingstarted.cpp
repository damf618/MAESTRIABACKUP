/*
 Código para establecer unicamente la coomunicación entre los dispositivos, en esta etapa se desea solo comunicarse con uno de los elementos,
 extrayendo solo en este momento los siguientes datos:
     
	 & ESTADO DE LOS CONTACTOS.
	 & RE ESCRBIR LA INFORMACION EN UN ARCHIVO DE DATOS PARA SU REGISTRO Y POST-USO.
	 & PROTOCOLO DE ENVIO Y TRACKEABILIDAD DE LOS DATOS PARA ESTABLECER LAS CONEXIONES EXISTENTES, CERCANIA Y STATUS DE LA RED.
	 
 MAX Devices: 5
	 
 */


#include <cstdlib>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <RF24/RF24.h>

using namespace std;
//
// Hardware configuration
// Configure the appropriate pins for your connections

/****************** Raspberry Pi ***********************/

// Radio CE Pin, CSN Pin, SPI Speed
// RPi generic:
RF24 radio(22, 0);

/********** User Config *********/
// Assign a unique identifier for this node, 0 or 1
bool radioNumber = 1;
//bool shadowmember[5] = [0,1,2,3,4,5];

/********************************/

// Radio pipe addresses for the 2 nodes to communicate.
const uint8_t pipes[][6] = {"1Node", "2Node"};
//Pipes Availables for the master: 1 to listen 2 3 4 5 y 6

unsigned long counter=0;

int main(int argc, char** argv)
{

    bool role_ping_out = true, role_pong_back = false;
    bool role = role_pong_back;

    cout << "Welcome to the DEEPCONTROL Algorithm\n";
/* NEW WRITING MESSAGE*/
    string lectura[30];
    fstream textfile;
    textfile.open("DATOS1.csv");
    textfile << "DEEPCONTROL DATA ALGORITHM ENCODING\n"<<endl;
    
/*NEW WRITING MESSAGE */

    // Setup and configure rf radio
    radio.begin();
    
	/*setRetries(uint8_t 	delay, uint8_t 	count)
     delay	How long to wait between each retry, in multiples of 250us, max is 15. 0 means 250us, 15 means 4000us.
	 count	How many retries before giving up, max 15	
	*/
    radio.setRetries(15, 15);
    
    //radio.setChannel(55);
	//default channel?
	//printing the entire configuration
    radio.printDetails();

    /********* Role chooser ***********/

    printf("\n ************ Role Setup ***********\n");
    string input = "";
    char myChar = {0}; 
	char aux=0;
	
	//SELECCION DE FORMA DE TRABAJO
    while(aux == 0){
		cout << "Choose a role: Enter 0 for Master, 1 for Slave (CTRL+C to exit) \n>";
		getline(cin, input);
		if (input.length() == 1) {
		aux=1;
		}
		else{
		cout << "Please write a valid option " << endl << endl;
		}
	}
	//ASIGNACION DE MODO DE TRABAJO
        myChar = input[0];
        if (myChar == '0') {
            cout << "Role: Pong Back, awaiting transmission " << endl << endl;
        } else {
            cout << "Role: Ping Out, starting transmission " << endl << endl;
            role = role_ping_out;
        }
	
    /***********************************/
    // This simple sketch opens two pipes for these two nodes to communicate
    // back and forth.

    if (!radioNumber) {
		/*openWritingPipe(const uint8_t * 	address)	
		address	The address of the pipe to open. Coordinate these pipe addresses amongst nodes on the network.
		
		openReadingPipe	(uint8_t   number,const uint8_t * 	address)
		Pipes 1-5 should share the same address, except the first byte. Only the first byte in the array should be unique.
		
		number	Which pipe# to open, 0-5.
		address	The 24, 32 or 40 bit address of the pipe to open.
		*/
		
        radio.openWritingPipe(pipes[0]);
        radio.openReadingPipe(1, pipes[1]);
		
    } else {
        radio.openWritingPipe(pipes[1]);
        radio.openReadingPipe(1, pipes[0]);
    }

    radio.startListening();

    // forever loop
    while (1) {
        if (role == role_ping_out) {
            // First, stop listening so we can talk.
            radio.stopListening();

            // Take the time, and send it.  This will block until complete

            printf("Now sending...");
			unsigned long start_time = millis();                             // Take the time, and send it.  This will block until complete


			while ((!radio.write( &start_time, sizeof(unsigned long)))&&(millis()-start_time<=1000)){
			printf(".");
			}
			printf("\n");
			
            // Now, continue listening
            radio.startListening();

            // Wait here until we get a response, or timeout (200ms)
            unsigned long started_waiting_at = millis();
            bool timeout = false;
            while (!radio.available() && !timeout) {
                if (millis() - started_waiting_at > 200) {
                    timeout = true;
                }
            }


            // Describe the results
			counter++;
			if (timeout) {
                printf("Failed. response timed out.\n");
				textfile <<counter << "," <<"F,"<<"0"<<"0"<< endl;
            } else {
                // Grab the response, compare, and send to debugging spew
				unsigned long got_time=0;
				unsigned long fulltime=0;
				char aux=1;
				radio.flush_rx();
				
				if (radio.available()) {
                // Dump the payloads until we've gotten everything
                // Fetch the payload, and see if this was the last one.
					while(aux){
						if(got_time<1){
							while (radio.available()) {
								radio.read(&got_time, sizeof(unsigned long));
							}
						}else{
							aux=0;
						}	
					}
	
				}
				
				fulltime=millis()-start_time;

                // Spew it
                printf(" Round-trip delay: %lu.\n" ,fulltime);
				textfile <<counter << "," << "S,"<< fulltime;
            }
			//Delay for 0,3 seg
			started_waiting_at = millis();
			while (!timeout) {
                if (millis() - started_waiting_at > 300) {
                    timeout = true;
                }
            }
			
			
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
			
			//sleep(1);
// ANOTHER CYCLE
/*
//...................................................................added layer
            // First, stop listening so we can talk.
            radio.stopListening();

            // Take the time, and send it.  This will block until complete

            printf("Now sending...");
			unsigned long start_time = millis();                             // Take the time, and send it.  This will block until complete


			while ((!radio.write( &start_time, sizeof(unsigned long)))&&(millis()-start_time<=1000)){
			printf(".");
			}
			printf("\n");
			
            // Now, continue listening
            radio.startListening();

            // Wait here until we get a response, or timeout (200ms)
            unsigned long started_waiting_at = millis();
            bool timeout = false;
            while (!radio.available() && !timeout) {
                if (millis() - started_waiting_at > 200) {
                    timeout = true;
                }
            }


            // Describe the results
			counter++;
			if (timeout) {
                printf("Failed. response timed out.\n");
				textfile <<counter << "," <<"F,"<<"0"<<"0"<< endl;
				
            } else {
                // Grab the response, compare, and send to debugging spew
				unsigned long got_time=0;
				unsigned long fulltime=0;
				
				if (radio.available()) {
                // Dump the payloads until we've gotten everything
                // Fetch the payload, and see if this was the last one.
					while (radio.available()) {
						radio.read(&got_time, sizeof(unsigned long));
					}
				}
				
				fulltime=millis() - got_time;

                // Spew it
                printf("Got response %lu, round-trip delay: %lu, status Alarm: %i \n" , got_time, fulltime,stattusA);
				textfile <<counter << "," << "S,"<< fulltime<<","<<stattusA<< endl;
            }				
            //Delay for 0,3 seg
			unsigned long started_waiting_at = millis();
			while (!timeout) {
                if (millis() - started_waiting_at > 300) {
                    timeout = true;
                }
            }*/			
//................................................
        }
		
        //
        // Pong back role.  Receive each packet, dump it out, and send it back
        //

        if (role == role_pong_back) {

            // if there is data ready
            if (radio.available()) {
                // Dump the payloads until we've gotten everything
                unsigned long got_time;

                // Fetch the payload, and see if this was the last one.
                while (radio.available()) {
                    radio.read(&got_time, sizeof(unsigned long));
                }
                radio.stopListening();

                radio.write(&got_time, sizeof(unsigned long));

                // Now, resume listening so we catch the next packets.
                radio.startListening();

                // Spew it
                printf("Got payload(%d) %lu...\n", sizeof(unsigned long), got_time);

                delay(925); //Delay after payload responded to, minimize RPi CPU time

            }

        }

    } // forever loop
	textfile.close();

    return 0;
}

