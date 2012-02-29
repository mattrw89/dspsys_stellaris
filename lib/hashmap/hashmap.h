//
//  hashmap.h
//  dspsys_xcode_dev
//
//  Created by Matt Webb on 1/20/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#ifndef dspsys_xcode_dev_hashmap_h
#define dspsys_xcode_dev_hashmap_h

#include <stdint.h>
#include <string.h>

typedef struct KeyValueMap {
    char keys[20][20];
    float values[20];
    uint8_t num_values;
    char char_value[20][10];
} KeyValueMap;

void map_add_key_value_pair(KeyValueMap* map, char* key, char* char_value, float value, uint8_t length);

void map_add_key_value_pair_lite(KeyValueMap* map, char* key, char* char_value, uint8_t length);

float map_get_value_by_key(KeyValueMap* map, char* key);

uint8_t map_get_length(KeyValueMap* map);

#endif
