//
//  http_api.c
//  dspsys_xcode_dev
//
//  Created by Matt Webb on 1/13/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#include <stdio.h>
#include "http_api.h"


void http_api_info_ctor(HttpApiInfo* info) {
    info->params.num_values = 0;
}

KeyValueMap* http_api_info_get_params(HttpApiInfo* info) {
    return &(info->params);
}

Io_enum http_api_info_get_io(HttpApiInfo* info) {
    return info->io_type;
}

uint8_t http_api_info_get_chan_num(HttpApiInfo* info) {
    return info->chan_num;
}

Io_enum http_api_info_set_io(HttpApiInfo* info, Io_enum io) {
    info->io_type = io;
    return io;
}

uint8_t http_api_info_set_chan_num(HttpApiInfo* info, uint8_t cnum) {
    info->chan_num = cnum;
    return cnum;
}



uint8_t http_process_url(char* first_char, uint8_t length, void* jsonvoid) {
    char temp[80];
    Json* json = (Json*) jsonvoid;
    uint8_t temp_len = length;
    
    HttpApiInfo api_info;
    http_api_info_ctor(&api_info);
    
    strncpy(temp, first_char, 80);
    
    if(!strncmp(temp, "/a/", 3)) {
        //ok, we have an api command
        advance_string(temp, &temp_len, 3);
        //!printf("we have a web API command: leftover %s\n", temp);
        
        if( ((temp[0] == 'o') || (temp[0] == 'i')) && (temp[1] == '/') ) {
            //we have an input or an output being specified
            //!printf("input/output status: %c\n", temp[0]);
            http_api_info_set_io(&api_info, ( (temp[0] == 'i') ? INPUT : OUTPUT ));
            
            advance_string(temp, &temp_len, 2);
            //!printf("%s\n",temp);
            //now we need to find the channel number
            uint8_t testnum = get_channel_number(temp,&temp_len);
            Io_enum io = (Io_enum) http_api_info_get_io(&api_info);
            
            if ( (testnum > 0) && (http_api_info_set_chan_num(&api_info, testnum) > 0) ) {
                Channel* tempchan = get_channel_from_memory(io, testnum);
                
                //make sure a channel according to those specifications exists.  if not, return no match.	
                if ( tempchan == NULL )	return 0;
                
                if (!strncmp(temp, "eqparams", 7)) {
                	json_encode_eq(tempchan, json);
                    //!printf("eqparams!!\n");
                    return 1;
                } else if(!strncmp(temp,"modeq",5)) {
                	advance_string(temp, &temp_len, 5);
                    extract_query_params(temp, &temp_len, http_api_info_get_params(&api_info));
                    float band_num = map_get_value_by_key(http_api_info_get_params(&api_info),"b");
                    float eq_type = map_get_value_by_key(http_api_info_get_params(&api_info),"t");
                    float eq_q = map_get_value_by_key(http_api_info_get_params(&api_info),"q");
                    float eq_freq = map_get_value_by_key(http_api_info_get_params(&api_info),"f");
                    float eq_gain = map_get_value_by_key(http_api_info_get_params(&api_info),"g");
                    float eq_enable = map_get_value_by_key(http_api_info_get_params(&api_info),"e");
                    
                    if( (band_num >= 1) && (band_num <= 4 ) ) {
                    	//EqBand* temp_eq_band = channel_get_eqband(tempchan, (uint8_t) band_num);
                    	
                    	//eqband_set_type(temp_eq_band, (Eq_type_enum)(eq_type));
                    	//eqband_set_bw(temp_eq_band, eq_q);
                    	//eqband_set_freq(temp_eq_band, eq_freq);
                    	//eqband_set_gain(temp_eq_band, eq_gain);
                    	//( (uint8_t)eq_enable == ENABLED) ? eqband_enable(temp_eq_band) : eqband_disable(temp_eq_band);
                    	
                    	Api_set_eqband_type(tempchan, (uint8_t) band_num, (Eq_type_enum)(eq_type));
                    	Api_set_eqband_bw(tempchan, (uint8_t) band_num, eq_q);
                    	Api_set_eqband_freq(tempchan, (uint8_t)band_num, eq_freq);
                    	Api_set_eqband_gain(tempchan, (uint8_t)band_num, eq_gain);
                    	( (uint8_t)eq_enable == ENABLED) ? Api_enable_eqband(tempchan, (uint8_t)band_num) : Api_disable_eqband(tempchan, (uint8_t)band_num);
                    	
                    	
                    	
                    	json_encode_ok(json);
                    	return 1;
                    } else return 0;
                    
                	
                } else if (!strncmp(temp, "compparams", 9)) {
                	json_encode_comp(tempchan, json);
                    //!printf("compparams!!\n");
                    return 1;
                } else if (!strncmp(temp,"modcomp",7)) {
                	//!printf("modcomp!\n");
                	advance_string(temp, &temp_len, 7);
                	
                    extract_query_params(temp, &temp_len, http_api_info_get_params(&api_info));
                    float ratio = map_get_value_by_key(http_api_info_get_params(&api_info),"r");
                    float threshold = map_get_value_by_key(http_api_info_get_params(&api_info),"t");
                    float attack = map_get_value_by_key(http_api_info_get_params(&api_info),"a");
                    float release = map_get_value_by_key(http_api_info_get_params(&api_info),"rls");
                    float gain = map_get_value_by_key(http_api_info_get_params(&api_info),"g");
                    float enable = map_get_value_by_key(http_api_info_get_params(&api_info),"e");

					Comp* temp_comp = channel_get_comp(tempchan);
                    comp_set_ratio(temp_comp, ratio);
                    comp_set_threshold(temp_comp,threshold);
                    comp_set_attack(temp_comp,attack);
                    comp_set_release(temp_comp,release);
                    comp_set_gain(temp_comp, gain);
                    ( (uint8_t)enable == ENABLED) ? comp_enable(temp_comp) : comp_disable(temp_comp);
                    	
                    json_encode_ok(json);
                    return 1;
                } else if (!strncmp(temp,"rename?name=",12)) {
                    //!printf("rename channel!!!");
                    advance_string(temp, &temp_len, 12);
                	//!printf("\nname: %s\n", temp);
                	
                	char ts[20];
                	strcpy(ts, temp);
                	
                	//allow A-Z, a-z, 0-9, -, _
                	//convert + to space
                	uint8_t j = 0;
                	uint8_t good_length = 0;
                	for(j=0; j < strlen(ts); j++) {
                		if ( ( (ts[j] <= 0x39) && (ts[j] >= 0x30) ) || 
                			 ( (ts[j] <= 0x5A) && (ts[j] >= 0x41) ) ||
                			 ( (ts[j] <= 0x7A) && (ts[j] >= 0x61) ) ||
                			 (ts[j] == 0x20) || (ts[j] == 0x5F) ||
                			 (ts[j] == 0x2D) || (ts[j] == 0x2B)
                			) {
                				if ( ts[j] == 0x2B ) ts[j] = 0x20;
                				good_length++;
                		} else break;
                	}
                	                    
                    channel_set_name(tempchan, ts);
                    json_encode_ok(json);
                    return 1;
                    
                } else {
                    //ERROR
                    //!printf("ERROR: not eqparams, compparams, or rename\n");
                }
                
            } else if (!strncmp(temp, "chanlist", 8)) {
                //return a list of channel data
                if ( io == INPUT) {
                	json_encode_channels(INPUT,NUM_INPUT_CHANNELS, json);
                	return 1;
                } else if ( io == OUTPUT) {
                	json_encode_channels(OUTPUT,NUM_OUTPUT_CHANNELS,json);
                	return 1;
                } else return 0;
                
            } else {
                //ERROR
                //!printf("ERROR: what error is this?\n");
            }
            
        } else if (temp[0] == 's') {
            //!printf("we are requesting a status\n");
            advance_string(temp, &temp_len, 2);
            if(!strncmp(temp,"bob",3)) {
                //!printf("return the bob status info\n");
            } else if(!strncmp(temp,"clip",4)) {
                //!printf("return clip information\n");
            } else {
                //!printf("no matching status api command\n");
            }
            
            
        } else if(temp[0] == 'm' && temp[1] == '/') {
            advance_string(temp, &temp_len, 2);
            //!printf("we are requesting routing matrix\n");
            if ( (http_api_info_set_chan_num(&api_info, get_channel_number(temp, &temp_len))) > 0) {
                //!printf("matrix output chan number: %d \n", http_api_info_get_chan_num(&api_info));
                
                if(!strncmp(temp, "modroute", 8)) {
                    //!printf("now we're modifying a route\n");
                    advance_string(temp, &temp_len, 8);
                    extract_query_params(temp, &temp_len, http_api_info_get_params(&api_info));
                    map_get_value_by_key(http_api_info_get_params(&api_info),"i");
                    char test[5];
                    strncpy(test, "blah",4);
                    map_get_value_by_key(http_api_info_get_params(&api_info), test);
                }
                
            }
            
        } else {
            //ERROR
            //!printf("ERROR: incorrect api command identifier\n");
            return 0;
        }
    } else {
        //printf("we don't have a web API command\n");
        return 0;
        //we don't have an API command... send it to the filesystem
    }
    //!printf("channel number: %d\n\n\n", http_api_info_get_chan_num(&api_info));
    return 0;
}

void advance_string(char* string, uint8_t* length, uint8_t num_chars) {
    strncpy(string,string + num_chars*sizeof(char), *length-num_chars);
    int i=1;
    for( i=1; i <= num_chars; i++) {
        string[*length-i] = '\0';    
    }
    
    *length = *length - num_chars;
}


uint8_t get_channel_number(char* string, uint8_t* length ) {
    //now we need to find the channel number
    
    uint8_t chan_num = 0;  //if function returns 0 there is an error
    
    if( (string[0] <= '9') && (string[0] >= '0') ) {
        if(string[1] == '/') {
            chan_num = (string[0] - '0');
            advance_string(string, length, 2);
        } else if( (string[1] <= '9') && (string[1] >= '0') && (string[2] == '/') ) {
            uint8_t msb = string[0] - '0';
            uint8_t lsb = string[1] - '0';
            chan_num = (msb*10 + lsb);
            advance_string(string, length, 3);
            
            if( chan_num > 16 ) {
                //ERROR
                //!printf("ERROR: channel_number is larger than max number of channels\n");
            }
        } else {
            //ERROR
            //!printf("ERROR: wrong channel number format\n");
        }
    } else {
        //ERROR
        //!printf("not a number to start channel!");
    }
    return chan_num;
}

uint8_t extract_query_params(char* string, uint8_t* length, KeyValueMap* map) {
    uint8_t kv_counter = 0;
    
    if ( string[0] == '?' ) {
        advance_string(string, length, 1); //get past the questionmark
        
        while ( (kv_counter != 99) && (string[0] != '\0') ) {
            uint8_t equals_position = strcspn(string, "="); //figure out where the equal sign is in the string
            string[equals_position] = '\0'; //set the equals to the string termination character
            
            char key[20],value[20];
            strncpy(key, string, equals_position); //copy key into new memory location before string gets incremented by get_value_from_string
            key[equals_position] = '\0';
            
            uint8_t length_of_value = strcspn(string, "&/");
            strncpy(value, string, length_of_value);
            value[length_of_value] = '\0';
            
            map_add_key_value_pair(map, key, value, get_value_from_string(string, length, equals_position), equals_position+1);
            
            
            //if there is another parameter, dictated by a &, then increment the counter and move it past there.
            if ( string[0] == '&' ) {
                kv_counter++;
                advance_string(string, length, 1);
            } else kv_counter = 99;
        }
    }
    return 1;
}

float get_value_from_string(char* string, uint8_t* length, uint8_t offset) {
    advance_string(string, length, offset+1);
    
    float mult = 1;
    float temp_value = 0.0;
    uint8_t dec_flag = 0;
    uint8_t neg_flag = 0;
    
    if (string[0] == '-') {
    	advance_string(string,length, 1);
    	neg_flag = 1;
    }
    
    uint8_t num_digits_front_decimal = strcspn(string, "./&");
    uint8_t j = num_digits_front_decimal-1;
    
    if (string[num_digits_front_decimal] == '.') {
        dec_flag = 1;
    }
    
    //create a multiplier according to the number of digits left of the decimal
    mult = 1;
    while( j > 0) {
        mult *= 10;
        j--;
    }
    
    //loop through the digits in front of the decimal
    for ( j = num_digits_front_decimal; j > 0; j--) {
        //if it's a number then add it's integer value to temp_value multiplied by the multiplier
        if( (string[0] <= '9') && (string[0] >= '0') ) {
            temp_value += ((string[0] - '0') * mult);
        }
        mult /= 10; // shift 
        advance_string(string, length, 1);
    }
    
    //if there is a decimal do the same thing as above
    if(dec_flag) {
        advance_string(string, length, 1);
        uint8_t num_digits_post_decimal = strcspn(string, "/&");

        for ( j = num_digits_post_decimal; j > 0; j--) {
            temp_value += (string[0] -'0') * mult;
            mult /= 10;
            advance_string(string, length, 1);
        }
    }
    
    if(neg_flag) {
    	temp_value *= -1; // make it negative!
    }
    
    return temp_value;
}

void get_string_from_float(float value, char* string) {
    //float value1 = value;
    uint8_t temp;
    float divider = 10000;
    uint8_t start = 0;
    uint8_t decimal_placed = 0;
    char* chr_ptr = string;
    uint8_t i=0;
    
    //if it's negative then add a negative sign
    if ( value < 0 ) {
    	*chr_ptr = '-';
    	chr_ptr += sizeof(char);
    	value *= -1;
    }
    
    for ( i=0; i<7; i++) {
        
        temp = value/divider;
        if(temp > 0 && start == 0) {
            start = 1;
        }
        
        if( start || decimal_placed || i == 4 ) {
            *chr_ptr = temp + '0';
            value -= temp*divider;
            chr_ptr += sizeof(char);
        }
        divider /= 10;
        
        //printf("divider:  %f, i = %d\n", divider, i);
        //printf("decimal_placed: %d, value: %f", decimal_placed, value);
        if( i > 3 && !decimal_placed ) {
            *chr_ptr = '.';
            decimal_placed = 1;
            chr_ptr += sizeof(char);
        }
    }
    
    if (chr_ptr == string) {
        *chr_ptr = '0';
        chr_ptr += sizeof(char);
    }
    
    *chr_ptr = '\0';
}

