
/*
* This code is a prototype sketch for the specialization in Embedded SYstem 
* is based on the RF24 library created by TMRh20 
* 
*/

#include <SPI.h>
#include "RF24.h"
#include "printf.h"


/****************** User Config ***************************/
/***      Set this radio as radio number 0 or 1         ***/
bool radioNumber = 0;

/* Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8 */
RF24 radio(6,8);
/**********************************************************/

byte addresses[][6] = {"1Node","2Node"};

// Used to control whether this node is sending or receiving
bool role = 0;
//Monitoring Pins for Fire Alarm and System Troubles
char ContactoA=3;
char ContactoF=4;
//Reset Pin for controlling the reset process
char resetPin=5;

//Status for Fire Alarm and System Troubles
int stattusA=0;
int stattusF=0;

char auxiliar=0;

void setup() {
  digitalWrite(resetPin,HIGH);
  Serial.begin(115200);
  Serial.println(F("*** PRESS 'T' to begin transmitting to the other node"));
  pinMode(ContactoA, INPUT);
  pinMode(ContactoF, INPUT);
  pinMode(resetPin, OUTPUT);
  radio.begin();
  radio.setChannel(115);
  radio.setPALevel(RF24_PA_LOW);
  //radio.printDetails();

  //radio.setChannel(55);   
  // Open a writing and reading pipe on each radio, with opposite addresses
  if(radioNumber){
    radio.openWritingPipe(addresses[1]);
    radio.openReadingPipe(1,addresses[0]);
  }else{
    radio.openWritingPipe(addresses[0]);
    radio.openReadingPipe(1,addresses[1]);
  }
  
  // Start the radio listening for data
  radio.startListening();
}


void loop() {
  
  
/****************** Ping Out Role ***************************/  
if (role == 1)  {
    radio.stopListening();                                    // First, stop listening so we can talk.
    Serial.println(F("Now sending"));
    unsigned long start_time = micros();                      // Take the time, and send it.  This will block until complete
    
    while ((!radio.write( &start_time, sizeof(unsigned long)))&&(micros()-start_time<=1000000)){ // It will print a point "." 
       Serial.print(F("."));                                  //continously for 10 seconds if the device doesn't get a correct transmition
    }
    
    Serial.println(F("/n done"));                              // Once the message was transmitted succesfully or  
    radio.startListening();                                    // the 10seconds window has transcurred.We start listening.
    
    unsigned long started_waiting_at = micros();               // Set up a timeout period, get the current microseconds
    boolean timeout = false;                                   // Set up a variable to indicate if a response was received or not
    
    while ( ! radio.available() ){                             // While nothing is received
      if (micros() - started_waiting_at > 500000 ){            // If waited longer than 500ms, indicate timeout and exit while loop
          timeout = true;
          break;
      }      
    }
        
    if ( timeout ){                                             // Describe the results
        Serial.println(F("Failed, response timed out."));
    }else{
        unsigned long got_time;                                 // Grab the response, compare, and send to debugging spew
        radio.read( &got_time, sizeof(unsigned long) );
        unsigned long end_time = micros();
        
        // Let's print howmuch timedoesittake to stablish a full message to make a cycle.
        Serial.print(F("Sent "));
        Serial.print(start_time);
        Serial.print(F(", Got response "));
        Serial.print(got_time);
        Serial.print(F(", Round-trip delay "));
        Serial.print(end_time-start_time);
        Serial.println(F(" microseconds"));
    }

    // Try again 1s later
    delay(1000);
  }



/****************** Pong Back Role ***************************/

  if ( role == 0 )
  {
    unsigned long check_time = millis();
    char allok=1;
    
    while(!radio.available()){                                     // Reset Code if the device staysmore than 5 seconds in waiting mode
      if(millis()-check_time>=5000){                               // it will restart and try again
      digitalWrite(resetPin,LOW);
      }
    }
      
    unsigned long got_time=0;
    
    
    if( radio.available()==1){
                                                                    // Variable for the received timestamp
      while (radio.available()) {                                   // While there is data ready
        radio.read( &got_time, sizeof(unsigned long) );             // Get the payload
        //stattus=digitalRead(ContactoF);
      }
     
      radio.stopListening();                                        // First, stop listening so we can talk   
      while ((!radio.write( &got_time, sizeof(unsigned long)))&&(micros()-got_time<=1000000)){
       Serial.print(F("."));
      }
      
      while (radio.available()) {                                   // While there is data ready
       radio.read( &stattusA, sizeof(int) );                       // Get the payload
      //stattus=digitalRead(ContactoF);
      }
      
      radio.stopListening();
      stattusA=digitalRead(ContactoA);
      while ((!radio.write( &stattusA, sizeof(int)))&&(micros()-got_time<=1000000)){
      Serial.print(F("."));
      }
      Serial.print(F("\n"));
/*..........................................................*/      
      radio.startListening();                                       // Now, resume listening so we catch the next packets.     
      Serial.print(F("Sent response "));
      Serial.print(got_time);  
      Serial.print(F(" Status Alarm: "));
      Serial.println(stattusA);
      
   }
  
 }




/****************** Change Roles via Serial Commands ***************************/
 
  if ( Serial.available()==1)
  {
    char c = toupper(Serial.read());
    Serial.println(c);
    if ( c == 'T' && role == 0 ){      
      Serial.println(F("*** CHANGING TO TRANSMIT ROLE -- PRESS 'R' TO SWITCH BACK"));
      role = 1;                  // Become the primary transmitter (ping out)
    
   }else if ( c == 'R' && role == 1 ){
      Serial.println(F("*** CHANGING TO RECEIVE ROLE -- PRESS 'T' TO SWITCH BACK"));      
       role = 0;                // Become the primary receiver (pong back)
       radio.startListening();
       
    }
    else{
      Serial.println(F("*** BRO WTF ?"));
    }
    
  }


} // Loop
