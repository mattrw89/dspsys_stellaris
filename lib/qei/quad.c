#include "quad.h"
#include "../../ints.h"

void quad_encoder() {
	IntDisable(INT_GPIOC);
	long temp;
	
	temp = GPIOPinIntStatus( GPIO_PORTC_BASE, true);

	
	Display *disp = global_current_display(0);
	
	if( (disp->menu_type >> 4) < 2 ) {
		if(temp & 0x20) {
			push_encoder_button();
			delaymycode(10);
		}
		
		else if(temp & 0x80) {
			push_back_button();
			delaymycode(10);
		
		}
		
		else if( (temp & 0x10) || (temp & 0x40) ) {
			unsigned char curA = GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_4);
			unsigned char curB = GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_6);
			Encoder_State* encoder = global_get_encoder();
			//(curA == 0)&&(curA != encoder->encoderA_last)&&(curB == encoder->encoderB_last)
			if( (curB) ) {
				
				turn_encoder_left();
				delaymycode(1);
				encoder->encoderA_last = curA;
				encoder->encoderB_last = curB;
				
			}
			//(curB == 0)&&(curB != encoder->encoderB_last)&&(curA == encoder->encoderA_last)
			else if( (~(encoder->encoderA_last ^ curB)) ) {
				turn_encoder_right();
				delaymycode(1);	
				encoder->encoderA_last = curA;
				encoder->encoderB_last = curB;
			}
		}	
	}
	
	else if ( (disp->menu_type >> 4) >= 2 ) {
		if(temp & 0x20) {
			push_encoder_button_change();
			delaymycode(10);
		}
		
		else if(temp & 0x80) {
			push_back_button_change();
			delaymycode(10);
		
		}
		
		else if( (temp & 0x10) || (temp & 0x40) ) {
			unsigned char curA = GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_4);
			unsigned char curB = GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_6);
			Encoder_State* encoder = global_get_encoder();
			
			//(curA == 0)&&(curA != encoder->encoderA_last)&&(curB == encoder->encoderB_last)
			if( (curB) ) {
					turn_encoder_left_change();
					delaymycode(1);
					encoder->encoderA_last = curA;
					encoder->encoderB_last = curB;
				
				
			}
			//(curB == 0)&&(curB != encoder->encoderB_last)&&(curA == encoder->encoderA_last)
			else if( (~(encoder->encoderA_last ^ curB)) ) {

					encoder->encoderA_last = curA;
					encoder->encoderB_last = curB;
					turn_encoder_right_change();
					delaymycode(1);	

			}
		}	
	}
	
		
	GPIOPinIntClear(INT_GPIOC,GPIO_PIN_5 | GPIO_PIN_4 | GPIO_PIN_6 | GPIO_PIN_7);
	
	IntEnable(INT_GPIOC);
}
