//
//  hashmap.c
//  dspsys_xcode_dev
//
//  Created by Matt Webb on 1/20/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#include "hashmap.h"

void map_add_key_value_pair(KeyValueMap* map, char* key, char* char_value, float value, uint8_t length) {
    uint8_t position = map_get_length(map);
    map->values[position] = value;
    strncpy(map->keys[position], key, length);
    map->keys[position][length] = '\0';    
    strcpy(map->char_value[position], char_value);
    map->num_values++;
}

void map_add_key_value_pair_lite(KeyValueMap* map, char* key, char* char_value, uint8_t length) {
    uint8_t position = map_get_length(map);
    strncpy(map->keys[position], key, length);
    map->keys[position][length] = '\0';
    strcpy(map->char_value[position], char_value);
    map->num_values++;
}


float map_get_value_by_key(KeyValueMap* map, char* key) {
    float returned_value = 0.0;
    char search_string[20];
    uint8_t length_values = map_get_length(map);
    uint8_t length_string = strlen(key);
    
    strncpy(search_string, key, 20);
    uint8_t i=0;
    for ( i=0; i < length_values; i++) {
        if (!strncmp(map->keys[i], key, length_string)) {
            returned_value = map->values[i];
        }
    }
    return returned_value;
}

uint8_t map_get_length(KeyValueMap* map) {
    return map->num_values;
}
