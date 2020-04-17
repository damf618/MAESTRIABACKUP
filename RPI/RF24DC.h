#if !defined( RF24DC )
#define RF24DC

typedef struct {
	char CE_pin = 22;
	char CSN_pin = 0;
	char Number = 1;
	int Status_A = 0;
	int Status_F = 0;
	int Status_Comm = 0;
	
}Monitor_t;


void Monitor_Init(char gpio1, char gpio2, char device_number, Monitor_t Monitor1);
void Writing_Data(Monitor_t Monitor1);
bool Read_Status();
void Read_Data(bool timeout, int statusA);
#endif 
