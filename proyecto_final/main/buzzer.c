#include "buzzer.h"

void delay10Us(){
    *TIM_CONFIG0 |= 80 << 13;  //Configuracion del divisor para 1 us

    if (*TIM_CONFIG0 & 1 << 13){
        *TIM_CONFIG0 ^= 1 << 13;
    }

    *TIM_CONFIG0 |= 1 << 31;  //Configuracion para habiliar el timer
    
    *TIM_ALARMLO0 = 10;     //Tiempo en el que termina en medios milisegundos (10 us)
    *TIM_CONFIG0 |= 1 << 10;  //Configuracion para habilitar la alarma

    while ((*TIM_CONFIG0 & 1 << 10) != 0){}  //Cicla hasta que se desactive la alarma
}