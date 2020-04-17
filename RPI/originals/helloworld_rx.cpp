/*
 Update 2014 - TMRh20
 */

/**
 * Simplest possible example of using RF24Network,
 *
 * RECEIVER NODE
 * Listens for messages from the transmitter and prints them out.
 */

#include <RF24/RF24.h>
#include <RF24Network/RF24Network.h>
#include <iostream>
#include <ctime>
#include <stdio.h>
#include <time.h>


/**
 * g++ -L/usr/lib main.cc -I/usr/include -o main -lrrd
 **/
//using namespace std;

// CE Pin, CSN Pin, SPI Speed

// Setup for GPIO 22 CE and GPIO 25 CSN with SPI Speed @ 1Mhz
//RF24 radio(RPI_V2_GPIO_P1_22, RPI_V2_GPIO_P1_18, BCM2835_SPI_SPEED_1MHZ);

// Setup for GPIO 22 CE and CE0 CSN with SPI Speed @ 4Mhz
//RF24 radio(RPI_V2_GPIO_P1_15, BCM2835_SPI_CS0, BCM2835_SPI_SPEED_4MHZ); 

// Setup for GPIO 22 CE and CE1 CSN with SPI Speed @ 8Mhz
RF24 radio(RPI_V2_GPIO_P1_15, BCM2835_SPI_CS0, BCM2835_SPI_SPEED_8MHZ);  

RF24Network network(radio);

// Address of our node in Octal format
const uint16_t this_node = 00;
//5858588
const uint8_t addresses[][6] = {"1Node","2Node"};

unsigned long last_sent;             // When did we last send?
unsigned long packets_sent;          // How many have we sent already

int aux=0;

struct payload_t {                  // Structure of our payload
  unsigned long ms;
  unsigned long counter;
  unsigned long ID;	
};

int main(int argc, char** argv) 
{
	// Refer to RF24.h or nRF24L01 DS for settings

	radio.begin();
	
	delay(5);
	network.begin(/*channel*/ 115, /*node address*/ this_node);
	radio.printDetails();
	unsigned long started_waiting_at = millis();
	
	while(1)
	{
	
	if (millis() - started_waiting_at > 200 ){            // If waited longer than 200ms, indicate timeout and exit while loop
		  network.begin(/*channel*/ 115, /*node address*/ this_node);		
		  network.update();
  		  while ( network.available() ) {     // Is there anything ready for us?
    			
		 	RF24NetworkHeader header;        // If so, grab it and print it out
   			 payload_t payload;
  			 network.read(header,&payload,sizeof(payload));
			
			printf("Received payload # %lu at %lu from #ID: %lu \n",payload.counter,payload.ms,payload.ID);
		 	started_waiting_at = millis();
		  }
		delay(50);
		 //sleep(2);
		 //fclose(pFile);
	}
	radio.begin();
	radio.setRetries(15,15);
	radio.setChannel(115);
	radio.setPALevel(RF24_PA_LOW);
    radio.openWritingPipe(addresses[0]);
    radio.openReadingPipe(1,addresses[1]);
	radio.startListening();
	radio.printDetails();
	delay(50);
		if( radio.available()){                                                                // Variable for the received timestamp
      while (radio.available()) {                                   // While there is data ready
        radio.read( &aux, sizeof(int) );             // Get the payload
      }
    	radio.stopListening();                                        // First, stop listening so we can talk   	
		printf("Received Individual :# %5d ",aux);	
	}
	}
	return 0;
}

