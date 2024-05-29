#include "buzzer.h"

volatile uint32_t *TI_CONFIG0  = (uint32_t *) 0x3FF5F000;
volatile uint32_t *TI_ALARMLO0 = (uint32_t *) 0x3FF5F010;

void delay10Us(){
    *TI_CONFIG0 |= 80 << 13;  //Configuracion del divisor para 1 us

    if (*TI_CONFIG0 & 1 << 13){
        *TI_CONFIG0 ^= 1 << 13;
    }

    *TI_CONFIG0 |= 1 << 31;  //Configuracion para habiliar el timer
    
    *TI_ALARMLO0 = 10;     //Tiempo en el que termina en medios milisegundos (10 us)
    *TI_CONFIG0 |= 1 << 10;  //Configuracion para habilitar la alarma

    while ((*TI_CONFIG0 & 1 << 10) != 0){}  //Cicla hasta que se desactive la alarma
}