#ifndef SCREEN_H_
#define SCREEN_H_

#include <stdint.h>
#include "../i2c/lib_I2C.h"


#define SCREEN_ADDR 0x50
#define COMMAND 0xFE

//Turns Screen Display on or off
//Variables:
//          Screen (show text): (on or off)
//          D_Options: (Address, shifting, clearing, baud rate...etc)

typedef enum {
	SCRN_ON = 0x41,
	SCRN_OFF = 0x42,
	SCRN_SET_CURSOR = 0x45,		//requires one byte after
	SCRN_UNDER_ON = 0x47,
	SCRN_UNDER_OFF = 0x48,
	SCRN_BLINK_ON = 0x4B,
	SCRN_BLINK_OFF = 0x4C,
	SCRN_BAUD_RATE = 0x71,
	SCRN_ADDRESS = 0x72,
	SCRN_VERSION = 0x70,
	SCRN_SHIFT_LEFT = 0x55,
	SCRN_SHIFT_RIGHT = 0x56,
	SCRN_CLEAR = 0x51,
	SCRN_BACKSPACE = 0x4E,
	MOVE_C_LEFT = 0x49,   // Move Cursor to the left 
	MOVE_C_RIGHT = 0x4A,  // Move cursor to the right
	CHANGE_BUAD = 0x61,
    CHANGE_ADDRESS = 0x62,
    CHANGE_CONTRAST = 0x52,
    CHANGE_BRIGHTNESS = 0x53
} Screen_enum;


//Set Cursor on Display screen
//Variables:
//        Sections or Text Location: (1, 2, 3, 4)
//        Move: (Left or Rght)


typedef enum {
	SEC_1 = 0x00,  //1st line 1st column
	SEC_2 = 0x08,  //1st line middle
	SEC_3 = 0x40,  //2nd line 1st column
	SEC_4 = 0x48,  //2nd line middle
	LINE2END_M1 = 0x4E //2nd line one space from last
} Cursor_enum;


void screen_on();
void screen_off();
void screen_write_txt(char *ptr_txt, uint8_t length);
void screen_write_txt_line_2(char *ptr_txt, uint8_t length);
void screen_write_txt_sec_2(char *ptr_txt, uint8_t length);
void screen_set_cursor(Cursor_enum loc);
void screen_underline_on();
void screen_underline_off();
void screen_move_cursor_right();
void screen_move_cursor_left();
void screen_blink_cursor_on();
void screen_blink_cursor_off();
void screen_clear();


#endif /*SCREEN_H_*/
