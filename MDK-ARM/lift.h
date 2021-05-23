#ifndef __LIFT_H
#define __LIFT_H

#include <stdint.h>
#include "doubleTapHandler.h"

#define TOP_FLOOR 9
#define BOTTOM_FLOOR 0

void initLift(void);
void initPwmSwitch(void);
	
void liftUp (void);
void liftDown(void);

void stopMotor(void);

void updateFloor(tapActionEnum tapAction);
uint16_t getFloor(void);

#endif
