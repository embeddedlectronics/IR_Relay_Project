/*
 * _74HC164.h
 *
 * Created: 3/4/2013 11:01:21 AM
 *  Author: mfolz
 */ 
#ifndef _74HC164_H_INCLUDED_
#define _74HC164_H_INCLUDED_


#include "../Atmega_Libs/AtmegaPins.h"
#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/io.h>

/// A0 - Clock 
/// A1 - Data
/// A2 - CS

#define  _74HC164_DDR  DDRC
#define  _74HC164_PORT PORTC
#define  _74HC164_Clock PORTC0
#define  _74HC164_Data PORTC2
#define	 _74HC164_CS	PORTC1

#define  _74HC164_Clock_Low()	CLEARBIT(_74HC164_PORT, _74HC164_Clock) // pull clock low
#define  _74HC164_Clock_High()	SETBIT(_74HC164_PORT,_74HC164_Clock) // pull clock high
#define  _74HC164_Data_Low() CLEARBIT(_74HC164_PORT, _74HC164_Data) // pull data low
#define  _74HC164_Data_High() SETBIT(_74HC164_PORT, _74HC164_Data) // pull data low
#define  _74HC164_CS_Low() CLEARBIT(_74HC164_PORT, _74HC164_CS) // pull data low
#define  _74HC164_CS_High()	 SETBIT(_74HC164_PORT, _74HC164_CS) // pull data low


char _8_Segment_Active_Low[10];

void _74HC164_ChipSelect(char chip);
void _74HC164_SendData(char data); 
void _74HC164_Initialize(void);


#endif /* 74HC164_H_ */
