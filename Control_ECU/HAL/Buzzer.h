/******************************************************************************
 *
 * Module: Timer
 *
 * File Name: timer.c
 *
 * Description: Header file for the Buzzer driver
 *
 * Author: Ahmed Yasser
 *
 ******************************************************************************/

#ifndef HAL_BUZZER_H_
#define HAL_BUZZER_H_
#include"../MCAL/gpio.h"


#define BUZZER_PIN       PIN7_ID
#define BUZZER_PORT      PORTB_ID

void Buzzer_init();
void Buzzer_on();
void Buzzer_off();

#endif /* HAL_BUZZER_H_ */
