/***********************************************
Sample 02
CSC 230 Fall 2014 Assignment 03
'C' code to output scrolling message to LCD 
Display on AVR microprocessor 
************************************************/

/*
CSC 230 Fall 2014 Assignment 3 Part 8
Jason Sanche
V00349530
*/

#include <string.h>
#include <stdlib.h>
#include "main.h"
#include "lcd_drv.h"

 
int main( void )
{
	lcd_init();

	char LINE0[16] = " Display by     ";
	char LINE1[16] = " Jason Sanche   ";

	for(;;){
		for(int i = 0; i < 16; i++){
			for(int j = 0; j < 16; j++){
				lcd_xy((i+j)%16, 0);
				lcd_putchar(LINE0[j]);
				lcd_xy((i+j)%16, 1);
				lcd_putchar(LINE1[j]);
			}
			_delay_ms( 250 );	
		}
	}

}
