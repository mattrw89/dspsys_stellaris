#ifndef GLOBAL_H_
#define GLOBAL_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lib/dspsys_lib_channel/channel.h"
#include "lib/dspsys_lib_channel/common.h"
#include "lib/menu/menu.h"
#include "lib/dspsys_lib_txrxspi/ApiHandler.h"
#include "lib/qei/quad.h"
#include "utils/lwiplib.h"

typedef struct {
	char ip[16];
	char subnet[16];
	char gateway[16];
} Address;

typedef struct {
	char left_level [16];
	char right_level [16];
	long l_level;
	long r_level;
} Volume;

Channel* get_channel_from_memory(Io_enum io, uint8_t channel_number);

Display* global_current_display(Display* disp);

Display* global_get_display(uint8_t num );

struct ApiHandlerVars* global_api_handler(struct ApiHandlerVars* vars);

void callbackFunction(void* api_ptr, float b);

Encoder_State* global_get_encoder(void);

Address* global_get_address(void);

Volume* global_get_volume(void);

void fix_address(Address *addr,int i);

void actual_change_address();

void convert_address_to_long(char *addr, unsigned long *laddr);

#endif /*GLOBAL_H_*/
