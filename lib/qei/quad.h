#ifndef QEI_H_
#define QEI_H_

#include "inc/hw_ints.h"
#include "inc/hw_types.h"
#include "driverlib/interrupt.h"

typedef enum {
	right = 0,
	left = 1,
	select = 2,
	back = 3
} direction_enum;

typedef struct {
	
	unsigned char encoderA_last;
	unsigned char encoderB_last;
	direction_enum last_direction;
	
} Encoder_State;


void quad_encoder();

#endif /*QEI_H_*/
