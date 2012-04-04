#include "ints.h"

void turn_encoder_right() {
	//printf("turn encoder right\n\n");
	Display* disp = global_current_display(0);
	(*(disp->func_right))(disp, disp->menu_type, NULL);
}

void turn_encoder_left() {
	//printf("turn encoder left\n\n");
	Display *disp = global_current_display(0);
	(*(disp->func_left))(disp, disp->menu_type, NULL);
}

void push_encoder_button() {
	//printf("push encoder button\n\n");
	Display *disp = global_current_display(0);
	(*(disp->func_select))(disp, disp->menu_type, NULL);
}

void push_back_button() {
	//printf("push back button\n\n");
	Display *disp = global_current_display(0);
	(*(disp->func_back))(disp, disp->menu_type, NULL);
}

void turn_encoder_right_change() {
	Display *disp = global_current_display(0);
	right_change(disp);
}

void turn_encoder_left_change() {
	Display *disp = global_current_display(0);
	left_change(disp);
}

void push_encoder_button_change() {
	Display *disp = global_current_display(0);
	select_change(disp);
}

void push_back_button_change() {
	Display *disp = global_current_display(0);
	back_change(disp);
}

