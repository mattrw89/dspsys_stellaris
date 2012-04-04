#ifndef GLOBAL_C_
#define GLOBAL_C_
#include "global.h"


#define NUM_INPUT_CHANNELS 14
#define NUM_OUTPUT_CHANNELS 14

// goal: statically allocate Channel objects in memory
// given Io_enum and a integral channel number within bounds
// return a pointer to the Channel object
Channel* get_channel_from_memory(Io_enum io, uint8_t channel_number) {
	static Channel input_channels[NUM_INPUT_CHANNELS];
	static Channel output_channels[NUM_OUTPUT_CHANNELS];

	//verify that the channel number is within bounds and that a valid IO enum value is given
	if( io == INPUT ) {
		//printf("input channel %d requested.  address: 0x%X\n",channel_number,&(input_channels[channel_number - 1]));
		return &(input_channels[channel_number - 1]);
	}
	else if( io == OUTPUT ) {
		//printf("output channel %d requested.  address: 0x%X\n",channel_number,&(output_channels[channel_number - 1]));
		return &(output_channels[channel_number - 1]);		
	} else return NULL;
}

Display* global_current_display(Display* disp) {
	static Display* current_display_ptr;

	if( (disp != NULL) || (disp != 0) ) {
		current_display_ptr = disp;
	}
	return current_display_ptr;
}

//num only valid 0 through 2
Display* global_get_display(uint8_t num ) {
	static Display one,two,three,four,five;
	
	switch(num) {
		case 0:
			return &one;
		case 1:
			return &two;
		case 2:
			return &three;
		case 3:
			return &four;
		case 4:
			return &five;
		default: 
			return NULL;
	}
}

struct ApiHandlerVars* global_api_handler(struct ApiHandlerVars* vars) {
	static struct ApiHandlerVars* handler;

	if( (vars != NULL) || (vars != 0) ) {
		handler = vars;
	}
	return handler;
}

void callbackFunction(void* api_ptr, float b) {
	ApiRead* a = (ApiRead*) api_ptr;
	printf("in callback: b = %lf, cmd_count:%d\n\n", b, (a->super).cmd_count);
}

//---------------------------------------------------------------------------------

Encoder_State* global_get_encoder(void) {
	static Encoder_State state;
	return &state;	
}

//-----------------------------------------------------------------------------------


Address* global_get_address(void){
	static Address addr;
	return &addr;
}

//-----------------------------------------------------------------------------------

Volume* global_get_volume(void) {
	static Volume vol;
	return &vol;
}




//------------------------------------------------------------------------------------------------------

void fix_address(Address *addr, int i) {
	switch(i) {
		case 0:{
			//fixes the IP address
			check_address(addr->ip);
			break;
		}
		case 1: {
			//fixes the SUBNET MASK
			check_address(addr->subnet);
			break;
		}
		case 2: {
			//fixes he GATEWAY
			check_address(addr->gateway);
			break;
		}
	}
}

//------------------------------------------------------------------------------------------------------

void actual_change_address() {
	unsigned long ip, subnet, gateway;
	Address* addr = global_get_address();

	convert_address_to_long(addr->ip, &ip);
	convert_address_to_long(addr->subnet, &subnet);
	convert_address_to_long(addr->gateway, &gateway);
	
	// IPADDR_USE_DHCP use for mode
	lwIPNetworkConfigChange(ip, subnet, gateway, IPADDR_USE_STATIC);


}

//------------------------------------------------------------------------------------------------------

void convert_address_to_long(char *addr, unsigned long *laddr){
	char point = '.';
	char zero = '0';
	char temp[16];
	char* s;
	int i, j, length;
	uint8_t oct[4];
	unsigned long t;
	
	memcpy( temp, addr, strlen(addr) );
	length = strlen(temp);
	
	for( i = 0; i <12; i++) {
		if( temp[i] != zero ) {
			for( j = i; j<12; j++) {
				if( temp[i] == point ) {
					j = 15;
				}
				else i++;
			}
		}
		else {
			length--;
			memcpy( &(temp[i]), &(temp[i+1]), strlen(&(temp[i+1])) );
			strncpy( &(temp[length]), 0, 1);
			i--;
		}
	}
	

	j = 0;
	length = 0;
	for( i = 0; i<16; i++) {
		if( (temp[i] == point) || (temp[i] == 208) || (temp[i] == 0) ) {
			if( j < 4){
				strncpy( &(temp[i]), '\0', 1);
				s = &(temp[length]);
				oct[j] = (uint8_t)strtol(s,NULL,10);
				length = (i+1);
				j++;
			}
		}
	}
	uint8_t temp2 = 0;
	temp2 = oct[0];
	oct[0] = oct[3];
	oct[3] = temp2;
	temp2 = oct[1];
	oct[1] = oct[2];
	oct[2] = temp2;
	
	unsigned long* templong;
	templong = (unsigned long*)oct;
	t = *templong;
	*laddr = t;
}

#endif /*GLOBAL_C_*/
