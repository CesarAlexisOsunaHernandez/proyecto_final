#include "buzzer.h"

void buzzerInit(){// Configurar el timer de LEDC
    
}


void buzzer_play(uint32_t *time){
    // Configurar el timer de LEDC
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_DUTY_RES,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = BUZZER_FREQUENCY,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ledc_timer_config(&ledc_timer);

    // Configurar el canal de LEDC
    ledc_channel_config_t ledc_channel = {
        .gpio_num = BUZZER_GPIO_PIN,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0,
        .hpoint = 0
    };
    ledc_channel_config(&ledc_channel);

    if(*time < 50){
        // Activar el buzzer
        ledc_set_duty(ledc_channel.speed_mode, ledc_channel.channel, 4096); // 50% duty cycle
        ledc_update_duty(ledc_channel.speed_mode, ledc_channel.channel);
        gpio_set_level(LED1_GPIO_PIN, 1);
        gpio_set_level(LED2_GPIO_PIN, 0);
    } // Sonar por 500ms

    if(*time > 50 && *time < 100){
        // Desactivar el buzzer
        ledc_set_duty(ledc_channel.speed_mode, ledc_channel.channel, 0);
        ledc_update_duty(ledc_channel.speed_mode, ledc_channel.channel);
        gpio_set_level(LED1_GPIO_PIN, 0);
        gpio_set_level(LED2_GPIO_PIN, 1);
        } // Pausa por 500ms

    if(*time >= 100)
        *time = 0;
    
}