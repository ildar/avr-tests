#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#define LEDOUT PORTB5
#define LEDPORT PORTB
#define LEDDDR DDRB
#define LEDDDRPIN DDB5

#include "uart.h"


void delay_ms(unsigned int xms)
{
        while(xms){
                _delay_ms(0.96);
                xms--;
        }
}

int main(void)
{
	char c;

	/* enable pin as output */
	LEDDDR|= (1<<LEDDDRPIN);

	uart0_init( UART_BAUD_SELECT(57600,F_CPU) );
	sei();

	LEDPORT|= (1<<LEDOUT); // led on, pin=1
	uart0_puts( "Hello, UART user!\r\n" );
	uart0_puts( "--\r\n" );
	LEDPORT &= ~(1<<LEDOUT); // led off, pin=0

	while ( !uart0_available() ) {
		LEDPORT|= (1<<LEDOUT); // led on, pin=1
		uart0_putc( '.' );
		LEDPORT &= ~(1<<LEDOUT); // led off, pin=0
		delay_ms(500);
	}
	
	for (;;)
		if ( uart0_available() ) {
			c = uart0_getc();
			LEDPORT|= (1<<LEDOUT); // led on, pin=1
			uart0_putc( '[' );
			uart0_putc( c );
			uart0_putc( ']' );
			LEDPORT &= ~(1<<LEDOUT); // led off, pin=0
		} else
			delay_ms(100);
	
	return 0; /* never reached */
}
