



#include "IR_Sensor.h"
// its only necessary to compare the last 5 
int _IR_Remote_Buttons[4][9] ={	
	{ 0xd777, 0x5574, 0x2aea, 0x7757, 0xeeee, 0x5550, 0x3bae, 0x7777, 0x003a},  // UP 
	{ 0xd777, 0x5574, 0x2aea, 0x7757, 0xeeee, 0x5570, 0x3aba, 0x7777, 0x003a},// DOWN 
    { 0xd777, 0x5574, 0x2aea, 0x7757, 0xeeee, 0x55d0, 0x2eba, 0x7777, 0x003a}, // RIGHT
	{ 0xd777, 0x5574, 0x2aea, 0x7757, 0xeeee, 0x5770, 0x2aea, 0x7777, 0x003a} // LEFT 
};

void IR_Initalize(void){
	// Trigger Interrupts on any Logic change // - input for IR 
	ANY_LOGIC_CHANGE;
	EIMSK |= (1 << INT0);     // Turns on INT0
	sei();  // inable interrupts 

	//Configure timer
	TCCR1B |= (1<<CS10) | (1 << CS11) & ~(1<< CS12);  //1:64 prescaler 4us per Tick 
	
	_Delay_1_Trigger = 0;  // Get Clock 
	_Delay_2_Trigger = 0;  // Get Signal 
	_Clock_Pulse_Trigger = 0;  // Get CP Count

	_IR_Read_State =0;   
	_counter_ =0;
	_binary_counter_ = 0;
	_binary_array_counter_ = 0;
	_count_delay=0;
	_counter_ON =0;
	_counter_OFF =0;
	_clear =0;


	_IR_Clock_Pulses=0;
	_IR_Get_Pules =0;

	_Delay_Time =0;
	_half_time =0;

	delay =0 ;
}

char IR_Compare(int *_IR_Input, int * _Existing_Code){
	char analyze =0;
	int _IR_input_temp =0, _Existing_input_temp=0;
	
	for (char _count=0; _count < (((_IR_Clock_Pulses) /16) +1 ); _count++){
		
		for (char _count_16=0; _count_16 < 16; _count_16++)
		{
			_IR_input_temp = ( _IR_Input[_count]  );
			_Existing_input_temp = ( _Existing_Code[_count] );
			
			_IR_input_temp &=   (1 << _count_16);
			_Existing_input_temp &= (1 << _count_16);
			
			if (_IR_input_temp == _Existing_input_temp)
			analyze++;
			
		}
		
		if (analyze >= ((((_IR_Clock_Pulses) /16) +1 )*16)  -2)
			return 1;
		
			
// 		if(_IR_Input[_count] != _Existing_Code[_count] )
// 		{
// 			printf("\n\r  0x%x !=  0x%x  %d  ", _IR_Input[_count] ,_Existing_Code[_count], (((_IR_Clock_Pulses) /16)) );
// 		}

		//printf("\n\r %d", analyze);

	}
	
	return 0;
}





ISR(INT0_vect){
	// Store the Value of TCNT1 asap 
	_Delay_Time = TCNT1; 
	
	if (_Delay_2_Trigger){
		// only record the on states, if the signal is pulled low recored it!
		if (!CHECKBIT(IR_PIN_PORT, IRpin_PIN ) && _Delay_Time < 4000)
		{
			_count_delay=0;
			
			for ( ;_count_delay < _Delay_Time / delay; _count_delay++)
			{

 					_Store_Input_Signal[_binary_array_counter_] += (1 << (_count_delay + _binary_counter_) );
					_counter_++; 
				
					if(_count_delay + _binary_counter_ >= 15){
						_binary_array_counter_ ++;
						_binary_counter_ = 0;
					}
					
			}
			_binary_counter_ += (_Delay_Time / delay); 
		}
		
		else {
			_binary_counter_++; 
			_counter_++;
		}
		
		
		if (_binary_counter_ >= 15){
			_binary_array_counter_ ++; 
			_binary_counter_ = 0;
		}
			
		if (_counter_ >= (_IR_Clock_Pulses) && _Delay_2_Trigger && _IR_Get_Pules != 1)
		{
			_counter_ =0;
			_Delay_2_Trigger = 0;
			_IR_Read_State = 1; 
			
		}
		
		if (_IR_Get_Pules == 1 && _Delay_Time > 4000 ){
			_IR_Clock_Pulses = (_counter_  -1); 
			_Clock_Pulse_Trigger = 1; 
			_Delay_1_Trigger = 0;
			_Delay_2_Trigger = 0;
			_counter_ = 0;
			_binary_counter_ = 0;
			_binary_array_counter_ =0;
			_IR_Get_Pules= 0; 
			
		}
	}
	
	// Store the clock pulses // get the clock pulse value
	if (_counter_ < 3  && _Delay_1_Trigger){
		Pulse_Time[2+_counter_] = _Delay_Time;
		_counter_++;
		if (_counter_  == 3){
			_Delay_1_Trigger = 0;
			_Delay_2_Trigger = 1;
			_binary_array_counter_ = 0; 
		
			
			_counter_ = 0;
			delay =  ((Pulse_Time[2]+Pulse_Time[3]+Pulse_Time[4]) / 3 ) -15; 	
			
			for(_counter_= 0; _counter_< 16; _counter_++)
					_Store_Input_Signal[_counter_] = 0x0000;
						
		}
	}

	// IR Start signal must be between 7ms & 19ms anything less & issues will be had 
	if (_Delay_Time > 200 && _Delay_Time < 3000 && CHECKBIT(IR_PIN_PORT, IRpin_PIN ) ){
		_counter_ = 0; 
		Pulse_Time[0] = _Delay_Time;		
		_half_time = ( Pulse_Time[0]/2);
	}
		
	// Triggered only if current delay time = half previous delay 
	if (_Delay_Time <= (_half_time +20)  && _Delay_Time >= (_half_time-20) &&  !CHECKBIT(IR_PIN_PORT, IRpin_PIN ) ){
		Pulse_Time[1] = _Delay_Time;
		_Delay_1_Trigger = 1;
	}
	
	
	// If delay is larger than 19 ms reset all values 
	if (_Delay_Time > 3200){
		_Delay_1_Trigger = 0;
		_Delay_2_Trigger = 0;
		_counter_ = 0; 
		_binary_counter_ = 0;
		_binary_array_counter_ =0; 			
	}
	
	TCNT1 = 0;            //Reset the timer value every interrupt
}



void IR_Determine_Signal_Length(void){
//	printf("\n\r Clock pulses: %d ", _IR_Clock_Pulses);
	_Clock_Pulse_Trigger =0 ;
}


void IR_Fun(void){
// 	printf("IR-Signal Detected \n\r -------------------- \n\r");
// 	printf("Delay 1: %u  \n\r",Pulse_Time[0]  ); // IR FUN HERE
// 	printf("Delay 2: %u  \n\r",Pulse_Time[1] ); // IR FUN HERE
// 	
// 	
// 	printf("Clock 1: %u Clock 2: %u Clock 3: %u \n\r",Pulse_Time[2],Pulse_Time[3] ,Pulse_Time[4]  ); // Show Clock Timing here //
// 	printf("Rounded Time For Clock Used : %u  \n\r", delay ); // Round clock down as the pulses can vary 1-5% which can make a pulse of 3 '1's be only 2 '1's
// 	
// 	for(char _count=0; _count < (_IR_Clock_Pulses/16)+2; _count++)
// 	printf(" 0x%x  \n\r", _Store_Input_Signal[_count]);
// 
// 	
	for (char x = 0; x < 4; x++)
		if (IR_Compare(_Store_Input_Signal,  _IR_Remote_Buttons[x])){
			printf ("\n\n\n FOUND %d", x);
			
			_74HC164_SendData(_8_Segment_Active_Low[x]);

			
			if (x >= 2){
				CLEARBIT(_Relay__PORT, _Relay_1_Pin);
				CLEARBIT(_Relay__PORT, _Relay_2_Pin);
			}
			else{
				SETBIT(_Relay__PORT, _Relay_1_Pin);
				SETBIT(_Relay__PORT, _Relay_2_Pin);
			}
		}
	
	return;
}