#ifndef GLOBAL_H_
#define GLOBAL_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lib/dspsys_lib_channel/channel.h"
#include "lib/dspsys_lib_channel/common.h"
#include "lib/menu/menu.h"
#include "lib/dspsys_lib_txrxspi/ApiHandler.h"

Channel* get_channel_from_memory(Io_enum io, uint8_t channel_number);

Display* global_current_display(Display* disp);

Display* global_get_display(uint8_t num );

struct ApiHandlerVars* global_api_handler(struct ApiHandlerVars* vars);

void callbackFunction(void* api_ptr, float b);

#endif /*GLOBAL_H_*/
