// TRANSMITTER 

#include <xc.h>

#pragma config FOSC = XT
#pragma config WDTE = OFF
#pragma config PWRTE = ON
#pragma config BOREN = ON
#pragma config LVP = OFF
#pragma config CPD = OFF
#pragma config WRT = OFF
#pragma config CP = OFF

unsigned char segmentValue = 0x00;
unsigned char ledValue = 0x00;

void delay(int time)
{
	int i, j;
	for (i = 0; i < time; i++)
	{
		for (j = 0; j < 100; j++);
	}
}

void usart_tx_init (void){
	// transmitter config
 	SPBRG = 0x19; // 9.6K baud rate @ FOSC=4MHz, asynchronous high speed
 	SYNC = 0; // asynchronous mode (TXSTA reg)
 	SPEN = 1; // enable serial port (RCSTA reg)
 	TX9 = 0; // 8-bit transmission (TXSTA reg)
 	BRGH = 1; // asynchronous high-speed (TXSTA reg)
 			      // (see formula in Table 10-1 data sheet)
 	TXEN = 1; // transmit enable (TXSTA reg)
}

void usart_rx_init (void){
	// receiver config
	RX9 = 0; // 8-bit reception (TXSTA reg)
	CREN = 1; // enable continuous receive (RCSTA reg)
}

void interrupt_init (void) {
	INTE = 1; // normal int enabled
	INTF = 0; // int flag initially cleared

    PEIE = 1; // peripherals int enabled
    GIE = 1; // global int enabled
}

void interrupt ISR () {
    GIE = 0;

	if (INTF) {
		INTF = 0; // clear via software

		// send switch data
		while(!TRMT);
		ledValue = (PORTB & 0xF0) >> 4;
		TXREG = ledValue;
	}

    GIE = 1;
}

 char combineNibbles (unsigned char uNibble, unsigned char lNibble) {
	return (uNibble & 0xF0) | lNibble;
}

void main (void) {
	// PORTs B
    TRISB = 0xF1; // button and switch inputs

	// PORTs C
	TRISC6 = 0;
	TRISC7 = 1;

	// PORTs D
	TRISD = 0x00; // output
	PORTD = 0;

	// usart and int configs
	usart_tx_init();
	usart_rx_init();
	interrupt_init();

    while (1) {
		// delay (200);

		while(!RCIF);
		segmentValue = RCREG;

		PORTD = combineNibbles(ledValue << 4, segmentValue & 0x0F);
    }
}
