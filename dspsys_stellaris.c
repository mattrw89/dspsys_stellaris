#include <string.h>
#include <cfloat>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_nvic.h"
#include "inc/hw_types.h"
#include "inc/hw_sysctl.h"
#include "driverlib/ethernet.h"
#include "driverlib/flash.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"
#include "utils/locator.h"
#include "utils/lwiplib.h"
#include "utils/uartstdio.h"
#include "utils/ustdlib.h"
#include "httpserver_raw/httpd.h"
#include "driverlib/qei.h"
#include "lib/hashmap/hashmap.h"
#include "lib/json/json.h"
#include "global.h"
#include "lib/dspsys_lib_channel/channel.h"
#include "lib/dspsys_lib_channel/common.h"
#include "lib/menu/menu.h"
#include "lib/i2c/lib_I2C.h"
#include "lib/lib_newhaven_screen/screen.h"
#include "ints.h"
#include "lib/dspsys_lib_txrxspi/ApiCommand.h"
#include "lib/dspsys_lib_txrxspi/ApiHandler.h"
#include "lib/lib_codec_cs42448/codec.h"
#include "lib/circbuff/circbuff.h"
#include "lib/uart/uart.h"
#include "lib/qei/quad.h"

//define the UART port that I'm using 
#define UART_PORT UART0_BASE
#define UART_INT INT_UART0


//*****************************************************************************
//
// Defines for setting up the system clock.
//
//*****************************************************************************
#define SYSTICKHZ               100
#define SYSTICKMS               (1000 / SYSTICKHZ)
#define SYSTICKUS               (1000000 / SYSTICKHZ)
#define SYSTICKNS               (1000000000 / SYSTICKHZ)

//*****************************************************************************
//
// Interrupt priority definitions.  The top 3 bits of these values are
// significant with lower values indicating higher priority interrupts.
//
//*****************************************************************************
#define SYSTICK_INT_PRIORITY    0x80
#define ETHERNET_INT_PRIORITY   0xC0

//*****************************************************************************
//
// External Application references.
//
//*****************************************************************************
extern void fs_init(void);
extern void fs_tick(unsigned long ulTickMS);

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, unsigned long ulLine)
{
}
#endif


//*****************************************************************************
//
// Display an lwIP type IP Address.
//
//*****************************************************************************
void
DisplayIPAddress(unsigned long ipaddr, unsigned long ulCol,
                 unsigned long ulRow)
{
    char pucBuf[16];
    unsigned char *pucTemp = (unsigned char *)&ipaddr;

    //
    // Convert the IP Address into a string.
    //
    usprintf(pucBuf, "%d.%d.%d.%d", pucTemp[0], pucTemp[1], pucTemp[2],
             pucTemp[3]);
    //
    // Display the string.
    //
    //RIT128x96x4StringDraw(pucBuf, ulCol, ulRow, 15);
}

//*****************************************************************************
//
// Required by lwIP library to support any host-related timer functions.
//
//*****************************************************************************

void
ConvertAddress2Char(unsigned long ipaddr, char* pucBuf )
{
    unsigned char *pucTemp = (unsigned char *)&ipaddr;

    //
    // Convert the IP Address into a string.
    //
    usprintf(pucBuf, "%d.%d.%d.%d", pucTemp[0], pucTemp[1], pucTemp[2],
             pucTemp[3]);

}

void address_set(unsigned long addr,int type) {
	char char1[16];
	Address* address = global_get_address();
	switch(type) {
		case 0: {
			ConvertAddress2Char(addr,char1);
			strncpy(address->ip,char1,sizeof(char1));
			break;
		}
		case 1: {
			ConvertAddress2Char(addr,char1);
			strncpy(address->subnet,char1,sizeof(char1));
			break;
		}
		case 2: {
			ConvertAddress2Char(addr,char1);
			strncpy(address->gateway,char1,sizeof(char1));
			break;
		}
	}
	fix_address(address,type);
}

//-----------------------------------------------------------------------------------------
void
lwIPHostTimerHandler(void)
{
    static unsigned long ulLastIPAddress = 0;
    static unsigned long ulLastSMAddress = 0;
    static unsigned long ulLastGWAddress = 0;
    unsigned long ulIPAddress;
    unsigned long ulSMAddress;
    unsigned long ulGWAddress;

    ulIPAddress = lwIPLocalIPAddrGet();
    ulSMAddress = lwIPLocalNetMaskGet();
    ulGWAddress = lwIPLocalGWAddrGet();
    

    //
    // If IP Address has not yet been assigned, update the display accordingly
    //
    if(ulIPAddress == 0 || ulSMAddress == 0 || ulGWAddress == 0)
    {
////        static int iColumn = 6;
////
////        
////         Update status bar on the display.
////        
////        RIT128x96x4Enable(1000000);
////        if(iColumn < 12)
////        {
////            RIT128x96x4StringDraw("< ", 0, 24, 15);
////            RIT128x96x4StringDraw("*",iColumn, 24, 7);
////        }
////        else
////        {
////            RIT128x96x4StringDraw(" *",iColumn - 6, 24, 7);
////        }
////
////        iColumn++;
////        if(iColumn > 114)
////        {
////            iColumn = 6;
////            RIT128x96x4StringDraw(" >", 114, 24, 15);
////        }
////        RIT128x96x4Disable();
    }

    //
    // Check if IP address has changed, and display if it has.
    //
    else if( (ulLastIPAddress != ulIPAddress) || (ulLastSMAddress != ulSMAddress) || (ulLastGWAddress != ulGWAddress) )
    {
        ulLastIPAddress = ulIPAddress;
        address_set(ulIPAddress, 0);
        
        ulLastSMAddress = ulSMAddress;
        address_set(ulSMAddress, 1);
        
        ulLastGWAddress = ulGWAddress;
        address_set(ulGWAddress, 2);
		
    }
}

//*****************************************************************************
//
// The interrupt handler for the SysTick interrupt.
//
//*****************************************************************************
void SysTickIntHandler(void) {
    // Call the lwIP timer handler.
    lwIPTimer(SYSTICKMS);

    // Run the file system tick handler.
    fs_tick(SYSTICKMS);
}


void channels_init() {
	uint8_t i=0;

	char tempi[10] = "Input ";
	char tempo[10] = "Output ";
	char temp[10];
	//loop through all input channels.  should be MAX of input and output channels
	for(i=0; i < NUM_INPUT_CHANNELS; i++) {

		strncpy(temp,tempi,6);
		temp[6] = i + 1 + 48;
		temp[7] = '\0';
		Channel_ctor(get_channel_from_memory(INPUT,i+1),i+1, temp, ACTIVE, INPUT);
	
		strncpy(temp,tempo,7);
		temp[7] = i + 1 + 48;
		temp[8] = '\0';
		Channel_ctor(get_channel_from_memory(OUTPUT,i+1),i+1, temp, ACTIVE, OUTPUT);
	}
}

void menu_init(Display *one, Display *two, Display *three, Display *four, Display *five){
	
	two->menu_type = HOME;
	
	two->left = one;
	two->right = three;
	two->select = five;
	two->back = four;
	
	one->i = 0;
	two->i = 0;
	three->i = 0;
	four->i = 0;
	five->i = 0;
	
	one->func_right = &s1_right;
	two->func_right = &s1_right;
	three->func_right = &s1_right;
	four->func_right = &s1_right;
	five->func_right = &s1_right;
	
	one->func_left = &s1_left;
	two->func_left = &s1_left;
	three->func_left = &s1_left;
	four->func_left = &s1_left;
	five->func_left = &s1_left;
	
	one->func_select = &s1_select;
	two->func_select = &s1_select;
	three->func_select = &s1_select;
	four->func_select = &s1_select;
	five->func_select = &s1_select;
	
	one->func_back = &s1_back;
	two->func_back = &s1_back;
	three->func_back = &s1_back;
	four->func_back = &s1_back;
	five->func_back = &s1_back;
	
	display_set_text(five, IP_TEXT, 16);
	display_ctor(five, IP_ADDRESS, NULL, NULL, NULL, NULL, NULL);
	
	display_set_text(two, HOME_TEXT, 16);
	display_set_text(three, HOME_TEXT, 16);
	display_set_text(one,HOME_TEXT,16);
	display_set_text(four,HOME_TEXT,16);
}


int8_t gpio_init(void) {
	
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
	GPIODirModeSet(GPIO_PORTC_BASE, GPIO_PIN_5 | GPIO_PIN_4 | GPIO_PIN_6 | GPIO_PIN_7, GPIO_DIR_MODE_IN);
	GPIOPadConfigSet(GPIO_PORTC_BASE, GPIO_PIN_5 | GPIO_PIN_4 | GPIO_PIN_6 | GPIO_PIN_7, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);
	GPIOIntTypeSet(GPIO_PORTC_BASE, GPIO_PIN_5 | GPIO_PIN_7 | GPIO_PIN_6 | GPIO_PIN_4, GPIO_LOW_LEVEL);
	GPIOPinIntEnable(GPIO_PORTC_BASE, GPIO_PIN_5| GPIO_PIN_4 | GPIO_PIN_6 | GPIO_PIN_7);
	IntEnable(INT_GPIOC);
	IntMasterEnable();
	GPIOPinIntClear(GPIO_PORTC_BASE, GPIO_PIN_5| GPIO_PIN_4 | GPIO_PIN_6 | GPIO_PIN_7);
	
	return 1;	
}

void UART_isr(void) {
	unsigned long int_status = UARTIntStatus(UART0_BASE, 1);
	char returned_value[17];
	unsigned long tempchar;
	int8_t uart_counter = 0;
	
	UARTIntClear(UART0_BASE, UART_INT_RX);
	
	if( int_status & UART_INT_RX ) {
		UARTCharPut(UART0_BASE, 'R');
		while ( UARTCharsAvail(UART0_BASE) == 1) {
    		if ( (tempchar = UARTCharGetNonBlocking(UART0_BASE)) > 0 ) {
    			returned_value[uart_counter] = (char) tempchar;
    			uart_counter++;
    		}
		}
		
		if(returned_value[0] == 0xFF) Api_rx_all(&(returned_value[1]));
		 
	} else if( int_status & UART_INT_TX ) {
		UARTCharPut(UART0_BASE, 'T');
	}

	while((UARTIntStatus(UART0_BASE, 0) & UART_INT_RX) ) {
		delaymycode(1);	
	}		
}

void TIMER0_isr(void) {
    //look to see if there are any objects in the circular ring buffer
	//if so, transmit them if there is room in the buffer
	//if not, exit the isr  
	
	//get the global ApiHandler
    struct ApiHandlerVars *handler = global_api_handler(0);
	CircularBuffer* cBuf = &(handler->tx_buffer);
	
	//if buffer has elements then transmit them and add them to transmitted stack
	while( !cbIsEmpty(cBuf) ) {
		ElemType elem;
		cbRead(cBuf, &elem);
		Api_tx_all(elem.value);
		
		//can do this loose cast because super is always the first 
		//element of ApiRead/ApiWrite/ApiAck/ApiNot
		//ApiCmd* api_cmd = (ApiCmd*) elem.value;
		
		//Api_tx_stack_append(&(handler->head), elem.value, api_cmd->cmd_count);
	}
    
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    
    while((TimerIntStatus(TIMER0_BASE, 1) & TIMER_TIMA_TIMEOUT) ) {
		delaymycode(1);	
	}	
}


void uart_tx_timer_init(void) {
 	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	TimerConfigure(TIMER0_BASE, TIMER_CFG_32_BIT_PER);
	TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet()/20);

	IntEnable(INT_TIMER0A);
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	TimerEnable(TIMER0_BASE, TIMER_A);
}



void notificationCallback(void* api_ptr) {
	ApiNot* notif = (ApiNot*)api_ptr;
	
	printf("notified!  message = %d", notif->message);	
}

void encoder_init(void) {
	Encoder_State* encoder = global_get_encoder();
	encoder->encoderA_last = GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_4);	
	encoder->encoderB_last = GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_6);
}

void screen_init(void) {
	screen_on();
	screen_clear();
	screen_underline_off();
	screen_blink_cursor_off();
}


void volume_init(void) {
	char char1[7] = "-63";
	Volume* vol = global_get_volume();
	strncpy(vol->left_level,char1,sizeof(char1));
	strncpy(vol->right_level,char1,sizeof(char1));
	vol->l_level = -63;
	vol->r_level = -63;
}


int main(void) {

    unsigned long ulUser0, ulUser1;
    unsigned char pucMACArray[8];

	if(REVISION_IS_A2) SysCtlLDOSet(SYSCTL_LDO_2_75V);

    // Set the clocking to run directly from the PLL at 50MHz.
    SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_8MHZ);

    //initialize the handler variables
    struct ApiHandlerVars handler_vars;
    struct ApiCmdNode* txed_stack = malloc(sizeof(struct ApiCmdNode));
    struct ApiCmdNode* tx_buffer = malloc(sizeof(struct ApiCmdNode));
    
    
    Api_register_notif_callback(&handler_vars, notificationCallback);
    
    Api_init_handler_vars(&handler_vars, txed_stack);
    txed_stack->next = NULL;
    txed_stack->cmd_count = 0;
    
    cbInit(&(handler_vars.tx_buffer), 20);
    
    
    global_api_handler(&handler_vars);
    handler_vars.cmd_counter = 17;
    
    IntMasterEnable();
    UART_init();
	uart_tx_timer_init();

    ApiRead  r1;//, r2;
    //ApiWrite w1, w2;
    //ApiAck   a1, a2;
    ApiNot n1;//, n2;

    //TEST API READ CONSTRUCTOR AND RE-CONSTRUCTOR
    ApiRead_ctor(&r1, 1, OUTPUT, COMP, THRESHOLD);
    ApiNot_ctor(&n1, 2, OUTPUT, COMP, 1);
    
    //let's transmit the read_1 api command
    //have it call callbackFunction upon reception of the corresponding value
    
    ElemType elem;
    elem.value = &(r1.super);
    cbWrite(&(handler_vars.tx_buffer), &elem);
    Api_set_callback(&(r1.super), callbackFunction);
    
    elem.value = &(n1.super);
    cbWrite(&(handler_vars.tx_buffer), &elem);
    
//    while(!cbIsEmpty(&(handler_vars.tx_buffer))) {
//    	cbRead(&(handler_vars.tx_buffer), &elem);
//   	Api_tx_all(object.value, &handler_vars);
//    }
    
    

//    long returned_value[100];
//    uint8_t uart_counter = 0;
//    long tempchar;
    
//    while(1) {
//    	uart_counter = 0;
//    	while( uart_counter < 8) {
//    		if ( UARTCharsAvail(UART0_BASE) == 1) {
//	    		if ( (tempchar = UARTCharGetNonBlocking(UART0_BASE)) > 0 ) {
//	    			returned_value[uart_counter] = (char) tempchar;
//	    			uart_counter++;
//	    		} else delaymycode(1);
//    		}
//	    }
//	    
//	    if (uart_counter > 0) {
//		    while( (uart_counter > 0) ) {
//		    	//printf("writing:  %c\n", (char)returned_value[uart_counter-1]);
//		    	while( !UARTSpaceAvail(UART0_BASE) ) {}
//		    		UARTCharPutNonBlocking(UART0_BASE, (char)returned_value[uart_counter-1]);
//		    	uart_counter--;
//		    }
//	    }
//	delaymycode(1);
//    }

    // Enable and Reset the Ethernet Controller.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ETH);
    SysCtlPeripheralReset(SYSCTL_PERIPH_ETH);

    // Enable Port F for Ethernet LEDs.
    //  LED0        Bit 3   Output
    //  LED1        Bit 2   Output
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIOPinTypeEthernetLED(GPIO_PORTF_BASE, GPIO_PIN_2 | GPIO_PIN_3);

    // Configure SysTick for a periodic interrupt.
    SysTickPeriodSet(SysCtlClockGet() / SYSTICKHZ);
    SysTickEnable();
    SysTickIntEnable();

	//configure the io (rotary encoder + buttons)
	//gpio_init();

    // Enable processor interrupts.
    IntMasterEnable();

    // Initialize the file system.
    fs_init();

    //
    // Configure the hardware MAC address for Ethernet Controller filtering of
    // incoming packets.
    //
    // For the LM3S6965 Evaluation Kit, the MAC address will be stored in the
    // non-volatile USER0 and USER1 registers.  These registers can be read
    // using the FlashUserGet function, as illustrated below.
    //
    FlashUserGet(&ulUser0, &ulUser1);
    if((ulUser0 == 0xffffffff) || (ulUser1 == 0xffffffff))
    {
        //
        // We should never get here.  This is an error if the MAC address has
        // not been programmed into the device.  Exit the program.
        //
       // RIT128x96x4Enable(1000000);
        //RIT128x96x4StringDraw("MAC Address", 0, 16, 15);
        //RIT128x96x4StringDraw("Not Programmed!", 0, 24, 15);
        while(1)
        {
        }
    }

    //
    // Convert the 24/24 split MAC address from NV ram into a 32/16 split MAC
    // address needed to program the hardware registers, then program the MAC
    // address into the Ethernet Controller registers.
    //
    pucMACArray[0] = ((ulUser0 >>  0) & 0xff);
    pucMACArray[1] = ((ulUser0 >>  8) & 0xff);
    pucMACArray[2] = ((ulUser0 >> 16) & 0xff);
    pucMACArray[3] = ((ulUser1 >>  0) & 0xff);
    pucMACArray[4] = ((ulUser1 >>  8) & 0xff);
    pucMACArray[5] = ((ulUser1 >> 16) & 0xff);

    // Initialze the lwIP library, using DHCP.
    lwIPInit(pucMACArray, 0, 0, 0, IPADDR_USE_DHCP);
	//netif_set_addr();
    // Setup the device locator service.
    LocatorInit();
    LocatorMACAddrSet(pucMACArray);
    LocatorAppTitleSet("AmpTraXX2");

    // Initialize a sample httpd server.
    httpd_init();
    

    // Set the interrupt priorities.  We set the SysTick interrupt to a higher
    // priority than the Ethernet interrupt to ensure that the file system
    // tick is processed if SysTick occurs while the Ethernet handler is being
    // processed.  This is very likely since all the TCP/IP and HTTP work is
    // done in the context of the Ethernet interrupt.
    IntPriorityGroupingSet(4);
    IntPrioritySet(INT_ETH, ETHERNET_INT_PRIORITY);
    IntPrioritySet(FAULT_SYSTICK, SYSTICK_INT_PRIORITY);
        
	channels_init();
	I2CInit();
	screen_init();
	volume_init();
	gpio_init();
	encoder_init();
	
	Display* one = global_get_display(0);
	Display* two = global_get_display(1);
	Display* three = global_get_display(2);
	Display* four = global_get_display(3);
	Display* five = global_get_display(4);	
	
	menu_init(one,two,three,four,five);
	global_current_display(two);
	screen_write_txt(&(two->characters[0][0]),strlen(two->characters[0]));
	
    
 /*   I2CInit();
	screen_on();
	screen_clear();
	
	Display* one   = global_get_display(0);
	Display* two   = global_get_display(1);
	Display* three = global_get_display(2);
	Display* four  = global_get_display(3);
	Display* five  = global_get_display(4);	
	
	global_current_display(two);
	
	menu_init(one,two,three,four,five);*/
    
	//screen_write_txt(&(two->characters[0][0]),strlen(two->characters[0]));
	
	//delaymycode(30);
	//push_encoder_button();
	
	int testnumber = 0;
		
	while(true){
		switch(testnumber){
			case 0: {
				while(1) {
				}	
			}
			case 1: {
		
			delaymycode(30);
			turn_encoder_right();
			
			delaymycode(30);
			turn_encoder_right();
		
			delaymycode(30);
			turn_encoder_right();
			break;
			}
			
			case 2:{
				
			delaymycode(30);
			turn_encoder_right();
			
			delaymycode(30);
			push_encoder_button();
			while(1){
				delaymycode(30);
				turn_encoder_right();
				}
			}	
			
			case 3: {
				delaymycode(30);
				turn_encoder_right();
			
				delaymycode(30);
				push_encoder_button();
				
				uint8_t x = 10;
				while(x > 0){
					delaymycode(30);
					turn_encoder_right();
					x--;
				}
				while(1) {
					delaymycode(30);
					turn_encoder_left();
				}
			}
			
			//autonomous mode
			case 4: {
				//delaymycode(30);
				//turn_encoder_right();
			
				//delaymycode(30);
				//push_encoder_button();
				while(1) {}	
			}
		}
	}
}
