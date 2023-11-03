/******************************************************************************
 *
 * Module: Timer
 *
 * File Name: timer.c
 *
 * Description: Header file for the Timer1 driver
 *
 * Author: Ahmed Yasser
 *
 *******************************************************************************/

#ifndef MCAL_TIMER_H_
#define MCAL_TIMER_H_

#include"../std_types.h"
#include"../common_macros.h"

// Enumerated type to define Timer1 prescaler options
typedef enum
{
    Prescalar_1 = 1,
    Prescalar_8,
    Prescalar_64,
    Prescalar_256,
    Prescalar_1024
} Timer1_Prescaler;

// Enumerated type to define Timer1 operating modes
typedef enum
{
    Normal_Mode,
    CTC_Mode = 4
} Timer1_Mode;

// Configuration structure for Timer1
typedef struct
{
    uint16 intial_value;        // Initial value of the timer
    uint16 compare_value;       // Compare value for the timer (used in CTC mode)
    Timer1_Prescaler prescaler; // Selected prescaler
    Timer1_Mode mode;           // Selected operating mode
} Timer1_ConfigType;

// Function to initialize Timer1 with the given configuration
void Timer1_init(const Timer1_ConfigType *Config_Ptr);

// Function to deinitialize Timer1
void Timer1_deInit(void);

// Function to set a callback function for Timer1 overflow or compare match interrupt
void Timer1_setCallBack(void (*a_ptr)(void));

#endif /* MCAL_TIMER_H_ */
