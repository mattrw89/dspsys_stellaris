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

void screen_underline_on() {
	Screen_enum screen = SCRN_UNDER_ON;
	char cmd[2] = {COMMAND,screen};
	I2CSendData(SCREEN_ADDR, &(cmd[0]), sizeof(cmd)); 
}
//-------------------------------------------------------------------------------------------------------------------------------------------

void screen_underline_off() {
	Screen_enum screen = SCRN_UNDER_OFF;
	char cmd[2] = {COMMAND,screen};
	I2CSendData(SCREEN_ADDR, &(cmd[0]), sizeof(cmd));
}

//-------------------------------------------------------------------------------------------------------------------------------------------
void screen_move_cursor_right() {
	Screen_enum screen = MOVE_C_RIGHT;
	char cmd[2] = {COMMAND,screen};
	I2CSendData(SCREEN_ADDR, &(cmd[0]), sizeof(cmd));
}
//-------------------------------------------------------------------------------------------------------------------------------------------

void screen_move_cursor_left() {
	Screen_enum screen = MOVE_C_LEFT;
	char cmd[2] = {COMMAND,screen};
	I2CSendData(SCREEN_ADDR, &(cmd[0]), sizeof(cmd));
}

//-------------------------------------------------------------------------------------------------------------------------------------------


void screen_blink_cursor_on() {
	Screen_enum screen =SCRN_BLINK_ON;
	char cmd[2] = {COMMAND,screen};
	I2CSendData(SCREEN_ADDR, &(cmd[0]), sizeof(cmd));
}

//-------------------------------------------------------------------------------------------------------------------------------------------


void screen_blink_cursor_off() {
	Screen_enum screen = SCRN_BLINK_OFF;
	char cmd[2] = {COMMAND,screen};
	I2CSendData(SCREEN_ADDR, &(cmd[0]), sizeof(cmd));
}

//-------------------------------------------------------------------------------------------------------------------------------------------

void screen_clear(){
	Screen_enum screen = SCRN_CLEAR;
	char cmd[2] = {COMMAND,screen};
	I2CSendData(SCREEN_ADDR,  &(cmd[0]), sizeof(cmd));
}