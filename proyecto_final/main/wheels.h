#ifndef WHEELS_H
#define WHEELS_H

#include "driver/gpio.h"

void initWheels();
void wheelsGoFoward();
void wheelsGoBackward();
void wheelsStop();
void wheelsTurnClockwise();
void wheelsTurnCounterClockwise();

#endif