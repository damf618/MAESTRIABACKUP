#include <RF24/RF24.h>				// BIBLIOTECA DE RF24 POR TMRH20
#include <RF24DC.h>
#include <fstream>
#include <sstream>
#include <string>


#define WRITING_MAX_TIME 1000
#define READING_MAX_TIME 200

#define RETRIES_MAX_NUMBER 15			//How many retries before giving up, max 15*
#define RETRIES_MAX_SPACING 15			//0 means 250us, 15 means 4000us, max 15*  

#define NORMALIZATION_TIME 300			//min 300 possible time to send amessage after the reading.

static RF24 radio(22,0);
static const uint8_t pipes[][6] = {"1Node", "2Node","3Node","4Node","5Node"};
uint64_t counter=0;						//message counter
static std::fstream textfile;

void Monitor_Init(Monitor_t Monitor1){
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
		textfile.open("REGISTRO_SISTEMA_ALARMA_INCENDIO.csv");	
	}
}

void Writing_Data(Monitor_t Monitor1){
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

void Read_Data (bool timeout, int statusA){
	
	// Describe the results
	counter++;
	if (timeout) 
	{
		printf("Failed. response timed out.\n");
		textfile <<counter << "," <<"F,"<<"0"<<"0"<< std::endl;
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
		textfile <<","<<statusA<< std::endl;
    }
	Normalization_Delay();	
}
