#include "wheels.h"

/**
 * @brief Inicializa los pines necesarios para el funcionamiento de las llantas.
 */
void initWheels(){
    //** PINES PARA CONTROLAR LAS RUEDAS **
    gpio_reset_pin(12);                               //rueda izq derecho
    gpio_set_direction(12, GPIO_MODE_OUTPUT);

    gpio_reset_pin(13);                               //rueda izq reversa
    gpio_set_direction(13, GPIO_MODE_OUTPUT);

    gpio_reset_pin(4);                                //rueda der derecho
    gpio_set_direction(4, GPIO_MODE_OUTPUT);

    gpio_reset_pin(15);                               //rueda der reversa
    gpio_set_direction(15, GPIO_MODE_OUTPUT);

    printf("** Llantas iniciadas con exito **\n");
}

/**
 * @brief El carro se mueve hacia adelante.
 */
void wheelsGoFoward(){
    gpio_set_level(12, 1);
    gpio_set_level(13, 0);
    gpio_set_level(4, 1);
    gpio_set_level(15, 0);
    printf("** FOWARD **\n");
}

/**
 * @brief El carro se mueve hacia atras.
 */
void wheelsGoBackward(){
    gpio_set_level(12, 0);
    gpio_set_level(13, 1);
    gpio_set_level(4, 0);
    gpio_set_level(15, 1);
    printf("** BACKWARD **\n");
}

/**
 * @brief El carro se deja de mover.
 */
void wheelsStop(){
    gpio_set_level(12, 0);
    gpio_set_level(13, 0);
    gpio_set_level(4, 0);
    gpio_set_level(15, 0);
    printf("** STOP **\n");
}

/**
 * @brief El carro rota en el sentido de la manecillas del reloj.
 */
void wheelsTurnClockwise(){
    gpio_set_level(12, 1);
    gpio_set_level(13, 0);
    gpio_set_level(4, 0);
    gpio_set_level(15, 1);
    printf("** TURN CLOCKWISE **\n");
}

/**
 * @brief El carro rota en el sentido OPUESTO de la manecillas del reloj.
 */
void wheelsTurnCounterClockwise(){
    gpio_set_level(12, 0);
    gpio_set_level(13, 1);
    gpio_set_level(4, 1);
    gpio_set_level(15, 0);
    printf("** TURN COUNTER CLOCKWISE **\n");
}