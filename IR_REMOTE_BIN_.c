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


#define ANY_LOGIC_CHANGE 	EICRA |= (1 << ISC00) & ~(1 << ISC01);    // set INT0 to trigger on ANY logic change
#define HIGH_LOW_LOGIC_CHANGE 	EICRA |= (1 << ISC01) & ~(1 << ISC00);    // set INT0 to trigger on ANY logic change
#define LOW_HIGH_LOGIC_CHANGE	EICRA |= (1 << ISC01) | (1 << ISC00); // LOW -> high transistion trigger

volatile _Delay_1_Trigger = 0;
volatile _Delay_2_Trigger = 0;  
volatile _counter_ =0; 

uint16_t _Delay_Time =0; 
uint16_t _half_time =0; 


// when i hit the button slow it counts as two unputs 

int Pulse_Time[30];
char Store_Pulse[120]; // store pulses here// good enough for 80ms of data//

void IR_Analyze(void);
void IR_Delay(char delay); 	


int main(void)
{
	uint16_t timer; 
	USART_init();
	fdevopen(USART_send, NULL); // use printf yyay

	printf("\n INIT "); 
	IR_Test();

		//IR_Analyze(); 
	
	//EICRA |= (1 << ISC00);    // set INT0 to trigger on ANY logic change
	ANY_LOGIC_CHANGE; 
	
	
	//HIGH_LOW_LOGIC_CHANGE;
	
	EIMSK |= (1 << INT0);     // Turns on INT0

	sei();

	//Configure timer
	TCCR1B |= (1<<CS12) | ( 1 << CS10) &  ~(1 << CS11) ;    //1:256 prescaler
	
    while(1)
    {


// 		
// 		if (_Delay_1_Trigger){
// 			//printf("Delay 1: %u  \n\r",Pulse_Time[0]  ); // IR FUN HERE
// 			//printf("Delay 2: %u  \n\r",Pulse_Time[1] ); // IR FUN HERE
// 			_Delay_1_Trigger = 0; 
// 		} 
// 	
	
		if (_Delay_2_Trigger){
// 			printf("IR-Signal Detected \n\r -------------------- \n\r"); 
// 			printf("Delay 1: %u  \n\r",Pulse_Time[0]  ); // IR FUN HERE
// 			printf("Delay 2: %u  \n\r",Pulse_Time[1] ); // IR FUN HERE
// 		
// 			printf("Clock_Delay_1: %u  \n\r",Pulse_Time[2] ); // IR FUN HERE
// 			printf("Clock_Delay_2: %u  \n\r",Pulse_Time[3] ); // IR FUN HERE
// 			printf("Clock_Delay_3: %u  \n\r",Pulse_Time[4] ); // IR FUN HERE	
// 			
// 			printf("Average Clock Delay: %u \n\r", ((Pulse_Time[2]+Pulse_Time[3]+Pulse_Time[4]) / 3) );
// 			
// 			
			_Delay_2_Trigger = 0;
			_counter_ = 0; 
			IR_Fun(); 
		}
	}
}


ISR(INT0_vect){
	_Delay_Time = TCNT1; 
	
	
	if (_counter_ < 3  && _Delay_1_Trigger){
		Pulse_Time[2+_counter_] = _Delay_Time; 
		_counter_++; 
		if (_counter_  == 3){
			_Delay_2_Trigger = 1;
			_Delay_1_Trigger = 0;
			_counter_ = 0; 
			} 

	}
	
		
	if (_Delay_Time > 100 && _Delay_Time < 500) 
		 Pulse_Time[0] = _Delay_Time;

	_half_time = ( Pulse_Time[0]/2);
	
	if (_Delay_Time <= (_half_time +20)  && _Delay_Time >= (_half_time-20) ){
		Pulse_Time[1] = _Delay_Time;
		_Delay_1_Trigger = 1; 
	}
	
	
	
	TCNT1 = 0;            //Reset the timer value every interrupt


}


void IR_Analyze(void){
	CLEARBIT(DDRB,PORTB5); 
	
	while(1){
		
		if(CHECKBIT(PINB,PINB5))
		{
			SETBIT(IR_PORT, IR_Output);
			_delay_ms(1); 
			CLEARBIT(IR_PORT, IR_Output);
			_delay_ms(1); 
		}
		
		
	}
	
}


void IR_Fun(void){
	
	cli();  // disable interrupts, no longer needed, reading input
	
	for(char _some_fuckign_counter = 0; _some_fuckign_counter < 120; _some_fuckign_counter++)
	{
		
		if (CHECKBIT(IR_PIN_PORT, IRpin_PIN ))
			Store_Pulse[_some_fuckign_counter] = 0x01; 
			
		else 
			Store_Pulse[_some_fuckign_counter] = 0x00; 
		//SETBIT(IR_PORT, IR_Output);
		IR_Delay(9);

	//	CLEARBIT(IR_PORT, IR_Output);
	}
	

	printf("IR-Signal Detected \n\r -------------------- \n\r");
	printf("Delay 1: %u  \n\r",Pulse_Time[0]  ); // IR FUN HERE
	printf("Delay 2: %u  \n\r",Pulse_Time[1] ); // IR FUN HERE

	printf("Clock_Delay_1: %u  \n\r",Pulse_Time[2] ); // IR FUN HERE
	printf("Clock_Delay_2: %u  \n\r",Pulse_Time[3] ); // IR FUN HERE
	printf("Clock_Delay_3: %u  \n\r",Pulse_Time[4] ); // IR FUN HERE

	printf("Average Clock Delay: %u \n\r", ((Pulse_Time[2]+Pulse_Time[3]+Pulse_Time[4]) / 3) );
				
	for(char _some_fuckign_counter = 0; _some_fuckign_counter < 120; _some_fuckign_counter++)
	{
	
		printf(" %d", Store_Pulse[_some_fuckign_counter]); 
	

	}
	
	// 64 pulses take input after the low is detected and is greater than 100
	
	// down 18950
	// up 8890
		
	// clock  1130 //
	
	// Read first signal Pulled Low 
	//while (!CHECKBIT(IRpin_PIN,IR_Input))
		
	// Pulled High Half low time 
	//while (!CHECKBIT(IRpin_PIN,IR_Input))
	
	// Get Clock Pulse 
	// Store Time for clock pulse
	
	// Store all proceeding data afterward// 
	
	sei();
	return; 
}


void IR_Delay(char delay){
	
	for (char _delay_count =0; _delay_count < delay; _delay_count++)
		_delay_us(64);
	
}