/*
 * I2C.h
 *
 * Created: 3/10/2013 2:24:56 PM
 *  Author: Michael
 */ 

/// Clock = Analog 5 
/// Data = Analog 4 
#include "I2C.h"
#define  F_CPU    16000000UL
#include <avr/io.h>
#include <util/delay.h>

////////////////////////////////////////////////////
///// I2C Code throw this into a library  
///// This so far is only good to write and not good 
///// for reading data from the lcd 
////////////////////////////////////////////////////


void I2C_StartWrite(void) 
{
	TWCR = (1<<TWINT) | (1<<TWSTA) | ( 1<< TWEN) | (1 << TWEA);  // write the START command
	while (! (TWCR & (1<<TWINT)));  // wait till condition has been sent
}

void I2C_SendByte(char byte)
{
	TWDR = byte;  // send the data to TWDR 
	TWCR = (1 << TWINT)| (1 << TWEN); // Clear TWINT bit and start transmission 
	while (! (TWCR & (1 << TWINT)));
}

void I2C_TerminateTransmission(void)
{
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO); // transmit stop command. 
	while(TWCR & (1 << TWSTO))	{ continue;}
}

char I2C_ReadByte(void) {
   TWCR = (1<<TWINT) | (1<<TWEN);
    
   while(!(TWCR & (1<<TWINT)));
    return TWDR;
}




void I2C_Initalize(void)
{
	DDRC |= (1 << PORTC4) | (1 << PORTC5); 
	PORTC |= (1 << PORTC4) | ( 1 << PORTC5);   // activate internal pullups for TWI
	/* SCL Frequency =   CPU CLOCK /  ( 16 + 2(TWBR) * (TWSR Value))   // see page 222
	  TWSR = prescaler value  // see page 243 for values */
	  
	TWSR = ( 0<< TWPS1) & (0 << TWPS0);  // set prescaler to 1
	TWBR = ((CPU_FREQ / TWI_FREQ) - 16) / 2;
	// enable twi module, acks, and twi interrupt
	TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWEA);
}

int TWI_Read(int addr) //Read data from TWI
{
   	char temp=0;
   	start();//
	wait();
	Delay();
	if(state()!=START) 
	return 1;			 		//ACK*/	   

	I2C_SendByte(w_direction);	//Write TWI slave address and write
	wait(); 
	Delay();
	if(state()!=MT_SLA_ACK) 
	return 1;				    //ACK*/

	I2C_SendByte(addr);		//Write the corresponding device register address
	wait();
	Delay();
	if(state()!=MT_DAT_ACK) 
	return 1;//*/

	start();	   				   	//TWI re-start
	wait();
	Delay();
	if (state()!=RESTART)  
	return 1;//*/

	I2C_SendByte(r_direction);	//Write TWI slave device address and read
	wait();
	Delay();
	if(state()!=SL_SLA_ACK)  
	return 1;				   //ACK*/

	twi();	 				   //Start the TWI read
	wait();
	Delay();
	if(state()!=SL_DAT_NACK) 
	return 1;					//ACK*/	

	temp=TWDR;//Read the TWI of the receive data
	stop();//
	Delay();
	return temp;
}