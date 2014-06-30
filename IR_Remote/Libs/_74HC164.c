/*
 * _74HC164.c
 *
 * Created: 4/23/2013 12:17:24 AM
 *  Author: Michael
*/ 
#include "_74HC164.h"


char _8_Segment_Active_Low[10] = {
0x01, //0
0xD7, //1
0x22, //2
0x12, //3
0x54, //4
0x18, //5
0x0C, //6
0x53, //7
0x00, //8 
0x10  //9
	};

void _74HC164_Initialize(void){
	_74HC164_DDR = (1 << _74HC164_Data) |(1 << _74HC164_Clock) | (1 << _74HC164_CS);
}

void _74HC164_SendData(char data){
	char datalength = 0; 
	_74HC164_CS_Low();
	_delay_ms(10); 
	_74HC164_CS_High(); 
	
	
	for (datalength = 0; datalength < 8 ; datalength++){
		if (data & 0x80)
		{
			_74HC164_Data_High();
		}
		else
		{
			_74HC164_Data_Low();
		}
		_delay_ms(1);
		_74HC164_Clock_Low();
		_delay_ms(1);
		_74HC164_Clock_High();
			
		data <<= 1;
	}
	_delay_ms(1); 
	
}

// Pull CS LOW to address the HT1632 chip  only good for the Sure display only goes as high as 4 
void _74HC164_ChipSelect(char chip){
	int total_clockcount= 0; 
	if (chip <= 0){
		_74HC164_Data_High();   // force low for all 4 clock cycles
		for(int x= 0; x < 4; x++){
			_74HC164_Clock_High();
			_74HC164_Clock_Low();
		}
	}		
		
	else {
		_74HC164_Data_High(); 
		// Send the initial clock pulses 
		for(int clockloop=4; clockloop >= 0; clockloop--){   /// pro tip change it to 8 and you can use more
			_74HC164_Clock_High();
			_74HC164_Clock_Low();
			_74HC164_Data_High();
			
			if ( clockloop == (chip))_74HC164_Data_Low();
			
			}			
		}
}


