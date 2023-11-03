#include "avr/io.h"
#include "pwn.h"
#include "../common_macros.h"
/*
 * Description:
 * Generate a PWM signal with frequency 500Hz
 * Timer0 will be used with pre-scaler F_CPU/8
 * F_PWM=(F_CPU)/(256*N) = (10^6)/(256*8) = 500Hz
 * Duty Cycle can be changed by updating the value
 * in The Compare Register
 */

void PWM_Timer0_Start(uint8 duty_cycle)
{

	CLEAR_BIT(TCNT0,0);			// Set Timer Initial Value to 0
	OCR0  = duty_cycle; 	//Set Compare value
	DDRB  = DDRB | (1<<PB3); // Configure PB3/OC0 as output pin --> pin where the PWM signal is generated from MC
	TCCR0 = (1<<WGM00) | (1<<WGM01) | (1<<COM01) | (1<<CS01);
}
