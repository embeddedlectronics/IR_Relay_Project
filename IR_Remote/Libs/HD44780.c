/*
 * HD44780.c
 *
 * Created: 28/08/2012 9:54:54 PM
 *  Author: Michael
 *  This library works with all HD44780 LCD MODULES 
 *  Only been tested on ATMEGA 328p 
 */  
#define  F_CPU    16000000UL   /* This is important for the delay.h utility, this should be set to your clock frequency */ 
#include <util/delay.h>        /* Links my library to the utility library delay.h*/
#include <avr/io.h>          /* standard input and output library */
#define  LCD_DAT  PORTD    /* Port D drives LCD data pins, E, and RS */
#define  LCD_DIR  DDRD    /* Direction of LCD port */
#define  LCD_E_RS_DAT PORTB   /*  */
#define  LCD_E_RS_DIR DDRB    /*Direction of enable and rs*/

/*   Confirm the pin out of your LCD Shield or LCD Project board   
    This LCD Shield Uses The last 4 bits of portD for parallel processing 
 */

#define  LCD_E    0x02     /* E signal  0000 0010    E is connected to pinB1*/
#define  LCD_RS   0x01     /* RS signal 0000 0001    RS is connected to pin B0*/
#define  LCD_E_RS 0x07    /* assert both E and RS signal   */
#define	 Backlit  0x04         


// LCD MODULES void cmd2LCD(char cmd)
void cmd2LCD(char cmd)
{
	char temp;
	temp = cmd;           /* save a copy of the char  */
	cmd &=0xF0;           /* zero out the lower 4 bits */
	LCD_E_RS_DAT &= (~LCD_RS); /* select LCD instruction register */
	LCD_E_RS_DAT |= LCD_E;     /* pull E signal to high */
	//cmd >>= 2;            /* shift to match LCD data pins */
	LCD_DAT = cmd;/* output upper 4 bits, E, and RS = 0 */
	_delay_us(3);
	LCD_E_RS_DAT &= (~LCD_E);  /* pull E signal to low */
	cmd = temp & 0x0F;    /* extract the lower four bits */
	cmd <<= 4;
	LCD_E_RS_DAT |= LCD_E;     /* pull E to high */
	LCD_DAT = cmd;/* output upper 4 bits, E, and RS */
	_delay_us(3);
	LCD_E_RS_DAT &= (~LCD_E);  /* pull E clock to low */
	_delay_us(50);       /* wait until the command is complete */
}

void openLCD(void)
{
	LCD_DIR = 0xFF;       /* configure LCD_DAT port for output */
	LCD_E_RS_DIR = 0xFF;
	_delay_ms(10);
	cmd2LCD(0x28);        /* set 4-bit data, 2-line display, 5x7 font */
	cmd2LCD(0x0C);        /* turn on display, cursor, blinking */
	cmd2LCD(0x06);        /* move cursor right */
	cmd2LCD(0x01);        /* clear screen, move cursor to home */
	_delay_ms(2);        /* wait until "clear display" command is complete */
}


void putcLCD(char cx)
{
	char temp;
	temp = cx;
	LCD_E_RS_DAT |= (LCD_E_RS) ; /* select LCD data register */
	cx &= 0xF0;           /* clear the lower 4 bits */
	LCD_DAT = cx;/* output upper 4 bits, E, and RS */
	_delay_us(3);
	LCD_E_RS_DAT &= (~LCD_E);  /* pull E to low */
	cx = temp & 0x0F;     /* get the lower 4 bits */
	cx <<= 4;
	LCD_E_RS_DAT |= LCD_E;     /* pull E to high */
	LCD_DAT = cx;/* output upper 4 bits, E, and RS */
	_delay_us(3);
	LCD_E_RS_DAT &= (~LCD_E);  /* pull E to low */
	_delay_us(50);
}

void putsLCD (char *ptr)  /* breaks the string down to characters ie a pointer to an address that holds the string */ 
{
	while (*ptr) {     /* while char pointer is not null*/ 
		putcLCD(*ptr);  /* put the character on the screen on letter at a time */
 		ptr++;   /* increment the memory the pointer points to */ 
	}
}


void clearscreen(void)
{
	cmd2LCD(0x01);  // clears screen  0x08 blanks the display without clearing
	cposition(0,1);   // returns curser home 
}

void clearline(int line)      
{
	cposition(0,line);  
	for (int i=0; i < 20; i++) {putsLCD(" "); }
	cposition(0,line);
}

void cposition(int x, int y)
{
	int temp = 0x00 + x;
	
	switch(y)
	{
		case 1:
		temp += (0x80);      /// return home
		break;   
		case 2:
		temp += (0xC0);      /// Line 2 
		break;
		case 3:
		temp += (0x94);      /// line 3
		break;
		case 4:
		temp += (0xD4);      /// line 4 
		break;
	}
	cmd2LCD(temp);
}


/*  ************************************************************
Custom character function
	you will need to pass an array of 8 integers see my example code to accomplish this. 
	characters are defined by 8 sets bytes. Each Byte represents one Row(ie 1st byte is 1st row) the first 5 bytes represent the on and off
	states of the pixels.
	Please note that each time a character is defined the cursor returns home. 
*/
void CChar(int *character, int ccram)
{
	int x= 0; /// temp var for counting. 
	char command= (0x40+(ccram*8));  // adds the cc ram address 0x40 + the input for the cc (note: each cc takes 8 bytes) 
	cmd2LCD(command);  /// selects the first location of CG ram 
	
	while (x < 8)
	{
		putcLCD(character[x]);
		x++; 		
	}	
	cmd2LCD(0x80);   /// send curser back home
}



/***************************************************************
Code Useful Only for the DFROBOT LCD Screen - you need to use my analog to digital library 
ADT328p.h
	Each button has two possible outcomes depending on how long the button has been pressed. button must be pressed for 8ms to respond and >450ms for the 2nd responce.
	note you may change the values below, i kept them wide as the resistors are shit and misbehave (ie for Up: give 141 most times and 200 at others)
*/
int checkPress(void)
{
	int press=0, delaypress =0;
	int x;
	while (ReadADC(0) <= 1000)
	{
     	if (ReadADC(0) >= 100 && ReadADC(0) <= 220 &&  (delaypress >= 2) && (delaypress <= 449) ){	press = 1;	}
        else if (ReadADC(0) >= 260 && ReadADC(0) <= 375 &&  (delaypress >= 2) && (delaypress <= 449) ){	press = 3;	}
		else if (ReadADC(0) >= 440 && ReadADC(0) <= 550 &&  (delaypress >= 2) && (delaypress <= 449)  ){	press = 4; 	}
		else if (ReadADC(0) >= 0 && ReadADC(0) <= 50 &&  (delaypress >= 2) && (delaypress <= 449) )	{	press = 2; 	}	
		else if (ReadADC(0) >= 700 && ReadADC(0) <= 770 &&  (delaypress >= 2) && (delaypress <= 449) )  {	press =5;	}
		_delay_ms(1); 
		++delaypress; 	
		if (delaypress >= 450){press= (press +5); printf(" --  %d -- %d", delaypress, press);} // held for 250 ms or more return a different number 
	}
	
return press; 
}
  