//
//  json.c
//  dspsys_xcode_dev
//
//  Created by Matt Webb on 1/21/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#include <stdio.h>
#include "json.h"

//Convert eqbands in a channel to json
Json* json_encode_eq(Channel* channel, Json* json_output) {
    Json* json[4];
    json[0] = malloc(sizeof(Json));
    json[1] = malloc(sizeof(Json));
    json[2] = malloc(sizeof(Json));
    json[3] = malloc(sizeof(Json));
    char temp[20];
    uint8_t i=0;
    for( i=0; i < 4; i++ ) {
    
        KeyValueMap map;
        map.num_values = 0;
        
        EqBand* current = channel_get_eqband(channel, i+1);
        
        get_string_from_float(i, temp);
        map_add_key_value_pair_lite(&map, "bandNum", temp, 7);
        
        get_string_from_float(eqband_is_enabled(current), temp);
        map_add_key_value_pair_lite(&map, "enable", temp, 6);
        
        get_string_from_float(eqband_get_bw(current), temp);
        map_add_key_value_pair_lite(&map, "q", temp, 1);
        
        get_string_from_float(eqband_get_freq(current), temp);
        map_add_key_value_pair_lite(&map, "freq", temp, 4);
        
        get_string_from_float(eqband_get_gain(current), temp);
        map_add_key_value_pair_lite(&map, "gain", temp, 4);
        
        get_string_from_float(eqband_get_type(current), temp);
        map_add_key_value_pair_lite(&map, "type", temp, 4);
        
        json[i] = json_encode_hashmap(json[i], &map);
    }
    
    //Now we have 4 json objects, each containing a string with an eq band json object, stringified
    //need to concatenate the objects into one json object that can be returned
    json_encode_array(json, 4, json_output);
    free(json[0]);
    free(json[1]);
    free(json[2]);
    free(json[3]);
    
    return json_output;
}

//Covert comp in a channel to json
Json* json_encode_comp(Channel* channel, Json* json_output) {
    KeyValueMap map;
    map.num_values = 0;
    char temp[20];
    
    Comp* current = channel_get_comp(channel);
    
    get_string_from_float(comp_get_gain(current),temp);
    map_add_key_value_pair_lite(&map, "gain", temp, 4);
    
    get_string_from_float(comp_get_threshold(current),temp);
    map_add_key_value_pair_lite(&map, "threshold", temp, 9);
    
    get_string_from_float(comp_get_ratio(current), temp);
    map_add_key_value_pair_lite(&map, "ratio", temp, 5);
   
    get_string_from_float(comp_get_attack(current), temp);
    map_add_key_value_pair_lite(&map, "attack", temp, 6);
    
    get_string_from_float(comp_get_release(current), temp);
    map_add_key_value_pair_lite(&map, "release", temp, 7);
    
    get_string_from_float(comp_is_enabled(current), temp);
    map_add_key_value_pair_lite(&map, "enable", temp, 6);
    
    json_encode_hashmap(json_output, &map);
    
    return json_output;
}

//Convert channel names and active/inactive status to json
Json* json_encode_channels(Io_enum io, uint8_t length, Json* json_output) {
    Json* json[16];
	Channel *current;
	
    char temp[3];
    char long_temp[20];
    int counter=0;
    
    for ( counter=0; counter < length; counter++) {
        KeyValueMap map;
        map.num_values = 0;
        current = get_channel_from_memory(io, counter+1);
        
        json[counter] = malloc(sizeof(Json));
        json[counter]->total_length = 0;
        
        strcpy(long_temp, channel_get_name(current));
        if (long_temp[0] == '\0') {
            long_temp[0] = '"';
            long_temp[1] = '"';
            long_temp[2] = '\0';
        }
        
        map_add_key_value_pair_lite(&map, "name", long_temp, 4);
        
        temp[0] = channel_is_active(current) + 48;
        temp[1] = '\0';
        
        map_add_key_value_pair_lite(&map, "a", temp, 6);
        
        //temp[0] = channel_get_io(current) + 48;
        //temp[1] = '\0';
        
        //map_add_key_value_pair_lite(&map, "io", temp, 2);
        
        itoa(channel_get_chan_num(current), temp);
        map_add_key_value_pair_lite(&map, "num", temp, 3);
        
        json[counter] = json_encode_hashmap(json[counter], &map);
    }
    json_encode_array(json, length, json_output);
    
    for( counter=0; counter < length; counter++) {
    	free(json[counter]);
    }
    return json_output;
}

//Encode Routing matrix as JSON
Json* json_encode_matrix(Matrix* matrix) {
    Json* json = malloc(sizeof(Json));
    json->string[0][0] = '[';
    uint8_t i = 0;
    
    for (i=0; i < matrix_get_num_output_channels(matrix); i++) {
        json->string[0][2*i+1] = matrix->routing[i] + 48;
        json->string[0][2*i+2] = ',';
    }
    json->string[0][2*i+2] = ']';
    json->total_length = 2*i+3;
    json->string[0][2*i+4] = '\0';
    return json;
}

Json* json_encode_ok(Json* json) {
    json->string[0][0] = '[';
    json->string[0][1] = ']';
    json->string[0][2] = '\0';
    json->total_length=2;
    return json;
}



//Convert Hashmap to JSON object
Json* json_encode_hashmap(Json* json, KeyValueMap* map) {
	json->total_length = 0;
    json->string[0][0] = '{'; //start the json object
    
    char* char_ptr = &(json->string[0][0]); // create a pointer to the first char memory
    
    char_ptr += sizeof(char); // increment the 
    uint8_t i =1;
    for(i=1; i <= map_get_length(map); i++) {
        *char_ptr = 34; //double quote ascii
        char_ptr += sizeof(char);
        uint8_t length_of_key = strlen(map->keys[i-1]);
        strncpy(char_ptr, map->keys[i-1], length_of_key);
        
        //increment the char pointer past newly copied key
        char_ptr += length_of_key*sizeof(char);
        json_inc_length(json, length_of_key); //increment the length of json
        
        *char_ptr = '"'; //add ending double quote
        char_ptr += sizeof(char);
        *char_ptr = ':'; //add semicolon
        char_ptr += sizeof(char);
        json_inc_length(json, 2);
        
        //now it's time to place the value as a string
        uint8_t string_flag = ( (*(map->char_value[i-1]) >= 'A') &&  (*(map->char_value[i-1]) <= 'z') ) ? 1 : 0;
        
        if ( string_flag ) {
            *char_ptr = '"';
            char_ptr += sizeof(char);
            json_inc_length(json, 1);
        }
        
        strcpy(char_ptr, map->char_value[i-1]);
        uint8_t length_of_value = strlen(map->char_value[i-1]);
        char_ptr += length_of_value*sizeof(char);
        json_inc_length(json, length_of_value+2);
        
        if ( string_flag ) {
            *char_ptr = '"';
            char_ptr += sizeof(char);
            json_inc_length(json, 1);
        }
        
        if ( i != map_get_length(map) ) {
            *char_ptr = ',';
        } else *char_ptr = '}';
        char_ptr += sizeof(char);
    }
    json_inc_length(json, 1); // inc for closing bracket
    
    //write extra space just to make sure we're OK once parsed by jQuery
    *char_ptr = 32;
    char_ptr += sizeof(char);
    *char_ptr = '\0';
    return json;
}

//Concatenate two JSON objects
Json* json_concatenate(Json* first, Json* second) {
    //need to check that there is enough room in a string (256 chars) 
    //  to be able to add the entirety of the second objects string
    
    uint8_t first_write_string = 0;
    uint16_t second_length = second->total_length;
    
    if ( (first->total_length > 256)  && (first->total_length < 512) ) {
        first_write_string = 1;
    } else if ( (first->total_length > 512)  && (first->total_length < 768) ) {
        first_write_string = 2;
    } else if ( (first-> total_length >768) ) first_write_string = 3;
    
    //if there are enough characters remaining in first then copy the entirety of second in.
    strcpy(first->string[first_write_string]+sizeof(char)*(first->total_length - 256*first_write_string), second->string[0]);
    first->total_length += second_length;
    return first;
}

//make a JSON array out of an array of JSON objects
Json* json_encode_array(Json* objects[], uint8_t num_objects, Json* json_output) {
    Json* array = json_output;
    array->total_length = 0;
    
    //add initial array bracket
    array->string[0][0] = '[';
    json_inc_length(array, 1);
    uint8_t i=0;
    for ( i=0; i < num_objects; i++) {
        json_concatenate(array, objects[i]);
        
        if ( i != (num_objects-1) ) {
            array->string[0][array->total_length] = ',';
            json_inc_length(array, 1);
        }
    }
    array->string[0][array->total_length] = ']';
    json_inc_length(array, 1);
    array->string[0][array->total_length] = 32; // 32 is a space.  to add padding for client-side JSON parser
    array->string[0][array->total_length+1] = 32;
    array->string[0][array->total_length+2] = '\0';
    
    return array;
}


uint16_t json_inc_length(Json* json, uint8_t length) {
    json->total_length += length;
    return json->total_length;
}

/* reverse:  reverse string s in place */
void reverse2(char s[])
{
    int i, j;
    char c;
    
    for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
} 

/* itoa:  convert n to characters in s */
void itoa(int n, char s[])
{
    int i, sign;
    
    if ((sign = n) < 0)  /* record sign */
        n = -n;          /* make n positive */
    i = 0;
    do {       /* generate digits in reverse order */
        s[i++] = n % 10 + '0';   /* get next digit */
    } while ((n /= 10) > 0);     /* delete it */
    if (sign < 0)
        s[i++] = '-';
    s[i] = '\0';
    reverse2(s);
}



