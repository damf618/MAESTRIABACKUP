#ifndef RF24DP_H
#define RF24DP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <delay.h>
#include <stdio.h>
#include <RF24/RF24.h>	

#define CE_pin_default 			22
#define CSN_pin_default			0
#define Number_default 			1
#define Contacto_A_default		19
#define Contacto_F_default		26
#define Status_Comm_default		0
#define Reset_Pin_default		20
#define read_timeout_default	3500
#define write_timeout_default	100

#define Comm_timeout 0
#define Comm_received 1
#define Comm_receiving 2


#define NORMAL_CODE 987
#define FAIL_CODE 654
#define ALARM_CODE 321

enum Code_t{
   Normal_Mode=0,
   Fail_Mode,
   Alarm_Mode,
   Receiving_Mode
}; 

typedef enum{
   RECEIVE_CONFIG,
   RECEIVE_RECEIVING,
   RECEIVE_RECEIVED_OK,
   RECEIVE_FULL_BUFFER,
   RECEIVE_TIMEOUT
} ReceiveOrTimeoutState_t;

typedef struct{
   ReceiveOrTimeoutState_t state;
   char*    string;
   uint16_t stringSize;
   uint16_t stringIndex;
   tick_t   timeout;
   delay_t  delay;
} ReceiveOrTimeout_t;


class RF24DP : public RF24	{
	
	private:
		char CE_pin = CE_pin_default; 			//22;
		char CSN_pin = CSN_pin_default;	 		//0;
		char Number = Number_default;			//1;
		char Contacto_A = Contacto_A_default;	//19; 
		char Contacto_F = Contacto_F_default;	//26;
		int Status_Comm = Status_Comm_default;	//0;
		char Reset_Pin = Reset_Pin_default;		//20;
		uint64_t counter=0;
		ReceiveOrTimeout_t Receiver;
		Code_t Code=Receiving_Mode;
		
	
	public:
		RF24DP(uint16_t cepin, uint16_t cspin):RF24{cepin,cspin}{
			RF24(cepin,cspin);  
		}		
		void Init(void);
		void Init(char CE, char CSN, char Num, char CA, char CF, char SC, char Reset, unsigned int timeout);
		void Set_Up(char intentos, char tiempo, char Canal);
		void Writing_Data(void); 
		bool Read_Status(void);
		void Normalization_Delay(void);
		void Read_Data (void);
		ReceiveOrTimeoutState_t  Wait_for_Code ();
		void RF24DPReset();
		void RF24DPUpdate();
		char RF24DPRead();
		int Get_Code();
};


#ifdef __cplusplus
}
#endif
#endif 
