#ifndef BUZZER_H
#define BUZZER_H

#include "driver/gpio.h"
#include "driver/ledc.h"

#define BUZZER_GPIO_PIN 19          // Definir el pin GPIO donde está conectado el buzzer
#define LEDC_CHANNEL_0 0            // Definir el canal de LEDC
#define BUZZER_FREQUENCY 2000       // Definir la frecuencia de la señal (en Hz)
#define LEDC_TIMER_0 0              // Definir el timer de LEDC
#define LEDC_DUTY_RES LEDC_TIMER_13_BIT // Definir la resolución del duty cycle

#define LED1_GPIO_PIN 26

#define LED2_GPIO_PIN 17

void buzzer_play(uint32_t *time);

#endif