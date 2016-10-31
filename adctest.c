/* The app dumps ADC data to serial port at speed (see below at uart0_init)
   To save the data (1) set serial port to the speed above
   (2) cat /dev/ttyUSB0 > /tmp/ttyUSB0.out
*/

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdlib.h>

#define LEDOUT PORTB5 // "13" on Arduino boards
#define LEDPORT PORTB
#define LEDDDR DDRB
#define LEDDDRPIN DDB5

#define ADCPORT 3
#define READ_DELAY 3 /* in ms */


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
	char buf_size_s[8];

	LEDDDR|= (1<<LEDDDRPIN); // enable pin as output
	ADCSRA = (1<<ADEN) | (1<<ADPS2) | (1<<ADPS0); // start ADC
	ADMUX = ADCPORT | (1<<REFS0);

	while ( ! (buf=malloc(buf_size)) ) buf_size -= 64;
	utoa(buf_size, buf_size_s, 10);

	while (1) {
		cli();
		for(unsigned int i=0; i<buf_size; i++) {
			ADCSRA |= (1<<ADSC); // Start conversion
			while (ADCSRA & (1<<ADSC)); // wait for conversion to complete
			adc_value = ADCW & 0x3FF;
			buf[i] = (unsigned char) (adc_value >> 2);
			_delay_ms(READ_DELAY);
		}
		LEDPORT|= (1<<LEDOUT); // led on, pin=1
		uart0_init( UART_BAUD_SELECT(57600,F_CPU) );
		sei();
		uart0_puts( "\r\nstart_ADC_data\r\n" );
		for(unsigned int i=0; i<buf_size; i++)
			uart0_putc(buf[i]);
		uart0_puts( "\r\nend_ADC_data, size=" );
		uart0_puts( buf_size_s );
		uart0_puts( "\r\n" );
		delay_ms(40);
		LEDPORT &= ~(1<<LEDOUT); // led off, pin=0
	}
	return 0; /* never reached */
}
