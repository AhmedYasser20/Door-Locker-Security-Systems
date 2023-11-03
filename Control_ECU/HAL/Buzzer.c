/******************************************************************************
 *
 * Module: Timer
 *
 * File Name: timer.c
 *
 * Description: Source file for the Buzzer driver
 *
 * Author: Ahmed Yasser
 *
 ******************************************************************************/
#include"Buzzer.h"
#include"../std_types.h"
void Buzzer_init()
{
	GPIO_setupPinDirection(BUZZER_PORT, BUZZER_PIN,PIN_OUTPUT);
	GPIO_writePin(BUZZER_PORT, BUZZER_PIN,LOGIC_LOW);
}
void Buzzer_on()
{
	GPIO_writePin(BUZZER_PORT, BUZZER_PIN,LOGIC_HIGH);
}
void Buzzer_off()
{
	GPIO_writePin(BUZZER_PORT, BUZZER_PIN,LOGIC_LOW);
}
