#include <RF24/RF24.h>				// BIBLIOTECA DE RF24 POR TMRH20



typedef enum {TRANSMITTER,RECEIVER} Working_Mode_t;

typedef struct
{
	char CE_pin= 22;
	char CSN_pin= 0;
	Working_Mode_t mode;
	char Number= 1;
	
}Monitor_t;

void Monitor_Init(char gpio1, char gpio2, char device_number, Monitor_t Monitor1);
void Writing_Data(Monitor_t Monitor1);
void Read_Data(Monitor_t Monitor1);