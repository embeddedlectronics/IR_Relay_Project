/*
 * IR_Sensor.h
 *
 * Created: 5/27/2014 4:39:50 PM
 *  Author: Michael
 */ 


#ifndef IR_SENSOR_H_INCLUDED_
#define IR_SENSOR_H_INCLUDED_

#define F_CPU 16000000UL
#include <util/delay.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <math.h>
#include <avr/interrupt.h>

#include "../Atmega_Libs/AtmegaPins.h"
#include "../Atmega_Libs/Serial_Atmel328p.h"

// Define Signal Paramaters here 
#define IR_Initial_Low_Pulse_Minimum	200
#define IR_Initial_Low_Pulse_Maximum	3000   // Values are multiples of 0.0065ms 
#define IR_Error_Tollerance_			20 // Not a percentage value but a value in 0.0065ms // its usually a good idea to have a value around 15-20

// Define IR Input here
#define IR_Input PORTD2 // input for ir
#define IR_PIN_DDR DDRD   // data direction port for the pins
#define IR_PORT PORTD
#define IR_PIN_PORT PIND

#define IRpin_PIN      PIND2
#define IRpin          2
//



#define ANY_LOGIC_CHANGE 	EICRA |= (1 << ISC00) & ~(1 << ISC01);    // set INT0 to trigger on ANY logic change
#define HIGH_LOW_LOGIC_CHANGE 	EICRA |= (1 << ISC01) & ~(1 << ISC00);    // set INT0 to trigger on ANY logic change
#define LOW_HIGH_LOGIC_CHANGE	EICRA |= (1 << ISC01) | (1 << ISC00); // LOW -> high transistion trigger




struct _IR_Signal{
	volatile char _IR_Status_Trigger;
	volatile char _IR_Get_Pulses; 
	volatile char _IR_Read_State;

	 
	uint16_t _Delay_Time;
	uint16_t _Delay_Variable;

	char _IR_Clock_Pulses; 
	char _IR_Counter_Variable; // used to count clock pulses 
	char _IR_Array_Counter;  // used to count current location in Input signal array
	char _IR_Array_Binary_Counter_; // used to count from 0-16 
	char _IR_Count_Delay; //
	char _IR_Vers_Variable; // var used for various applications
		
	uint16_t _IR_Pulse_Time[5]; // stores all 5 portions of the start pulse 
	uint16_t _IR_Half_Pulse;  // stores the previous pulse  
	uint16_t _IR_Store_Input_Signal[16]; // stores signals 256 pulses length // anything beyond that is insane. 
	
}Properties;



int _IR_Remote_Buttons[4][9];

char IR_Compare(int *_IR_Input, int * _Existing_Code); 
void IR_Initalize(void);




#endif /* IR_SENSOR_H_ */