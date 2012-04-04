#include "menu.h"
#include "global.h"
#include <string.h>

//------------------------------------------------------------------------------------------------------

void s1_right(Display *disp, Menu_enum menu_type, uint16_t value) {
	if(menu_type != HOME) {
		//temp storage for realignment of pointers
		Display* oldcurrent = disp;
		Display* oldright = disp->right;
		Display* oldleft = disp->left;
		
		Display* oldselect = disp->select;
		Display* oldback = disp->back;
		
		//realign pointers to the apprioate spot
		disp = oldright;
		disp->left = oldcurrent;
		disp->right = oldleft;
		disp->back = oldback;
		disp->select = oldselect;
	
		//determines which stage menu is at
		switch( (int)(menu_type >> 4) ) {
			
			case 0: {
				display_ctor(disp->right, (Menu_enum)HOME, NULL, NULL, NULL, NULL, NULL);
				break;
			}
			
			case 1: {
				if( menu_type < 0x13)
					disp->menu_type = ++menu_type;
				else {
					disp->menu_type = IP_ADDRESS;
					menu_type = IP_ADDRESS;
				}
				if( menu_type < 0x13)
					menu_type++;
				else menu_type = IP_ADDRESS;
				
				display_ctor(disp->right, menu_type, disp, NULL, NULL, disp->back, NULL);
				break;
			}
			}
	
		
		screen_clear();		//clear screen
		Cursor_enum temp1 = SEC_1;  
		screen_set_cursor(temp1);	//set cursor on screen sec 1
		screen_write_txt(&(disp->characters[0][0]), strlen(disp->characters[0]));	//writes the first line of characters to screen
		
		if((disp->menu_type >> 4)> 0)	//writes second line of characters to screen
			screen_write_txt_line_2(&(disp->characters[1][0]),strlen(disp->characters[1]));
			
		global_current_display(disp);	//updates current screen object
	}
}


//------------------------------------------------------------------------------------------------------

void s1_left(Display *disp, Menu_enum menu_type, uint16_t value){
	if( menu_type != HOME ) {
		Display* oldcurrent = disp;
		Display* oldright = disp->right;
		Display* oldleft = disp->left;
		Display* oldselect = disp->select;
		Display* oldback = disp->back;
		
		
		disp = oldleft;
		disp->left = oldright;
		disp->right = oldcurrent;
		disp->back = oldback;
		disp->select = oldselect;
		
		
		switch( (int)(menu_type >> 4) ) {
			case 0: {
				display_ctor(disp->left, HOME, NULL, NULL, disp->select, disp->back,NULL);
				break;
			}
			case 1: {
				if( menu_type > 0x10 ) {
					disp->menu_type = --menu_type;
				}
				else {
					disp->menu_type = VOLUME;
					menu_type = VOLUME;
				}
				if( menu_type > 0x10)
					menu_type--;
				else menu_type = VOLUME;
				
				display_ctor(disp->left, menu_type, NULL, disp, NULL, disp->back, NULL);
				break;
			}
		}
		
		screen_clear();
		Cursor_enum temp1 = SEC_1;
		screen_set_cursor(temp1);
		screen_write_txt(&(disp->characters[0][0]), strlen(disp->characters[0]));
		if((disp->menu_type >> 4) > 0)  //writes second lin of charaters to screen
			screen_write_txt_line_2(&(disp->characters[1][0]),strlen(disp->characters[1]));
		
		global_current_display(disp);
		
	}
}

//------------------------------------------------------------------------------------------------------

void s1_select(Display *disp, Menu_enum menu_type, uint16_t value){
	
	if( (menu_type >> 4) < 1) {	
		Display* oldcurrent = disp;
		Display* oldright = disp->right;
		Display* oldleft = disp->left;
		Display* oldselect = disp->select;
		Display* oldback = 	disp->back;
		
		disp = oldselect;
		disp->back = oldcurrent;
		disp->select = oldback;
		disp->left = oldleft;
		disp->right = oldright;

		display_ctor(disp,IP_ADDRESS,disp->left,disp->right,disp->select,disp->back,NULL);
		display_ctor(disp->left, VOLUME, NULL, NULL, NULL, disp, NULL);
		display_ctor(disp->right, SUBNETMASK, NULL, NULL, NULL, disp, NULL);

		screen_clear();
		Cursor_enum temp1 = SEC_1;
		screen_set_cursor(temp1);
		screen_write_txt(&(disp->characters[0][0]), strlen(disp->characters[0]));
		
		if((disp->menu_type >> 4) > 0) 
			screen_write_txt_line_2(&(disp->characters[1][0]),strlen(disp->characters[1]));
		
		global_current_display(disp);
	}
	else {
		disp->menu_type = (Menu_enum)(disp->menu_type + 0x10);
		
		if( (disp->menu_type & 0x0F) != 3 ) {
	 		Cursor_enum temp1 = SEC_3;
	 		screen_set_cursor(temp1);
	 		screen_underline_on();
			disp->i = 0;
		}
		else {
			display_set_text(disp,"Select a Channel", 16);
			disp->i = 1;
			Io_enum temp_io = OUTPUT;
			Channel* temp_channel = get_channel_from_memory(temp_io, disp->i);
			display_set_text_line_2(disp, channel_get_name(temp_channel), 16);
 			Cursor_enum temp1 = SEC_1;
			screen_set_cursor(temp1);
			screen_write_txt( &(disp->characters[0][0]), 16 );
			screen_write_txt_line_2( &(disp->characters[1][0]), strlen(disp->characters[1]) );
		}
	
	}
}


//------------------------------------------------------------------------------------------------------


void s1_back(Display *disp, Menu_enum menu_type, uint16_t value){
	if( menu_type != HOME ) {
		Display* oldcurrent = disp;
		Display* oldright = disp->right;
		Display* oldleft = disp->left;
		Display* oldselect = disp->select;
		Display* oldback = disp->back;
		
		disp = oldback;
		disp->select = oldcurrent;
		disp->back = oldselect;
		disp->left = oldleft;
		disp->right = oldright;
		
		switch( (int)(menu_type >> 4) ) {
			case 0: {
				display_ctor(disp->back, HOME, NULL, NULL, disp->select, NULL, NULL);
				break;
			}
			
			case 1: {
				if( menu_type == HOME ) {
					display_ctor(disp->back, HOME, disp->left, disp->right, disp, NULL, NULL);
				}
				
				else display_ctor(disp->back, HOME, disp->left, disp->right, disp, NULL, NULL);	
			break;
			}
		}
		
		screen_clear();
		Cursor_enum temp1 = SEC_1;
		screen_set_cursor(temp1);
		screen_write_txt(&(disp->characters[0][0]), strlen(disp->characters[0]));
		
		if((disp->menu_type >> 4) > 0) 
			screen_write_txt_line_2(&(disp->characters[1][0]),strlen(disp->characters[1]));
		
		global_current_display(disp);
		
	}	
}

//------------------------------------------------------------------------------------------------------

void display_ctor(Display* disp, Menu_enum menu_type, Display* left, Display* right,
	Display* select, Display* back, uint16_t value) {
		
	disp->menu_type = menu_type;
	disp->left = left;
	disp->right = right;
	disp->select = select;
	disp->back = back;
	disp->i = value;
	
	switch(menu_type){
		case HOME: {
			display_set_text(disp, HOME_TEXT, 16);
			break;}
		
		case IP_ADDRESS: {
			Address* addr = global_get_address();
			display_set_text(disp, IP_TEXT, 16);
			display_set_text_line_2(disp,addr->ip,16);
			break;}
			
		case SUBNETMASK: {
			Address* addr = global_get_address();
			display_set_text(disp, SUBNETMASK_TEXT, 16);
			display_set_text_line_2(disp,addr->subnet,16);
			break;}
			
		case GATEWAY: {
			Address* addr = global_get_address();
			display_set_text(disp, GATEWAY_TEXT, 16);
			display_set_text_line_2(disp,addr->gateway,16);
			break;
		}
		case VOLUME: {
			Volume* vol = global_get_volume();
			display_set_text(disp,VOLUME_TEXT, 16);
			display_set_text_line_2(disp," ",16);
			break;	
		}
	}
}


//------------------------------------------------------------------------------------------------------

void display_set_text(Display* disp, char* chars, uint8_t length) {
	//printf("disp pointer: %ld, char ptr: %ld, length: %d",disp, chars, length);
	strncpy(&(disp->characters[0][0]), chars, length);
}

//------------------------------------------------------------------------------------------------------

void display_set_text_line_2(Display* disp, char* chars, uint8_t length){
	strncpy(&(disp->characters[1][0]), chars, length);	
}

//------------------------------------------------------------------------------------------------------

char numberarray[10] = "0123456789";

//------------------------------------------------------------------------------------------------------


void ChangeAddress(Display *disp) {
	Address* addr = global_get_address();
	
	switch( (int)(disp->menu_type & 0x0F) ) {
		case 0: {
			strncpy( &(addr->ip[0]), &(disp->characters[1][0]), 16 );
			break;
		}
		case 1: {
			strncpy( &(addr->subnet[0]), &(disp->characters[1][0]), 16 );
			break;
		}
		case 2: {
			strncpy( &(addr->gateway[0]), &(disp->characters[1][0]), 16 );
			break;
		}
	}
// TODO: INSERT FUNCTION TO ACTUALLY CHANGE ADDRESS HERE Optional may have made change in backchange

}

//------------------------------------------------------------------------------------------------------


void change_volume(Display *disp,int direction) {
	long i;
	int length;
	Volume* vol = global_get_volume();
	if(direction == 0) {
			if(disp->i == 1) {
				if(vol->l_level != 0) {
					i = vol->l_level + 1;
					if(i == 0) length = 1;
					else if( (i > -10) && (i != 0 ) ) length = 2;
					else if( (i > -100) && (i <= -10) ) length = 3;
					else length = 4;
					vol->l_level = i;
					ltoa(vol->l_level,vol->left_level);
					screen_set_cursor(SEC_4);
					screen_write_txt(vol->left_level, length);
					screen_write_txt(" dB   ", 6);
				}
			}
			else if(disp->i == 2) {
					if(vol->r_level !=0) {
						i = vol->r_level + 1;
						if(i == 0) length = 1;
						else if( (i > -10) && (i != 0 ) ) length = 2;
						else if( (i > -100) && (i <= -10) ) length = 3;
						else length = 4;
						vol->r_level = i;
						ltoa(vol->r_level,vol->right_level);
						screen_set_cursor(SEC_4);
						screen_write_txt(vol->right_level,length);
						screen_write_txt(" dB   ", 6);
					}
				}
	}
	else {
		if(disp->i == 1) {
			if(vol->l_level != -127) {
				i = vol->l_level - 1;
				if(i == 0) length = 1;
				else if( (i > -10) && (i != 0 ) ) length = 2;
				else if( (i > -100) && (i <= -10) ) length = 3;
				else length = 4;
				vol->l_level = i;
				ltoa(vol->l_level,vol->left_level);
				screen_set_cursor(SEC_4);
				screen_write_txt(vol->left_level, length);
				screen_write_txt(" dB   ", 6);
			}
		}
		else if(disp->i == 2) {
			if(vol->r_level != -127) {
				i = vol->r_level - 1;
				if(i == 0) length = 1;
				else if( (i > -10) && (i != 0 ) ) length = 2;
				else if( (i > -100) && (i <= -10) ) length = 3;
				else length = 4;
				vol->r_level = i;
				ltoa(vol->r_level,vol->right_level);
				screen_set_cursor(SEC_4);
				screen_write_txt(vol->right_level,length);
				screen_write_txt(" dB   ", 6);
			}
		}
	}
	
// TODO: INSERT FUNCTION TO ACTUALLY CHANGE VOLUME HERE	
	
}



//------------------------------------------------------------------------------------------------------

void right_change(Display *disp) {
	if( (disp->menu_type & 0x0F) != 3 ) {
		switch( (int)(disp->menu_type >> 4) ) {
			case 2: {
				switch(disp->i) {
					case 2: {
						screen_move_cursor_right();
						screen_move_cursor_right();
						disp->i++;
						disp->i++;
						break;
					}
					case 6: {
						screen_move_cursor_right();
						screen_move_cursor_right();
						disp->i++;
						disp->i++;
						break;
					}
					case 10: {
						screen_move_cursor_right();
						screen_move_cursor_right();
						disp->i++;
						disp->i++;
						break;
					}								
					case 14: {
						Cursor_enum loc = SEC_3;
						screen_set_cursor(loc);
						disp->i = 0;
						break;
					}				
					default: {
						screen_move_cursor_right();
						disp->i++;
						break;
					}					
				}
				break;
			}
			
			case 3: {
				int j, i, k, temp;
				char char1, char2;
			
				j = disp->i;
				
				for(k = 0; k < 10; k++) {
					char1 = disp->characters[1][j];
					char2 = numberarray[k];
					
					if( char1 == char2 )
						temp = k;
				}
				
				if(temp == 9)
					strncpy( &(disp->characters[1][j]), &(numberarray[0]), 1);
				else
					strncpy( &(disp->characters[1][j]), &(numberarray[temp+1]),1);
				
				screen_write_txt_line_2(&(disp->characters[1][0]),strlen(disp->characters[1]));
				screen_set_cursor(SEC_3);
				
				for(i = 0; i<j; i++) {
				screen_move_cursor_right();
				}
				break;
			}
		}
	}
	else {
		switch( (int)(disp->menu_type >> 4) ) {
			case 2: {
				if(disp->i < 2){
					disp->i++;
					Io_enum temp_io = OUTPUT;
					Channel* temp_channel = get_channel_from_memory(temp_io, disp->i);
					display_set_text_line_2(disp, channel_get_name(temp_channel), 16);
					screen_write_txt_line_2( &(disp->characters[1][0]), strlen(disp->characters[1]) );
				}
				else{
					disp->i = 1;
					Io_enum temp_io = OUTPUT;
					Channel* temp_channel = get_channel_from_memory(temp_io, disp->i);
					display_set_text_line_2(disp, channel_get_name(temp_channel), 16);
					screen_write_txt_line_2( &(disp->characters[1][0]), strlen(disp->characters[1]) );
				}
				break;	
			}
			case 3: {
				change_volume(disp,0);
				break;
			}				
		}
	}
}

//------------------------------------------------------------------------------------------------------

void left_change(Display *disp) {
	if( (disp->menu_type & 0x0F) != 3 ) {
		switch( (int)(disp->menu_type >> 4) ) {
			case 2: {			
				switch(disp->i) {
					case 4: {
						screen_move_cursor_left();
						screen_move_cursor_left();
						disp->i--;
						disp->i--;
						break;
					}
					case 8: {
						screen_move_cursor_left();
						screen_move_cursor_left();
						disp->i--;
						disp->i--;
						break;
					}
					case 12: {
						screen_move_cursor_left();
						screen_move_cursor_left();
						disp->i--;
						disp->i--;
						break;
					}								
					case 0: {
						Cursor_enum loc = LINE2END_M1;
						screen_set_cursor(loc);
						disp->i = 14;
						break;
					}				
					default: {
						screen_move_cursor_left();
						disp->i--;
						break;
					}					
				}
			break;
			}
			case 3: {
				int j, i, k, temp;
				char char1, char2;
			
				j = disp->i;
				
				for(k = 0; k < 10; k++) {
					char1 = disp->characters[1][j];
					char2 = numberarray[k];
					
					if( char1 == char2 )
						temp = k;
				}
				
				if(temp == 0)
					strncpy( &(disp->characters[1][j]), &(numberarray[9]), 1);
				else
					strncpy( &(disp->characters[1][j]), &(numberarray[temp-1]),1);
				
				screen_write_txt_line_2(&(disp->characters[1][0]),strlen(disp->characters[1]));
				screen_set_cursor(SEC_3);
				
				for(i = 0; i<j; i++) {
				screen_move_cursor_right();
				}
			break;
			}
		}
	}
	else {
		switch( (int)(disp->menu_type >> 4) ) {
			case 2: {
				if(disp->i > 1){
					disp->i--;
					Io_enum temp_io = OUTPUT;
					Channel* temp_channel = get_channel_from_memory(temp_io, disp->i);
					display_set_text_line_2(disp, channel_get_name(temp_channel), 16);
					screen_write_txt_line_2( &(disp->characters[1][0]), strlen(disp->characters[1]) );
				}
				else{
					disp->i = 2;
					Io_enum temp_io = OUTPUT;
					Channel* temp_channel = get_channel_from_memory(temp_io, disp->i);
					display_set_text_line_2(disp, channel_get_name(temp_channel), 16);
					screen_write_txt_line_2( &(disp->characters[1][0]), strlen(disp->characters[1]) );
				}
				break;	
			}
			case 3: {
				change_volume(disp,1);
				break;
			}				
		}
	}
}

//------------------------------------------------------------------------------------------------------

void select_change(Display *disp){
	if( (disp->menu_type & 0x0F) != 3 ) {
		switch( (int)(disp->menu_type >> 4) ) {
			case 2: {
				screen_blink_cursor_on();
				screen_underline_off();
				disp->menu_type = (Menu_enum)(disp->menu_type + 0x10);
				break;
			}
			
			case 3: {
				screen_blink_cursor_off();
				screen_underline_on();
				ChangeAddress(disp);
				disp->menu_type = (Menu_enum)(disp->menu_type - 0x10);
				break;
			}
		}
	}
	else {
		switch( (int)(disp->menu_type >> 4) ) {
			case 2: {
				display_set_text(disp, &(disp->characters[1][0]), 16);
				Volume* vol = global_get_volume();
				display_set_text_line_2(disp, "Volume: ", 16);
				screen_set_cursor(SEC_1);
				screen_clear();
				screen_write_txt( &(disp->characters[0][0]), strlen(disp->characters[0]) );
				screen_write_txt_line_2( &(disp->characters[1][0]), strlen(disp->characters[0]) );
				screen_set_cursor(SEC_4);
				if( disp->i == 1 ){
					screen_write_txt( vol->left_level, strlen(vol->left_level) );
					screen_write_txt( " dB", 3);
				}
				else {
					screen_write_txt( vol->right_level, strlen(vol->right_level) );
					screen_write_txt( " dB", 3);
				}
				
				disp->menu_type = (Menu_enum)(disp->menu_type + 0x10);
				break;
				
			}
			case 3: {
				screen_clear();
				display_set_text(disp, "Volume", 16);
				display_set_text_line_2(disp, " ", 16);
				screen_write_txt(&(disp->characters[0][0]) , strlen(disp->characters[0]) );
				disp->menu_type = VOLUME;
				break;
				
			}				
		}
	}
}

//------------------------------------------------------------------------------------------------------

void back_change(Display *disp){
	if( (disp->menu_type & 0x0F) != 3 ) {
		switch( (int)(disp->menu_type >> 4) ) {
			case 2: {
				screen_underline_off();
				disp->menu_type = (Menu_enum)(disp->menu_type - 0x10);
				actual_change_address();
				break;
			}
			
			case 3: {
				int i, j;
				j = disp->i;
				
				Address* addr = global_get_address();
				
				switch( (int)(disp->menu_type & 0x0F) ) {
					case 0: {
						strncpy( &(disp->characters[1][0]), addr->ip, 16 );
						break;
					}
					case 1: {
						strncpy( &(disp->characters[1][0]), addr->subnet, 16 );
						break;
					}
					case 2: {
						strncpy( &(disp->characters[1][0]), addr->gateway, 16 );
						break;
					}
				}
				screen_write_txt_line_2(&(disp->characters[1][0]),strlen(disp->characters[1]));
				screen_set_cursor(SEC_3);
				for(i = 0; i<j; i++) {
				screen_move_cursor_right();
				}
				
				screen_blink_cursor_off();
				screen_underline_on();
				disp->menu_type = (Menu_enum)(disp->menu_type - 0x10);
				break;
			}
		}
	}
	else {
		switch( (int)(disp->menu_type >> 4) ) {
			case 2: {
				display_set_text(disp, "Volume", 16);
				display_set_text_line_2(disp, " ", 16);
				screen_clear();
				screen_write_txt(&(disp->characters[0][0]) , strlen(disp->characters[0]) );
				disp->menu_type = VOLUME;
				break;
			}
			case 3: {
				screen_clear();
				display_set_text(disp,"Select a Channel", 16);
				Io_enum temp_io = OUTPUT;
				Channel* temp_channel = get_channel_from_memory(temp_io, disp->i);
				display_set_text_line_2(disp, channel_get_name(temp_channel), 16);
				screen_set_cursor(SEC_1);
				screen_write_txt( &(disp->characters[0][0]), 16 );
				screen_write_txt_line_2( &(disp->characters[1][0]), strlen(disp->characters[1]) );
				disp->menu_type = (Menu_enum)(disp->menu_type - 0x10);
				break;
			}
		}
	}
}
//------------------------------------------------------------------------------------------------------

void shift_address(char* addr, int start, int length) {
	char temp[16] = 0;
	char zero = '0';
	
	switch(start) {
		case 1: {
			memcpy(&(temp[length]), addr, strlen(addr)+1 );
			
			if(length == 1) {
				strncpy(&(temp[0]),&zero,1);
			}
			else if(length == 2) {
				strncpy(&(temp[0]),&zero,1);
				strncpy(&(temp[1]),&zero,1);
			}
			memcpy(addr , &(temp[0]), strlen(temp));
			break;
		}
		case 2: {
			
			memcpy(&(temp[4+length]), &(addr[4]), strlen(&(addr[4])) );
			if(length == 1) {
				strncpy(&(temp[4]),&zero,1);
			}
			else if(length == 2) {
				strncpy(&(temp[4]),&zero,1);
				strncpy(&(temp[5]),&zero,1);
			}
			memcpy(&(addr[4]), &(temp[4]), strlen(&(temp[4])) );
			break;
		}
		case 3: {
			memcpy( &(temp[8+length]),&(addr[8]), strlen(&(addr[8])) );

			if(length == 1) {
				strncpy(&(temp[8]),&zero,1);
			}
			else if(length == 2) {
				strncpy(&(temp[8]),&zero,1);
				strncpy(&(temp[9]),&zero,1);
			}
			memcpy(&(addr[8]), &(temp[8]), strlen(&(temp[8])));
			break;
		}
		case 4: {
			memcpy(&(temp[12+length]), &(addr[12]), strlen(&(addr[12])) );
			
			if(length == 1) {
				strncpy(&(temp[12]),&zero,1);
			}
			else if(length == 2) {
				strncpy(&(temp[12]),&zero,1);
				strncpy(&(temp[13]),&zero,1);
			}
			memcpy(&(addr[12]), &(temp[12]), strlen(&(temp[12])) );
			break;
		}
	}	
}

//------------------------------------------------------------------------------------------------------
void check_address(char* addr) {
	char point = '.';
	char end = 0;
	int i;
	
	
	for(i = 1; i != 3; i++) {
		if( addr[i] == point ) {
			switch(i) {
				case 1: {
					shift_address(addr, 1, 2);
					break;
				}
				case 2: {
					shift_address(addr, 1, 1);
					break;
				}
			}
		i = 2;
		}	
	}
	for(i = 5; i != 7; i++) {
		if( addr[i] == point ) {
			switch(i) {
				case 5: {
					shift_address(addr, 2, 2);
					break;
				}
				case 6: {
					shift_address(addr, 2, 1);
					break;
				}	
			}
		i = 6;
		}
	}
	for(i = 9; i != 11; i++) {
		if( addr[i] == point ) {
			switch(i) {
				case 9: {
					shift_address(addr, 3, 2);
					break;
				}
				case 10: {
					shift_address(addr, 3, 1);
					break;
				}
			}
		i = 10;
		}	
	}
	for(i = 13; i != 15; i++) {
		if( addr[i] == end ) {
			switch(i) {
				case 13: {
					shift_address(addr, 4, 2);
					break;
				}
				case 14: {
					shift_address(addr, 4, 1);
					break;
				}	
			}	
		i = 14;
		}
	}		
}



//------------------------------------------------------------------------------------------------------

void delaymycode(uint16_t delaytime) {
	uint16_t i = 0;
	
	while(i < delaytime){
		SysCtlDelay(800000);  //100ms. argument of 100 is 2uS
		i++;
	}
	return;
}

