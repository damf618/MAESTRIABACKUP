#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <RF24/RF24.h>
using namespace std;

RF24 radio(22, 0);

const uint8_t pipes[][6] = {"1Node", "2Node"};

char Msg[3]={15,30,45};


int main(int argc, char** argv)
{
	cout << "TODO BIEN MENOR "<<endl;
	cout << "MASTER SOLICITANDO UN UNICO MENSAJE "<<endl;
	
	radio.begin();
    radio.setRetries(15, 15);
	radio.setChannel(108);
	radio.setPALevel(RF24_PA_LOW);
    radio.printDetails();

	cout << "Now writing "<<endl;
	radio.stopListening();
	
	radio.openWritingPipe(pipes[1]);
    radio.openReadingPipe(1, pipes[0]);
	
	delay(925); 
	while(1){
		for(int i=0;i<=2;i++)
		{	
    		if (!radio.write(&Msg[i], sizeof(char)))
			{
				cout<<"failed: "<<Msg[i]<<endl;
			}
			else
			{
				cout<<"ok!: "<<Msg[i]<<endl;
			}	
			delay(925); 
		}
	}

}