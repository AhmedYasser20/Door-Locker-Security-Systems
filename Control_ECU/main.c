/******************************************************************************
 *
 * Module: Control_ECU
 *
 * File Name: main.c
 *
 * Description:	Main File For Controller MCU
 *
 * Author: Ahmed Yasser
 *
 *******************************************************************************/

#include "std_types.h"
#include <util/delay.h>
#include "MCAL/uart.h"
#include "HAL/external_eeprom.h"
#include"HAL/dc_motor.h"
#include "avr/io.h"
#include"MCAL/timer.h"
#include "HAL/Buzzer.h"
#include "MCAL/twi.h"


/*	Define messages or signals
 * 	That helps to interact between 2 MCU
*/
#define REG_PASSWORD 0x10			// Asking For Password is existed or not
#define CREATINGPASSWORD 0x12		// Signal to Creating Password and Rewrite in memory
#define SAVEDPASSWORD 0x13			// Password is correct
#define WRONGPASSWORD 0x14			// Wrong Password
#define PASSWORD_COMFIRM 0x15		// Memory had to Write Safe
#define PASSWORD_NOTCOMFIRM 0x17	// Error while writing on memory
#define CHECKPASSWORD 0x20			// Signal to Read Password and Compare it with memory (Real Password)
#define EREASEPASSWORD 0x21			// Remove memory
#define OPENBUZZER 0x25				// Signal to Open buzzer
#define ROTATE_MOTOR 0x30			// signal to Start Open Door
#define HMI_READY 0x35				// Ready Signal For HML
#define CONTROL_READY 0x40			// Ready Signal For Controller


/*	global Var.*/
uint8 Password[5]={0};
uint8 Password2[5]={0};
uint8 freeze=0;
Timer1_ConfigType Config={0,7813,Prescalar_1024,CTC_Mode};

/*
 * Description: Function Used to Receive Password by UART
 *
 * input: Pointer for Array where saved
 * output: void
 * */

void ReceivePassword(uint8 ptr[]){
	for(int i=0;i<5;i++){
			uint8  x =UART_recieveByte();
			ptr[i]=x;
		}
}
/*
 * Description: Function Used to Read Password form EEPROM
 * and save it into the Array
 *
 * input: Pointer for Array where saved
 * output: void
 * */
void ReadPasswordFormEEPROM(uint8 ptr[]){
	for(int i=0;i<5;i++){
			EEPROM_readByte(i,&ptr[i]);
			_delay_ms(10);
		}
}


/*
 * Description: Function Used to Compare the 2 passwords where store in Global Arrays Password and Password2
 * if not matched return 0
 * else return 1
 *
 * input: void
 * output: Boolen True or False
 *
 * */

uint8 CompareTheTwoPasswords(){
	for(uint8 counter=0;counter<5;counter++){
			if(Password[counter]!=Password2[counter]){
				return 0;
			}
		}
	return 1;
}



void CheckPassword(){
	uint8 flag=1;
	ReceivePassword(Password);
	ReadPasswordFormEEPROM(Password2);
	flag=CompareTheTwoPasswords();
	if(flag){
		UART_sendByte(SAVEDPASSWORD);
	}
	else{
		UART_sendByte(WRONGPASSWORD);
	}
}




void CreatingPassword(){
	uint8 flag=1;
	ReceivePassword(Password);
	ReceivePassword(Password2);

	flag=CompareTheTwoPasswords();
	if(flag){
		for(uint8 counter=0;counter<5;counter++){
			EEPROM_writeByte(counter, Password[counter]); /* Write 0x0F in the external EEPROM */
			_delay_ms(10);
		}
		EEPROM_writeByte(0x0100,1);
		_delay_ms(10);
		UART_sendByte(SAVEDPASSWORD);
	}
	else{
		UART_sendByte(WRONGPASSWORD);
	}

}

/*
 * Description: Function Used as Callback Function.
 * increase count depends on Number of interrputs.
 * Set/Clear Freeze which used to stop the mcu until Timer End.
 *
 * input: void
 * output: void
 * */
void UnLockingDoor_15S(){
	static uint8 count=0;
	count++;
	if(count==14){
	count=0;
	freeze=0;
	Timer1_deInit();
	}
}


/*
 * Description: Function Used as Callback Function.
 * increase count depends on Number of interrputs.
 * Set/Clear Freeze which used to stop the mcu until Timer End.
 *
 * input: void
 * output: void
 * */
void Holding_3S(){
	static uint8 count=0;
		count++;
		if(count==2){
		count=0;
		freeze=0;
		Timer1_deInit();
		}
}

/*
 * Description: OpeningDoor Function
 * prepare the timer and Motor to do the 3 stages of opening the door
 *
 * input: void
 * output: void
 * */
void OpeningDoor(){
	Timer1_init(&Config);
	Timer1_setCallBack(&UnLockingDoor_15S);
	DcMotor_Rotate(State_ClockWise, 255);
	freeze=1;
	while(freeze);
	Timer1_init(&Config);
	Timer1_setCallBack(&Holding_3S);
	DcMotor_Rotate(State_Stop, 0);
	freeze=1;
	while(freeze);
	Timer1_init(&Config);
	Timer1_setCallBack(&UnLockingDoor_15S);
	DcMotor_Rotate(State_AntiClockWise, 255);
	freeze=1;
	while(freeze);
	DcMotor_Rotate(State_Stop, 0);
}


/*
 * Description: Function Used as Callback Function.
 * increase count depends on Number of interrputs.
 * Set/Clear Freeze which used to stop the mcu until Timer End.
 *
 * input: void
 * output: void
 * */
void OpenBuzzer_60s(){
	static uint8 count=0;
	count++;
	if(count==59){
		count=0;
		freeze=0;
		Timer1_deInit();
	}
}

/*
 * Description: Setup Timer and Buzzer to the ERROR Stage
 *
 * input: void
 * output: void
 * */
void OpenBuzzer(){
	Timer1_init(&Config);
	Timer1_setCallBack(&OpenBuzzer_60s);
	Buzzer_on();
	freeze=1;
	while(freeze);
	Buzzer_off();
}



int main(){
	TWI_ConfigType	TwiConfig={FastMode,1};
	SET_BIT(SREG,7);
	UART_ConfigType UartConfig={Eight_Bit,Disabled,One_Bit,9600};



	UART_init(&UartConfig);
	Buzzer_init();
	TWI_init(&TwiConfig);
	DcMotor_Init();

	uint8 SavedPassword;
	//EEPROM_readByte(0x0100,&SavedPassword);
	//if(SavedPassword==0)
	EEPROM_writeByte(0x0100,0);

	_delay_ms(10);
	UART_sendByte(CONTROL_READY);

	while(1){
		uint8 order=UART_recieveByte();

		/*
		 * Here We check the flag in address 100 in EEPROM
		 * to see if there is a password or no in  EEPROM
		 * then Send if there is a password or no
		 * */
		if(order==REG_PASSWORD){
			EEPROM_readByte(0x0100,&SavedPassword);
			if(SavedPassword)
				UART_sendByte(PASSWORD_COMFIRM);
			else
				UART_sendByte(PASSWORD_NOTCOMFIRM);
		}

		/*
		 * here if we can order to Creating password
		 * we read the 2 passwords form uart then write the password on EEPROM
		 * */
		else if(order==CREATINGPASSWORD){
			CreatingPassword();
		}
		else if(order==CHECKPASSWORD){
			CheckPassword();
		}
		else if(order==ROTATE_MOTOR){
			OpeningDoor();
			UART_sendByte(ROTATE_MOTOR);
		}

		// To Erease the password just Clear the Flag and overwrite on it
		else if(order==EREASEPASSWORD){
			EEPROM_writeByte(0x0100,0);
			_delay_ms(10);
			UART_sendByte(EREASEPASSWORD);
		}
		else if(order==OPENBUZZER){
			OpenBuzzer();
			UART_sendByte(OPENBUZZER);
		}
	}

}
