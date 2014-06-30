/*
 * I2C.h
 *
 * Created: 3/10/2013 2:24:56 PM
 *  Author: Michael
 */ 

/// Clock = Analog 5 
/// Data = Analog 4 
#ifndef I2C_H_INCLUDED_
#define I2C_H_INCLUDED_

#define CPU_FREQ 16000000
#define TWI_FREQ 100000

#define START 		0X08   //start() has been successfully sent
#define RESTART 	0X10 //The restart() has been successfully sent
#define MT_SLA_ACK 	0X18 //Load data has been sent ack()
#define MT_SLA_NACK 0X20 //Load data has been sent nack()
#define MT_DAT_ACK 	0X28  //Data has been sent ack()
#define MT_DAT_NACK 0X30 //Data has been sent nack()
#define SL_SLA_ACK 	0X40 //Data has been loaded from the machine to send ack()
#define SL_SLA_NACK 0X48 //Data has been loaded from the machine to send nack()
#define SL_DAT_ACK 	0X50  //Slave data has been sent ack()
#define SL_DAT_NACK 0X58  //Slave data has been sent nack()

#define start() 	TWCR=((1<<TWINT)|(1<<TWSTA)|(1<<TWEN)) //Send start signal
#define restart() 	TWCR=((1<<TWINT)|(1<<TWEN)|(1<<TWSTA)) //Sent to re-start signal
#define Delay();    _delay_us(20);// 4.7 us delay min
#define wait()  	while(!(TWCR & (1<<TWINT))) //Waiting for the signal
#define state() 	(TWSR&0XF8) //The register TWSR state extraction
#define stop() 		TWCR=((1<<TWEN)|(1<<TWINT)|(1<<TWSTO)) //Stop signal
#define twi()		(TWCR=(1<<TWINT)|(1<<TWEN))	//Open the TWI function
#define setAck()	(TWCR|= (1<<TWEA))				//Make Not Ack response
#define setnoAck()	(TWCR&=~(1<<TWEA))				//Make Not Ack response

#define r_direction 0xD1 //Read the device address
#define w_direction 0xD0 //Write device address

////////////////////////////////////////////////////
///// I2C Code throw this into a library  
////////////////////////////////////////////////////

// IMPORTANT! Wire. must have a begin() before calling init()

void I2C_StartWrite(void); 
void I2C_SendByte(char byte); // send 8 bytes
void I2C_TerminateTransmission(void); //end the current transmission session
char I2C_ReadByte(void); // read only 8 bytes 
void I2C_Initalize(void); // initalize the I2C Functionality
int TWI_Read(int addr); //Read data from TWI






#endif /* I2C_H_ */