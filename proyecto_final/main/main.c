#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

void app_main(void)
{
    gpio_reset_pin(26);                                  //sensor infrarojo
    gpio_set_direction(26, GPIO_MODE_INPUT);

    while (1){
        printf("%d\n", gpio_get_level(26));              //Usualmente esta en alto, cuando se activa el sensor baja
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    
}
