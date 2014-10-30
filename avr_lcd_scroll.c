/***********************************************
Sample 02
CSC 230 Fall 2014 Assignment 03
'C' code to output scrolling message to AVR microprocessor 
************************************************/

#include <string.h>
#include <stdlib.h>
#include "main.h"
#include "lcd_drv.h"

 
int main( void )
{
	lcd_init();
	int loopNum = 100;
	while(loopNum != 0)
	{
		int xVal = 0;
		while(xVal<16)
		{  		
  			lcd_xy( xVal, 0 );
  			lcd_puts( "Display by" );
			lcd_puts( "                 " );
			lcd_xy( xVal, 1 );
  			lcd_puts( "Jason Sanche" );
			_delay_ms( 250 );
			lcd_puts( "                 " );
			xVal++;
			
		}
		loopNum--;
	}
#ifdef LCD_LINE2
  //lcd_xy( 0, 1 );
  //lcd_puts( " " );
#ifdef LCD_LINE3
  lcd_xy( 0, 2 );
  lcd_puts( "Line 3" );
#ifdef LCD_LINE4
  lcd_xy( 0, 3 );
  lcd_puts( "Line 4" );
#endif
#endif  
#endif  
  for(;;){
  }
}
