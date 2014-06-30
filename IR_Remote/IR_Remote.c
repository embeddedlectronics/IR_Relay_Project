/*
 * IR_Remote.c
 *
 * Created: 5/27/2014 4:19:27 PM
 *  Author: Michael
 */ 
#define F_CPU 16000000UL  //16Mhz clock


#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h> // provides delays
#include <avr/power.h> // used for setting clock prescaler
#include <stdbool.h>
#include <avr/interrupt.h>
#include <string.h>


#include "Atmega_Libs/AtmegaPins.h"
#include "Atmega_Libs/Serial_Atmel328p.h"
#include "Libs/HD44780.h"
#include "Libs/IR_Sensor.h"




#define HIGH_LOW_LOGIC_CHANGE 	EICRA |= (1 << ISC01) & ~(1 << ISC00);    // set INT0 to trigger on ANY logic change
#define LOW_HIGH_LOGIC_CHANGE	EICRA |= (1 << ISC01) | (1 << ISC00); // LOW -> high transistion trigger



int main(void)
{


	USART_init();
	fdevopen(USART_send, NULL); // use printf yyay

	printf("\n INIT "); 
	
	SETBIT(IR_PIN_DDR, IR_Output);
	CLEARBIT(IR_PIN_DDR, IR_Input);
	

/// Initalize I2C	
	I2C_Initalize();
	
/// INITALIZE 8 Seg 
	_74HC164_Initialize(); 

//// SET RELAY 
	SETBIT(_Relay__DDR, _Relay_1_Pin);
	SETBIT(_Relay__DDR, _Relay_2_Pin);
	
	
	SETBIT(_Relay__PORT, _Relay_1_Pin);
	SETBIT(_Relay__PORT, _Relay_2_Pin);
	
	IR_Initalize();
		//// Read the input clock signal//
		_IR_Get_Pules = 1;
	
	

    while(1)
    {
			SETBIT(IR_PORT, IR_Output);
			_delay_us(100); 
			CLEARBIT(IR_PORT, IR_Output);
			_delay_us(100); 
				
		if (_IR_Read_State){
			_counter_ = 0;
			IR_Fun();
			_IR_Read_State = 0;
		}
	

	}
}



