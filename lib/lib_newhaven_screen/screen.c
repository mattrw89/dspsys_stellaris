#include "screen.h"



//-------------------------------------------------------------------------------------------------------------------------------------------

void screen_on(){
	Screen_enum screen = SCRN_ON;
	char cmd[2] = {COMMAND,screen} ;
	I2CSendData(SCREEN_ADDR, &(cmd[0]), sizeof(cmd));
}

//-------------------------------------------------------------------------------------------------------------------------------------------

void screen_off(){
	Screen_enum screen = SCRN_OFF;
	char cmd[2] = {COMMAND,screen} ;
	I2CSendData(SCREEN_ADDR, &(cmd[0]), sizeof(cmd));
}

//-------------------------------------------------------------------------------------------------------------------------------------------

void screen_set_cursor(Cursor_enum loc){
	Screen_enum screen = SCRN_SET_CURSOR;
	char cmd[3] = {COMMAND,screen,loc};
	I2CSendData(SCREEN_ADDR, &(cmd[0]), sizeof(cmd));
}

//-------------------------------------------------------------------------------------------------------------------------------------------

void screen_write_txt(char *ptr_txt, uint8_t length){
	I2CSendData(SCREEN_ADDR, ptr_txt,length);
}

//-------------------------------------------------------------------------------------------------------------------------------------------

void screen_write_txt_line_2(char *ptr_txt, uint8_t length){
	screen_set_cursor(SEC_3);
	I2CSendData(SCREEN_ADDR, ptr_txt,length);
}

//-------------------------------------------------------------------------------------------------------------------------------------------

void screen_write_txt_sec_2(char *ptr_txt, uint8_t length){
	screen_set_cursor(SEC_2);
	I2CSendData(SCREEN_ADDR, ptr_txt,length);
}

//-------------------------------------------------------------------------------------------------------------------------------------------

void screen_clear(){
	Screen_enum screen = SCRN_CLEAR;
	char cmd[2] = {COMMAND,screen};
	I2CSendData(SCREEN_ADDR,  &(cmd[0]), sizeof(cmd));
}
