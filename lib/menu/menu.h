#ifndef MENU_H_
#define MENU_H_

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "../lib_newhaven_screen/screen.h"


#define HOME_TEXT	"SD Group 13"
#define IP_TEXT "IP Address" 
#define SUBNETMASK_TEXT "Subnet Mask"
#define GATEWAY_TEXT "Gateway"
#define VOLUME_TEXT "Volume"

typedef enum {
	HOME = 0x00,
	IP_ADDRESS = 0x10,
	SUBNETMASK = 0x11,
	GATEWAY = 0x12,
	VOLUME = 0x13,
	IP_ADDRESS_CHANGE = 0x20,
	SUBNETMASK_CHANGE = 0x21,
	GATEWAY_CHANGE = 0x22,
	VOLUME_CHANGE = 0x23,
	IP_ADDRESS_CHANGE_2 = 0x30,
	SUBNETMASK_CHANGE_2 = 0x31,
	GATEWAY_CHANGE_2 = 0x32,
	VOLUME_CHANGE_2 = 0x33
} Menu_enum;

typedef struct Display {
	Menu_enum menu_type;
	struct Display *left;
	struct Display *right;
	struct Display *select;
	struct Display *back;
	uint16_t i;
	void (*func_left)(struct Display*, Menu_enum, uint16_t);
	void (*func_right)(struct Display*,Menu_enum, uint16_t);
	void (*func_select)(struct Display*,Menu_enum, uint16_t);
	void (*func_back)(struct Display*,Menu_enum, uint16_t);
	char characters[2][17];
	
}Display;


//4 function that allow for movement through menu
void s1_right(Display *disp, Menu_enum menu_type, uint16_t value);

void s1_left(Display *disp, Menu_enum menu_type, uint16_t value);

void s1_select(Display *disp, Menu_enum menu_type, uint16_t value);

void s1_back(Display *disp, Menu_enum menu_type, uint16_t value);

// constructs a new menu object
void display_ctor(Display* disp, Menu_enum menu_type, Display *left, Display *right,
	Display *select, Display *back, uint16_t value);

void display_set_text(Display* disp, char* chars, uint8_t length);

void display_set_text_line_2(Display* disp, char* chars, uint8_t length);

//void
//SetIPAddress(unsigned long ipaddr, Display* disp);
void ChangeAddress(Display *disp);

void change_volume(Display *disp, int direction);

void right_change(Display *disp);

void left_change(Display *disp);

void select_change(Display *disp);

void back_change(Display *disp);

void shift_address(char* addr, int start, int length);

void check_address(char* addr);

void delaymycode(uint16_t delaytime);
#endif /*MENU_H_*/
