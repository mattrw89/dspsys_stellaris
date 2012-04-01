#include "uart.h"

void UART_init(void) {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	
	UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 9600, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
		
	UARTFIFOEnable(UART0_BASE);
	UARTFIFOLevelSet(UART0_BASE, UART_FIFO_TX1_8, UART_FIFO_RX4_8);
	UARTIntClear(UART0_BASE, 0xFFFFFFFF);

	IntEnable(INT_UART0);
	UARTIntEnable(UART0_BASE, UART_INT_RX);
	
	uart_transmit("AmpTraXX2!\n", 11);
}

uint8_t uart_transmit(char* formatted, uint8_t length) {
    //FOR TESTING PURPOSES ONLY
    uint8_t uart_counter = 0;
    
    while( uart_counter < 8 ) {
	    while( !UARTSpaceAvail(UART0_BASE)  ) {}
	    if (uart_counter == 0) {
	    	UARTCharPutNonBlocking(UART0_BASE, (char) 0xFF);
	    } else UARTCharPutNonBlocking(UART0_BASE, formatted[uart_counter-1]);
	    uart_counter++;
	}
    
    //Api_rx_all(formatted, vars);
    
    return 1;
}
