//
//  json.h
//  dspsys_xcode_dev
//
//  Created by Matt Webb on 1/21/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#ifndef dspsys_xcode_dev_json_h
#define dspsys_xcode_dev_json_h

#include <stdint.h>
#include "channel.h"
#include "hashmap.h"
#include "http_api.h"
#include <stdlib.h>
#include "string.h"
#include "matrix.h"
#include "global.h"

typedef struct Json {
    char string[3][256];
    uint16_t total_length;
} Json;

//Convert eqbands in a channel to json
Json* json_encode_eq(Channel* channel, Json* json_output);

//Covert comp in a channel to json
Json* json_encode_comp(Channel* channel, Json* json_output);

//Convert channel names and active/inactive status to json
Json* json_encode_channels(Io_enum io, uint8_t length, Json* json_output);

//Convert matrix IO to json
Json* json_encode_matrix(Matrix* matrix);

//Convert breakout box status to json
Json* json_encode_status_bob();

//Convert channel clip/overflow status to json
Json* json_encode_status_clip();


//json to send when a change went through OK
Json* json_encode_ok(Json* json); 

//Convert a hashmap to JSON object
Json* json_encode_hashmap(Json* json, KeyValueMap* map);

//Concatenate two JSON objects
Json* json_concatenate(Json* first, Json* second);

//make a JSON array out of an array of JSON objects
Json* json_encode_array(Json* objects[], uint8_t num_objects, Json* json_output);

uint16_t json_inc_length(Json* json, uint8_t length);

char* inc_char_pointer(char* chr);

void reverse2(char s[]);
void itoa(int n, char s[]);
#endif
