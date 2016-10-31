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
	unsigned int adc_value, buf_size=2048;
	unsigned char* buf;

	LEDDDR|= (1<<LEDDDRPIN); // enable pin as output
	ADCSRA = (1<<ADEN) | (1<<ADPS2) | (1<<ADPS0); // start ADC
	ADMUX = ADCPORT | (1<<REFS0);

	while ( ! (buf=malloc(buf_size)) ) buf_size -= 64;

	while (1) {
		cli();
		for(unsigned int i=0; i<buf_size; i++) {
			ADCSRA |= (1<<ADSC); // Start conversion
			while (ADCSRA & (1<<ADSC)); // wait for conversion to complete
			adc_value = ADCW & 0x3FF;
			buf[i] = (unsigned char) adc_value >> 2; // FIXME opt.
		}
		LEDPORT|= (1<<LEDOUT); // led on, pin=1
		uart0_init( UART_BAUD_SELECT(57600,F_CPU) );
		sei();
		uart0_puts( "\r\nstart_ADC_data\r\n" );
		for(unsigned int i=0; i<buf_size; i++)
			uart0_putc(buf[i]);
		uart0_puts( "\r\nend_ADC_data\r\n" );
		LEDPORT &= ~(1<<LEDOUT); // led off, pin=0
	}
	return 0; /* never reached */
}
