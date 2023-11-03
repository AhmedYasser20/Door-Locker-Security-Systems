/******************************************************************************
 *
 * Module: Timer
 *
 * File Name: timer.c
 *
 * Description: Source file for the Motor driver
 *
 * Author: Ahmed Yasser
 *
 ******************************************************************************/


#include "../MCAL/gpio.h"
#include "dc_motor.h"
#include "../MCAL/pwn.h"
/*
 * Description:
 * DcMotor initial Fucntion
 * 1-set PIN0 and PIN1 in Port of motor to output
 * 2-Write inital value on the two pins
 */



void DcMotor_Init(void){
	GPIO_setupPinDirection(DC_MOTOR_PORT,DC_MOTOR_PIN0,PIN_OUTPUT);
	GPIO_setupPinDirection(DC_MOTOR_PORT,DC_MOTOR_PIN1,PIN_OUTPUT);
	GPIO_writePin(DC_MOTOR_PORT,DC_MOTOR_PIN0,(State_Stop&~(1<<0)));
	GPIO_writePin(DC_MOTOR_PORT,DC_MOTOR_PIN1,(State_Stop&~(1<<1)));
};


/*
 * Description:
 * DcMotor Rotate Function
 * Set the state and speed of motor
 * write State on two pins of motor
 * then set PWN in Timer0
 * input: State , unsigned char speed
 */
void DcMotor_Rotate(DcMotor_State state,uint8 speed){
	if(state==State_Stop)
		{
		GPIO_writePin(DC_MOTOR_PORT,DC_MOTOR_PIN0,LOGIC_LOW);
		GPIO_writePin(DC_MOTOR_PORT,DC_MOTOR_PIN1,LOGIC_LOW);
		}
		else if(state==State_AntiClockWise)
		{
			GPIO_writePin(DC_MOTOR_PORT,DC_MOTOR_PIN0,LOGIC_LOW);
			GPIO_writePin(DC_MOTOR_PORT,DC_MOTOR_PIN1,LOGIC_HIGH);

		}
		else if(state==State_ClockWise)
			{
			GPIO_writePin(DC_MOTOR_PORT,DC_MOTOR_PIN0,LOGIC_HIGH);
			GPIO_writePin(DC_MOTOR_PORT,DC_MOTOR_PIN1,LOGIC_LOW);
			}
	PWM_Timer0_Start(speed);

}
