



#include "IR_Sensor.h"
// its only necessary to compare the last 5
int _IR_Remote_Buttons[4][9] ={
	{ 0xd777, 0x5574, 0x2aea, 0x7757, 0xeeee, 0x5550, 0x3bae, 0x7777, 0x003a},  // UP
	{ 0xd777, 0x5574, 0x2aea, 0x7757, 0xeeee, 0x5570, 0x3aba, 0x7777, 0x003a},// DOWN
	{ 0xd777, 0x5574, 0x2aea, 0x7757, 0xeeee, 0x55d0, 0x2eba, 0x7777, 0x003a}, // RIGHT
	{ 0xd777, 0x5574, 0x2aea, 0x7757, 0xeeee, 0x5770, 0x2aea, 0x7777, 0x003a} // LEFT
};

void IR_Initalize(void){
	struct _IR_Signal *_IR_PointProperties= &Properties;
	// Trigger Interrupts on any Logic change // - input for IR
	ANY_LOGIC_CHANGE;
	EIMSK |= (1 << INT0);     // Turns on INT0
	sei();  // enable interrupts

	//Configure timer
	TCCR1B |= (1<<CS10) | (1 << CS11) & ~(1<< CS12);  //1:64 prescaler 4us per Tick
	
	_IR_PointProperties->_IR_Status_Trigger = 0;  // Get Clock

	_IR_PointProperties->_IR_Read_State =0;
	_IR_PointProperties->_IR_Counter_Variable =0;
	_IR_PointProperties->_IR_Array_Binary_Counter_ = 0;
	_IR_PointProperties->_IR_Array_Counter = 0;
	_IR_PointProperties->_IR_Count_Delay=0;


	_IR_PointProperties->_IR_Clock_Pulses=0;
	_IR_PointProperties->_IR_Get_Pulses =1;

	_IR_PointProperties->_Delay_Time =0;
	_IR_PointProperties->_IR_Half_Pulse =0;

	_IR_PointProperties->_Delay_Variable =0 ;
}

char IR_Compare(int *_IR_Input, int * _Existing_Code){
	struct _IR_Signal *_IR_PointProperties= &Properties;
	char analyze =0;
	int _IR_input_temp =0, _Existing_input_temp=0;
	
	for (char _count=0; _count < (((_IR_PointProperties->_IR_Clock_Pulses) /16) +1 ); _count++){
		
		for (char _count_16=0; _count_16 < 16; _count_16++)
		{
			_IR_input_temp = ( _IR_Input[_count]  );
			_Existing_input_temp = ( _Existing_Code[_count] );
			
			_IR_input_temp &=   (1 << _count_16);
			_Existing_input_temp &= (1 << _count_16);
			
			if (_IR_input_temp == _Existing_input_temp)
			analyze++;
			
		}
		
		if (analyze >= ((((_IR_PointProperties->_IR_Clock_Pulses) /16) +1 )*16)  -2)
		return 1;


	}
	
	return 0;
}





ISR(INT0_vect){
	struct _IR_Signal *_IR_PointProperties= &Properties;
	// Store the Value of TCNT1 asap
	_IR_PointProperties->_Delay_Time = TCNT1;
	
	if (_IR_PointProperties->_IR_Status_Trigger == 0x02){
		
		// only record the on states, if the signal is pulled low recored it!
		if (!CHECKBIT(IR_PIN_PORT, IRpin_PIN ) && _IR_PointProperties->_Delay_Time < 4000)
		{
			_IR_PointProperties->_IR_Count_Delay=0;
			
			// Store the Signal in a series of Ints 
			for ( ;_IR_PointProperties->_IR_Count_Delay < _IR_PointProperties->_Delay_Time / _IR_PointProperties->_Delay_Variable; _IR_PointProperties->_IR_Count_Delay++)
			{
				// add 1 shifted by the counter var and the delay 
				_IR_PointProperties->_IR_Store_Input_Signal[_IR_PointProperties->_IR_Array_Counter] += (1 << (_IR_PointProperties->_IR_Count_Delay + _IR_PointProperties->_IR_Array_Binary_Counter_) );
				_IR_PointProperties->_IR_Counter_Variable++; // used for counting clock pulses 
				
				// split into int if the delay + the counter  > 16 
				if(_IR_PointProperties->_IR_Count_Delay + _IR_PointProperties->_IR_Array_Binary_Counter_ >= 15){
					_IR_PointProperties->_IR_Array_Counter++;
					_IR_PointProperties->_IR_Array_Binary_Counter_ = 0;
				}
				
			}
			_IR_PointProperties->_IR_Array_Binary_Counter_ += (_IR_PointProperties->_Delay_Time / _IR_PointProperties->_Delay_Variable);
		}
		
		else {
			_IR_PointProperties->_IR_Array_Binary_Counter_++;
			_IR_PointProperties->_IR_Counter_Variable++;
		}
		
		
		if (_IR_PointProperties->_IR_Array_Binary_Counter_ >= 15){
			_IR_PointProperties->_IR_Array_Counter ++;
			_IR_PointProperties->_IR_Array_Binary_Counter_ = 0;
		}
		
		// if counted clock pulses > Stored pulses This signifies the end of the signal, terminate the signal
		if (_IR_PointProperties->_IR_Counter_Variable >= (_IR_PointProperties->_IR_Clock_Pulses) && _IR_PointProperties->_IR_Status_Trigger == 0x02 && _IR_PointProperties->_IR_Get_Pulses != 1)
		{
			_IR_PointProperties->_IR_Counter_Variable =0;
			_IR_PointProperties->_IR_Status_Trigger = 0;
			_IR_PointProperties->_IR_Read_State = 1;
			
		}
		
		// Gets the pulses stores it in CLOCK PULSES // - if the delay is larger than 3000* 0.0065ms / 19.5m terminate / some remotes send several additional pulses 25- 40ms after the initial signal
		// My library discards these
		if (_IR_PointProperties->_IR_Get_Pulses == 1 && _IR_PointProperties->_Delay_Time > 3000 ){
			_IR_PointProperties->_IR_Clock_Pulses = (_IR_PointProperties->_IR_Counter_Variable  -1);
			_IR_PointProperties->_IR_Status_Trigger = 0;
			_IR_PointProperties->_IR_Counter_Variable = 0;
			_IR_PointProperties->_IR_Array_Binary_Counter_ = 0;
			_IR_PointProperties->_IR_Array_Counter =0;
			_IR_PointProperties->_IR_Get_Pulses= 0; 
			_IR_PointProperties->_IR_Read_State = 1;
		}
	}
	
	// Store the clock pulses // get the clock pulse value
	if (_IR_PointProperties->_IR_Counter_Variable < 3  && _IR_PointProperties->_IR_Status_Trigger == 0x01){
		_IR_PointProperties->_IR_Pulse_Time[2+_IR_PointProperties->_IR_Counter_Variable] = _IR_PointProperties->_Delay_Time;
		_IR_PointProperties->_IR_Counter_Variable++;
		if (_IR_PointProperties->_IR_Counter_Variable  == 3){
			_IR_PointProperties->_IR_Status_Trigger = 0x02;
			_IR_PointProperties->_IR_Array_Counter = 0;
			
			
			_IR_PointProperties->_IR_Counter_Variable = 0;
			_IR_PointProperties->_Delay_Variable =  ((_IR_PointProperties->_IR_Pulse_Time[2]+_IR_PointProperties->_IR_Pulse_Time[3]+_IR_PointProperties->_IR_Pulse_Time[4]) / 3 ) -IR_Error_Tollerance_;
			
			for(_IR_PointProperties->_IR_Counter_Variable= 0; _IR_PointProperties->_IR_Counter_Variable< 16; _IR_PointProperties->_IR_Counter_Variable++)
			_IR_PointProperties->_IR_Store_Input_Signal[_IR_PointProperties->_IR_Counter_Variable] = 0x0000;
			
		}
	}

	// IR Start signal must be between 200 * 0x0065 = 1.3ms & 19.5ms anything less or more will be ignored // please configure manually 
	if (_IR_PointProperties->_Delay_Time > IR_Initial_Low_Pulse_Minimum && _IR_PointProperties->_Delay_Time < IR_Initial_Low_Pulse_Maximum && CHECKBIT(IR_PIN_PORT, IRpin_PIN ) ){
		_IR_PointProperties->_IR_Counter_Variable = 0;
		_IR_PointProperties->_IR_Pulse_Time[0] = _IR_PointProperties->_Delay_Time;
		_IR_PointProperties->_IR_Half_Pulse = ( _IR_PointProperties->_IR_Pulse_Time[0]/2);
	}
	
	// Triggered only if current delay time = half previous delay
	if (_IR_PointProperties->_Delay_Time <= (_IR_PointProperties->_IR_Half_Pulse +IR_Error_Tollerance_)  && _IR_PointProperties->_Delay_Time >= (_IR_PointProperties->_IR_Half_Pulse-IR_Error_Tollerance_) &&  !CHECKBIT(IR_PIN_PORT, IRpin_PIN ) ){
		_IR_PointProperties->_IR_Pulse_Time[1] = _IR_PointProperties->_Delay_Time;
		_IR_PointProperties->_IR_Status_Trigger = 0x01;
	}
	
	
	// If delay is larger than 19 ms reset all values
	if (_IR_PointProperties->_Delay_Time > IR_Initial_Low_Pulse_Maximum){
		_IR_PointProperties->_IR_Status_Trigger = 0;
		_IR_PointProperties->_IR_Counter_Variable = 0;
		_IR_PointProperties->_IR_Array_Binary_Counter_ = 0;
		_IR_PointProperties->_IR_Array_Counter =0;
	}
	
	TCNT1 = 0;            //Reset the timer value every interrupt
}



