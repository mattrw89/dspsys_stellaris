//*****************************************************************************
//
// enet_lwip.c - Sample WebServer Application using lwIP.
//
// Copyright (c) 2007-2011 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 7611 of the EK-LM3S8962 Firmware Package.
//
//*****************************************************************************
#include <string.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_nvic.h"
#include "inc/hw_types.h"
#include "driverlib/ethernet.h"
#include "driverlib/flash.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "utils/locator.h"
#include "utils/lwiplib.h"
#include "utils/uartstdio.h"
#include "utils/ustdlib.h"
#include "httpserver_raw/httpd.h"
//#include "third_party/lwip-1.3.2/apps/httpserver_raw/httpd.h"
#include "drivers/rit128x96x4.h"
//added by matt webb
#include "io.h"
#include "cgifuncs.h"
#include "lib/hashmap/hashmap.h"
#include "lib/json/json.h"
#include "global.h"
#include "lib/dspsys_lib_channel/channel.h"
#include "lib/dspsys_lib_channel/common.h"
#include <cfloat>


//*****************************************************************************
//
//! \addtogroup example_list
//! <h1>Ethernet with lwIP (enet_lwip)</h1>
//!
//! This example application demonstrates the operation of the Stellaris
//! Ethernet controller using the lwIP TCP/IP Stack.  DHCP is used to obtain
//! an Ethernet address.  If DHCP times out without obtaining an address,
//! AUTOIP will be used to obtain a link-local address.  The address that is
//! selected will be shown on the OLED display.
//!
//! The file system code will first check to see if an SD card has been plugged
//! into the microSD slot.  If so, all file requests from the web server will
//! be directed to the SD card.  Otherwise, a default set of pages served up
//! by an internal file system will be used.
//!
//! For additional details on lwIP, refer to the lwIP web page at:
//! http://savannah.nongnu.org/projects/lwip/
//
//*****************************************************************************

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


//ADDED BY MATT WEBB TO INCLUDE CGI FUNCTIONALITY

//*****************************************************************************
//
// SSI tag indices for each entry in the g_pcSSITags array.
//
//*****************************************************************************
#define SSI_INDEX_JSON1   0
#define SSI_INDEX_JSON2   1
#define SSI_INDEX_JSON3   2
#define SSI_INDEX_JSON4   3
#define SSI_INDEX_JSON5   4

//*****************************************************************************
//
// This array holds all the strings that are to be recognized as SSI tag
// names by the HTTPD server.  The server will call SSIHandler to request a
// replacement string whenever the pattern <!--#tagname--> (where tagname
// appears in the following array) is found in ".ssi", ".shtml" or ".shtm"
// files that it serves.
//
//*****************************************************************************
static const char *g_pcConfigSSITags[] =
{
    "JSON1",        // SSI_INDEX_LEDSTATE
    "JSON2",        // SSI_INDEX_PWMSTATE
    "JSON3",        // SSI_INDEX_PWMFREQ
    "JSON4",        // SSI_INDEX_PWMDUTY
    "JSON5"         // SSI_INDEX_FORMVARS
};

//*****************************************************************************
//
//! The number of individual SSI tags that the HTTPD server can expect to
//! find in our configuration pages.
//
//*****************************************************************************
#define NUM_CONFIG_SSI_TAGS     (sizeof(g_pcConfigSSITags) / sizeof (char *))



//*****************************************************************************
//
//! Prototypes for the various CGI handler functions.
//
//*****************************************************************************
static char *ControlCGIHandler(int iIndex, int iNumParams, char *pcParam[],
                              char *pcValue[]);
static char *SetTextCGIHandler(int iIndex, int iNumParams, char *pcParam[],
                              char *pcValue[]);

//*****************************************************************************
//
// CGI URI indices for each entry in the g_psConfigCGIURIs array.
//
//*****************************************************************************
#define CGI_INDEX_CONTROL       0
#define CGI_INDEX_TEXT          1

//*****************************************************************************
//
//! This array is passed to the HTTPD server to inform it of special URIs
//! that are treated as common gateway interface (CGI) scripts.  Each URI name
//! is defined along with a pointer to the function which is to be called to
//! process it.
//
//*****************************************************************************
static const tCGI g_psConfigCGIURIs[] =
{
    { "/iocontrol.cgi", ControlCGIHandler },      // CGI_INDEX_CONTROL
    { "/settxt.cgi", SetTextCGIHandler }          // CGI_INDEX_TEXT
};

//*****************************************************************************
//
//! The number of individual CGI URIs that are configured for this system.
//
//*****************************************************************************
#define NUM_CONFIG_CGI_URIS     (sizeof(g_psConfigCGIURIs) / sizeof(tCGI))

//*****************************************************************************
//
//! The file sent back to the browser by default following completion of any
//! of our CGI handlers.  Each individual handler returns the URI of the page
//! to load in response to it being called.
//
//*****************************************************************************
#define DEFAULT_CGI_RESPONSE    "/io_cgi.ssi"

//*****************************************************************************
//
//! The file sent back to the browser in cases where a parameter error is
//! detected by one of the CGI handlers.  This should only happen if someone
//! tries to access the CGI directly via the broswer command line and doesn't
//! enter all the required parameters alongside the URI.
//
//*****************************************************************************
#define PARAM_ERROR_RESPONSE    "/perror.htm"

#define JAVASCRIPT_HEADER                                                     \
    "<script type='text/javascript' language='JavaScript'>\n"
#define JAVASCRIPT_FOOTER                                                     \
    "</script>\n"  


//*****************************************************************************
//
// This CGI handler is called whenever the web browser requests iocontrol.cgi.
//
//*****************************************************************************
static char *
ControlCGIHandler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[])
{
    tBoolean bParamError;
    long lLEDState, lPWMState, lPWMDutyCycle, lPWMFrequency;

    //
    // We have not encountered any parameter errors yet.
    //
    bParamError = false;

    //
    // Get each of the 4 expected parameters.
    //
    lLEDState = FindCGIParameter("LEDOn", pcParam, iNumParams);
    lPWMState = FindCGIParameter("PWMOn", pcParam, iNumParams);
    lPWMDutyCycle = GetCGIParam("PWMDutyCycle", pcParam, pcValue, iNumParams,
                                &bParamError);
    lPWMFrequency = GetCGIParam("PWMFrequency", pcParam, pcValue, iNumParams,
                                &bParamError);

    //
    // Was there any error reported by the parameter parser?
    //
    if(bParamError || (lPWMDutyCycle < 0) || (lPWMDutyCycle > 100) ||
       (lPWMFrequency < 200) || (lPWMFrequency > 20000))
    {
        return(PARAM_ERROR_RESPONSE);
    }

    //
    // We got all the parameters and the values were within the expected ranges
    // so go ahead and make the changes.
    //
    io_set_led((lLEDState == -1) ? false : true);
    io_pwm_dutycycle((unsigned long)lPWMDutyCycle);
    io_pwm_freq((unsigned long)lPWMFrequency);
    io_set_pwm((lPWMState == -1) ? false : true);

    //
    // Send back the default response page.
    //
    return(DEFAULT_CGI_RESPONSE);
}

//*****************************************************************************
//
// This CGI handler is called whenever the web browser requests settxt.cgi.
//
//*****************************************************************************
static char *
SetTextCGIHandler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[])
{
    long lStringParam;
    char pcDecodedString[24];

    //
    // Find the parameter that has the string we need to display.
    //
    lStringParam = FindCGIParameter("DispText", pcParam, iNumParams);

    //
    // If the parameter was not found, show the error page.
    //
    if(lStringParam == -1)
    {
        return(PARAM_ERROR_RESPONSE);
    }

    //
    // The parameter is present. We need to decode the text for display.
    //
    DecodeFormString(pcValue[lStringParam], pcDecodedString, 24);

    //
    // Claim the SSI for communication with the display.
    //
    RIT128x96x4Enable(1000000);

    //
    // Erase the previous string and overwrite it with the new one.
    //
    RIT128x96x4StringDraw("                      ", 0, 64, 12);
    RIT128x96x4StringDraw(pcDecodedString, 0, 64, 12);

    //
    // Release the SSI.
    //
    RIT128x96x4Disable();

    //
    // Tell the HTTPD server which file to send back to the client.
    //
    return(DEFAULT_CGI_RESPONSE);
}

//*****************************************************************************
//
// This function is called by the HTTP server whenever it encounters an SSI
// tag in a web page.  The iIndex parameter provides the index of the tag in
// the g_pcConfigSSITags array. This function writes the substitution text
// into the pcInsert array, writing no more than iInsertLen characters.
//
//*****************************************************************************
static int
SSIHandler(int iIndex, char *pcInsert, int iInsertLen)
{
    unsigned long ulVal;
    int tempa = 0;

    //
    // Which SSI tag have we been passed?
    //
    switch(iIndex)
    {
        case SSI_INDEX_JSON1:
            io_get_ledstate(pcInsert, iInsertLen);
            break;

        case SSI_INDEX_JSON2:
            io_get_pwmstate(pcInsert, iInsertLen);
            break;

        case SSI_INDEX_JSON3:
            ulVal = io_get_pwmfreq();
            usnprintf(pcInsert, iInsertLen, "%d", ulVal);
            break;

        case SSI_INDEX_JSON4:
            ulVal = io_get_pwmdutycycle();
            usnprintf(pcInsert, iInsertLen, "%d", ulVal);
            break;

         case SSI_INDEX_JSON5:
         	usnprintf(pcInsert, iInsertLen,
         	"%s\njson=%s\n%s",
         	JAVASCRIPT_HEADER,
         	//"[{\"name\":\"ch01\",\"active\":1,\"io\":0,\"num\":2},{\"name\":\"Ch2!\",\"active\":1,\"io\":0,\"num\":2},{\"name\":\"1\",\"active\":0,\"io\":0,\"num\":0},{\"name\":\"1\",\"active\":0,\"io\":0,\"num\":0},{\"name\":\"2\",\"active\":0,\"io\":0,\"num\":0},{\"name\":\"2\",\"active\":0,\"io\":0,\"num\":0},{\"name\":\"3\",\"active\":0,\"io\":0,\"num\":0},{\"name\":\"3\",\"active\":0,\"io\":0,\"num\":0},{\"name\":\"4\",\"active\":0,\"io\":0,\"num\":0},{\"name\":\"4\",\"active\":0,\"io\":0,\"num\":0},{\"name\":\"5\",\"active\":0,\"io\":0,\"num\":0},{\"name\":\"5\",\"active\":0,\"io\":0,\"num\":0},{\"name\":\"6\",\"active\":0,\"io\":0,\"num\":0},{\"name\":\"7\",\"active\":0,\"io\":0,\"num\":0}]",
         	"[{\"name\":\"ch01\",\"active\":1,\"io\":0,\"num\":2},{\"name\":\"Ch2!\",\"active\":1,\"io\":0,\"num\":2}]",
         	JAVASCRIPT_FOOTER);
         	break;

        default:
            usnprintf(pcInsert, iInsertLen, "??");
            break;
    }

    //
    // Tell the server how many characters our insert string contains.
    //

    tempa = strlen(pcInsert);
    return(tempa);
}   
    //END CODE INSERTED BY MATT

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
    RIT128x96x4StringDraw(pucBuf, ulCol, ulRow, 15);
}

//*****************************************************************************
//
// Required by lwIP library to support any host-related timer functions.
//
//*****************************************************************************
void
lwIPHostTimerHandler(void)
{
    static unsigned long ulLastIPAddress = 0;
    unsigned long ulIPAddress;

    ulIPAddress = lwIPLocalIPAddrGet();

    //
    // If IP Address has not yet been assigned, update the display accordingly
    //
    if(ulIPAddress == 0)
    {
        static int iColumn = 6;

        //
        // Update status bar on the display.
        //
        RIT128x96x4Enable(1000000);
        if(iColumn < 12)
        {
            RIT128x96x4StringDraw("< ", 0, 24, 15);
            RIT128x96x4StringDraw("*",iColumn, 24, 7);
        }
        else
        {
            RIT128x96x4StringDraw(" *",iColumn - 6, 24, 7);
        }

        iColumn++;
        if(iColumn > 114)
        {
            iColumn = 6;
            RIT128x96x4StringDraw(" >", 114, 24, 15);
        }
        RIT128x96x4Disable();
    }

    //
    // Check if IP address has changed, and display if it has.
    //
    else if(ulLastIPAddress != ulIPAddress)
    {
        ulLastIPAddress = ulIPAddress;
        RIT128x96x4Enable(1000000);
        RIT128x96x4StringDraw("                       ", 0, 16, 15);
        RIT128x96x4StringDraw("                       ", 0, 24, 15);
        RIT128x96x4StringDraw("IP:   ", 0, 16, 15);
        RIT128x96x4StringDraw("MASK: ", 0, 24, 15);
        RIT128x96x4StringDraw("GW:   ", 0, 32, 15);
        DisplayIPAddress(ulIPAddress, 36, 16);
        ulIPAddress = lwIPLocalNetMaskGet();
        DisplayIPAddress(ulIPAddress, 36, 24);
        ulIPAddress = lwIPLocalGWAddrGet();
        DisplayIPAddress(ulIPAddress, 36, 32);
        RIT128x96x4Disable();
    }
}

//*****************************************************************************
//
// The interrupt handler for the SysTick interrupt.
//
//*****************************************************************************
void
SysTickIntHandler(void)
{
    //
    // Call the lwIP timer handler.
    //
    lwIPTimer(SYSTICKMS);

    //
    // Run the file system tick handler.
    //
    fs_tick(SYSTICKMS);
}

//*****************************************************************************
//
// This example demonstrates the use of the Ethernet Controller.
//
//*****************************************************************************

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


int
main(void)
{

    unsigned long ulUser0, ulUser1;
    unsigned char pucMACArray[8];

    //
    // Set the clocking to run directly from the crystal.
    //
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_8MHZ);

    //
    // Initialize the UART for debug output.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTStdioInit(0);

    //
    // Initialize the OLED display.
    //
    RIT128x96x4Init(1000000);
    RIT128x96x4StringDraw("Ethernet  w/  lwIP", 12, 0, 15);

    //
    // Enable and Reset the Ethernet Controller.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ETH);
    SysCtlPeripheralReset(SYSCTL_PERIPH_ETH);

    //
    // Enable Port F for Ethernet LEDs.
    //  LED0        Bit 3   Output
    //  LED1        Bit 2   Output
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIOPinTypeEthernetLED(GPIO_PORTF_BASE, GPIO_PIN_2 | GPIO_PIN_3);

    //
    // Configure SysTick for a periodic interrupt.
    //
    SysTickPeriodSet(SysCtlClockGet() / SYSTICKHZ);
    SysTickEnable();
    SysTickIntEnable();

    //
    // Enable processor interrupts.
    //
    IntMasterEnable();

    //
    // Initialize the file system.
    //
    RIT128x96x4Disable();
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
        RIT128x96x4Enable(1000000);
        RIT128x96x4StringDraw("MAC Address", 0, 16, 15);
        RIT128x96x4StringDraw("Not Programmed!", 0, 24, 15);
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

    //
    // Initialze the lwIP library, using DHCP.
    //
    lwIPInit(pucMACArray, 0, 0, 0, IPADDR_USE_DHCP);

    //
    // Setup the device locator service.
    //
    LocatorInit();
    LocatorMACAddrSet(pucMACArray);
    LocatorAppTitleSet("EK-LM3S8962 enet_lwip");

    //
    // Indicate that DHCP has started.
    //
    RIT128x96x4Enable(1000000);
    RIT128x96x4StringDraw("Waiting for IP", 0, 16, 15);
    RIT128x96x4StringDraw("<                   > ", 0, 24, 15);
    RIT128x96x4Disable();

    //
    // Initialize a sample httpd server.
    //
    httpd_init();
    
    // ADDED BY MATT WEBB
    
    //
    // Pass our tag information to the HTTP server.
    //
    http_set_ssi_handler(SSIHandler, g_pcConfigSSITags,
                         NUM_CONFIG_SSI_TAGS);
                         
                         
    // Pass our CGI handlers to the HTTP server.
    //
    http_set_cgi_handlers(g_psConfigCGIURIs, NUM_CONFIG_CGI_URIS);
    //END ADD BY MATT WEBB

    //
    // Set the interrupt priorities.  We set the SysTick interrupt to a higher
    // priority than the Ethernet interrupt to ensure that the file system
    // tick is processed if SysTick occurs while the Ethernet handler is being
    // processed.  This is very likely since all the TCP/IP and HTTP work is
    // done in the context of the Ethernet interrupt.
    //
    IntPriorityGroupingSet(4);
    IntPrioritySet(INT_ETH, ETHERNET_INT_PRIORITY);
    IntPrioritySet(FAULT_SYSTICK, SYSTICK_INT_PRIORITY);

        UARTprintf("enet_lwip\n");
        
	io_init();
    //
    // Loop forever.  All the work is done in interrupt handlers.
    //
    
	channels_init();
    
    while(1)
    {
    }
}


