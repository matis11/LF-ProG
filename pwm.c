#define F_CPU 1000000L
#include <avr/io.h>
#include <util/delay.h>

#include<timercounter0.h>
/*Includes timercounter0.h header file which defines different functions for the timer counter 0. TIMER COUNTER 0 header file version is 1.1*/

#include<timercounter2.h>
/*Includes timercounter2.h header file which defines different functions for the timer counter 0. TIMER COUNTER 2 header file version is 1.1*/


int main(void)
{
    DDRB=0xff;
	/* PB3 pin of PORTB is declared output (PWM1 pin of DC Motor Driver is connected) */

	DDRD=0xff;
	/* PD7 pin of PORTD is declared output (PWM2 pin of DC Motor Driver is connected) */

	DDRC=0x0f;
	/*PC0,PC1,PC2 and PC3 pins of PortC are declared output ( i/p1,i/p2,i/p3 and i/p4 pins of DC Motor Driver are connected)*/

	set_timercounter0_mode(3);
	/*Timer counter 0 is set to fast pwm mode*/

	set_timercounter0_prescaler(4);
	/*Timer counter 0 frequency is set to 3.90625KHz*/

	set_timercounter0_output_mode(2);
	/*Timer counter 0 output mode is set  to non-inverting mode*/

	set_timercounter2_mode(3);
	/*Timer counter 2 is set to fast pwm mode*/

	set_timercounter2_prescaler(4);
	/*Timer counter 2 frequency is set to 3.90625KHz*/

	set_timercounter2_output_mode(2);
	/*Timer counter 2 output mode is set to non-inverting mode*/
	
	/*Start of infinite loop*/
	while(1)
	{
		set_timercounter0_compare_value(255);
		/*OCR0 register value is set to 255*/
		
		set_timercounter2_compare_value(255);
		/*OCR2 register value is set to 255*/
	
		PORTC=0x0a;
		/*Robot will move forward in forward direction*/
	
		_delay_ms(2000);
		/*Robot moves in forward direction for 4s with medium speed*/

		set_timercounter0_compare_value(100);
		/*OCR2 register value is set to 100*/
		
		set_timercounter2_compare_value(100);
		/*OCR2 register value is set to 100*/
	
		_delay_ms(2000);
		/*Robot moves in forward direction for 4s with low speed*/
		
		set_timercounter0_compare_value(0);
		/*OCR2 register value is set to 100*/
		
		set_timercounter2_compare_value(0);
		/*OCR2 register value is set to 100*/
	
		_delay_ms(2000);
	}
  
}
