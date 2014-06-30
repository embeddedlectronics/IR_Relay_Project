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
#include "_74HC164.h"


#define IR_Input PORTD2 // input for ir
#define IR_Output PORTD3 // input for ir

#define IR_PIN_DDR DDRD   // data direction port for the pins
#define IR_PORT PORTD
#define IR_PIN_PORT PIND

#define IRpin_PIN      PIND2
#define IRpin          2


#define _Relay_1_Pin	PORTB0
#define _Relay_2_Pin	PORTB1

#define _Relay__PORT	PORTB
#define _Relay__DDR	DDRB

#define ANY_LOGIC_CHANGE 	EICRA |= (1 << ISC00) & ~(1 << ISC01);    // set INT0 to trigger on ANY logic change

volatile _Delay_1_Trigger;
volatile _Delay_2_Trigger;
volatile _Clock_Pulse_Trigger;

volatile char _IR_Read_State;
volatile char _Delay_3_Trigger;
volatile char _counter_;
volatile char _binary_counter_;
volatile char _binary_array_counter_;
volatile char _count_delay;
volatile char _counter_ON ;
volatile char _counter_OFF;
volatile char _clear ;


volatile char _IR_Clock_Pulses;
volatile char _IR_Get_Pules;

uint16_t _Delay_Time;
uint16_t _half_time ;

int delay ;

// when i hit the button slow it counts as two unputs

int Pulse_Time[5];

uint16_t _Store_Input_Signal[32];
uint16_t _Store_ON_Pulse[80];



int _IR_Remote_Buttons[4][9]; 


char IR_Compare(int *_IR_Input, int * _Existing_Code); 
void IR_Initalize(void);
void IR_Analyze(void);
void IR_Determine_Signal_Length(void);

#endif /* IR_SENSOR_H_ */