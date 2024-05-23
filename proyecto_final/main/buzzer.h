#ifndef BUZZER_H
#define BUZZER_H

#include "driver/gpio.h"

volatile uint32_t *TIM_CONFIG0  = (uint32_t *) 0x3FF5F000;
volatile uint32_t *TIM_ALARMLO0 = (uint32_t *) 0x3FF5F010;

void delay10Us();

#endif