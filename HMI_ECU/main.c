/******************************************************************************
 *
 * Module: HML.ECU
 *
 * File Name: main.c
 *
 * Description:	Main File For HML ECU
 *
 * Author: Ahmed Yasser
 *
 *******************************************************************************/

#include "MCAL/gpio.h"
#include "HAL/lcd.h"
#include "std_types.h"
#include"common_macros.h"
#include <util/delay.h>
#include "HAL/keypad.h"
#include "MCAL/uart.h"
#include "avr/io.h"
#include"MCAL/timer.h"


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


uint8 freeze=0;
uint8 key;
uint8 Password[5]={0};
uint8 PasswordConfirm[5]={0};
Timer1_ConfigType Config={0,7813,Prescalar_1024,CTC_Mode};



/*
 * Description: Function Used as Callback Function.
 * increase count depends on Number of interrputs.
 * Set/Clear Freeze which used to stop the mcu until Timer End.
 *
 * input: void
 * output: void
 * */
void ErrorMessage_1Min(void){
	static uint8 count=0;
	count++;
	freeze=1;
	if(count ==14){
		freeze=0;
		count=0;
		Timer1_deInit();
	}
}
/*
 * Description: Used to Print Screen number one for ICD
 *
 * Screen one :
 * ==========================================
 * ||Plz enter pass:						||
 * || 										||
 * ==========================================
 * input: void
 * output: void
 * */

void PrintScreen1_LCD(void){
	LCD_clearScreen();
	LCD_displayStringRowColumn(0,0,"Plz enter pass:");
	LCD_moveCursor(1,0);
}

/*
 * Description: Used to Print Screen number two on ICD
 *
 * Screen two :
 * ==========================================
 * ||Plz re-enter pass:						||
 * ||Same Pass:								||
 * ==========================================
 * input: void
 * output: void
 * */

void PrintScreen2_LCD(void){
	LCD_displayStringRowColumn(0,0,"Plz re-enter pass:");
	LCD_displayStringRowColumn(1,0,"Same Pass:");
}


/*
 * Description: Function Used to Read Password For User
 *
 * input: Pointer for Array where saved
 * output: void
 * */

void ReadPassword(uint8 Ptr[]){
	for(uint8 counter=0;counter<5;counter++){
			key=KEYPAD_getPressedKey();
			if((key <= 9) && (key >= 0)){
				Ptr[counter]=key;
				LCD_displayCharacter('*');
				_delay_ms(250);
			}
			else{
				counter--;
			}
		}
}

/*
 * Description: Function Used to Send Password by UART
 *
 * input: Pointer for Array where saved
 * output: void
 * */
void SendPassword(uint8 Ptr[]){
	for(uint8 counter=0;counter<5;counter++){
			UART_sendByte(Ptr[counter]);
		}
}
/*
 * Description: Function Used Waiting on/c Key (Enter Key)
 *
 * input: void
 * output: void
 * */

void WaitEnterKey(){
	while(key!=13){
			key=KEYPAD_getPressedKey();
	}
}

/*
 * Description: Function Used to
 * 1)Print Screen 1 on lcd
 * 2)Read Password Form User
 * 3)Waiting Enter Key
 * 4)Send Password To Contoller
 *
 * input: void
 * output: void
 * */
void CheckPassword(){
	PrintScreen1_LCD();
	ReadPassword(PasswordConfirm);
	WaitEnterKey();
	SendPassword(PasswordConfirm);
}

/*
 * Description: Function Used to
 * 1)Print Screen 2 on lcd
 * 2)Read  Password 2 times Form User
 * 3)Waiting Enter Key
 * 4)Send  2 Passwords To Contoller
 *
 * input: void
 * output: void
 * */

void CreatingPassword(){
	PrintScreen1_LCD();
	ReadPassword(Password);
	WaitEnterKey();

	PrintScreen2_LCD();
	ReadPassword(PasswordConfirm);
	WaitEnterKey();

	SendPassword(Password);
	_delay_ms(10);
	SendPassword(PasswordConfirm);
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
 * Description: Function Used to Freeze Code
 * 1)set Freeze
 * 2)wait until Callback function Clear Freeze
 *
 * input: void
 * output: void
 * */
void PollingUntilFreezeEnd(){
	freeze=1;
	while(freeze);
}


/*
 * Description: OpeningDoor Function
 * prepare the timer and LCD to do the 3 stages of opening the door
 *
 * input: void
 * output: void
 * */
void OpeningDoor(){
	LCD_clearScreen();
	Timer1_init(&Config);
	Timer1_setCallBack(&UnLockingDoor_15S);
	LCD_displayStringRowColumn(0,0,"Door is Unlocking");
	PollingUntilFreezeEnd();

	LCD_clearScreen();
	Timer1_init(&Config);
	Timer1_setCallBack(&Holding_3S);
	LCD_displayStringRowColumn(0,0,"Holding");
	PollingUntilFreezeEnd();

	LCD_clearScreen();
	Timer1_init(&Config);
	Timer1_setCallBack(&UnLockingDoor_15S);
	LCD_displayStringRowColumn(0,0,"Door is Locking");
	PollingUntilFreezeEnd();
}


/*
 * Description: Function Used to Freeze Code
 * 1)set Freeze
 * 2)wait until Callback function Clear Freeze
 *
 * input: void
 * output: void
 * */

void DisplayError_60s(){
	static uint8 count=0;
	count++;
	if(count==59){
		count=0;
		freeze=0;
		Timer1_deInit();
	}
}




void DisPlayErrorMessageAndDisplayInput(){
	LCD_clearScreen();
	Timer1_init(&Config);
	Timer1_setCallBack(&DisplayError_60s);
	LCD_displayStringRowColumn(0,0,"ERROR!!");
	PollingUntilFreezeEnd();
}



int main(){
	SET_BIT(SREG,7); // Enable Interrupts

	UART_ConfigType UartConfig={Eight_Bit,Disabled,One_Bit,9600};


	UART_init(&UartConfig);
	LCD_init();


	while(UART_recieveByte()!=CONTROL_READY);	// waiting Control end his part
	while(1){

		UART_sendByte(REG_PASSWORD);   // check if there is Password in EEPROM or not
 		uint8 order=UART_recieveByte();	// waiting the answer of Control MC


		if(order==PASSWORD_COMFIRM){			// if there is a Password
			uint8 key2='a';
			LCD_clearScreen();
			LCD_displayStringRowColumn(0,0,"+  : Open Door");
			LCD_displayStringRowColumn(1,0,"-  : Change Pass");
			while(key2!='+' && key2!='-'){			//	Waiting until user Press the Correct Key
				key2=KEYPAD_getPressedKey();
			}

			/*
			 * here we will Get Password and send to controller to check it
			 * looping until the 3 tries end or the user enter the right password
			 * */
			uint8 try=3;
			uint8 answer;
			do{
			try--;
			UART_sendByte(CHECKPASSWORD);
			CheckPassword();
			answer=UART_recieveByte();
			}while(try>0 && answer!=SAVEDPASSWORD);

			if(try==0 && answer==SAVEDPASSWORD){
				try=3;
			}
			/*
			 * here we Send to controller to start to Rotate the motor
			 * and we prepare the LCD to show the messages while rotating the motor
			 * */
			if(key2=='+' &&try>0){
				UART_sendByte(ROTATE_MOTOR);
				OpeningDoor();
				uint8 answerOfMotor;
				do{
				  answerOfMotor=UART_recieveByte();
				}while(answerOfMotor!=ROTATE_MOTOR);
			}

			/*
			 * Here we just send to controller to Remove the Password form EEPROM
			 * and waiting it EREASE it
			 * */
			else if (key2=='-'&&try>0){
				UART_sendByte(EREASEPASSWORD);
				uint8 answerOfDeletedPassword;
				do{
					answerOfDeletedPassword =UART_recieveByte();
				}while(answerOfDeletedPassword!=EREASEPASSWORD);
			}

			/*
			 *If the two passwords are not matched for the third consecutive time, then:
					• Activate Buzzer for 1-minute.
					• Display error message on LCD for 1 minute.
					• System should be locked no inputs from Keypad will be accepted during
					  this time period
			 * */
			else{
				UART_sendByte(OPENBUZZER);
				DisPlayErrorMessageAndDisplayInput();
				uint8 answerOfBuzzer;
				do{
					answerOfBuzzer=UART_recieveByte();
				}while(answerOfBuzzer!=OPENBUZZER);
			}
		}
		else{											// if there is no Password in EEPROM
			uint8 answer;
			do{
			UART_sendByte(CREATINGPASSWORD);			// Send to Controller to setup and be ready to Creating Password
			CreatingPassword();
			answer=UART_recieveByte();
			}while(answer!=SAVEDPASSWORD);				// looping until Controller write Password Correct in EEPROM
		}
	}

}



