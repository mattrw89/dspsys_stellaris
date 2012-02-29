//
//  http_api.h
//  dspsys_xcode_dev
//
//  Created by Matt Webb on 1/13/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#ifndef dspsys_xcode_dev_http_api_h
#define dspsys_xcode_dev_http_api_h

#include <string.h>
#include <stdint.h>
#include "hashmap.h"
#include "common.h"
#include <stdlib.h>
#include "global.h"
#include "json.h"
#include "channel.h"
#include "eq.h"
#include "comp.h"

////////////HTTP API INFO STRUCTURE & METHODS///////////////
typedef struct HttpApiInfo {
    uint8_t chan_num;
    Io_enum io_type;
    KeyValueMap params;
} HttpApiInfo;   

//////////////  Constructor /////////////////
void http_api_info_ctor(HttpApiInfo* info);

//////////////  Getters ///////////////////////
KeyValueMap* http_api_info_get_params(HttpApiInfo* info);
Io_enum http_api_info_get_io(HttpApiInfo* info);
uint8_t http_api_info_get_chan_num(HttpApiInfo* info);

//////////////  Setters //////////////////////
Io_enum http_api_info_set_io(HttpApiInfo* info, Io_enum io);
uint8_t http_api_info_set_chan_num(HttpApiInfo* info, uint8_t cnum);


///////////// Actual Processing Method //////////////

uint8_t http_process_url(char* first_char, uint8_t length, void* json);


///////////// Private Processing Methods ///////////////////
void advance_string(char* string, uint8_t* length, uint8_t num_chars);
uint8_t get_channel_number(char* string, uint8_t* length);
uint8_t extract_query_params(char* string, uint8_t* length, KeyValueMap* map);
float get_value_from_string(char* string, uint8_t* length, uint8_t offset);
void get_string_from_float(float value, char* string);
#endif
