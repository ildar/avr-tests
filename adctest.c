#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdlib.h>

#define LEDOUT PORTB5 // "13" on Arduino boards
#define LEDPORT PORTB
#define LEDDDR DDRB
#define LEDDDRPIN DDB5

#define ADCPORT 3

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
	unsigned int adc_value;
	char buf[16];

	/* enable pin as output */
	LEDDDR|= (1<<LEDDDRPIN);
	ADCSRA = (1<<ADEN) | (1<<ADPS2) | (1<<ADPS0); // start ADC
	ADMUX = ADCPORT | (1<<REFS0);

	uart0_init( UART_BAUD_SELECT(57600,F_CPU) );
	sei();

	LEDPORT|= (1<<LEDOUT); // led on, pin=1
	uart0_puts( "Hello, UART user!\r\n" );
	uart0_puts( "--\r\n" );
	LEDPORT &= ~(1<<LEDOUT); // led off, pin=0

	while (1) {
		ADCSRA |= (1<<ADSC); // Start conversion
		while (ADCSRA & (1<<ADSC)); // wait for conversion to complete
		adc_value = ADCW & 0x3FF;
		LEDPORT|= (1<<LEDOUT); // led on, pin=1
		uart0_puts(utoa(adc_value, buf, 10));
		uart0_puts(" \r\n");
		LEDPORT &= ~(1<<LEDOUT); // led off, pin=0
		delay_ms(500);
	}
	return 0; /* never reached */
}
