#include "wheels.h"
#define MOTOR2_1 4
#define MOTOR2_2 15
#define MOTOR1_1 33
#define MOTOR1_2 32
/**
 * @brief Inicializa los pines necesarios para el funcionamiento de las llantas.
 */
void initWheels(){
    //** PINES PARA CONTROLAR LAS RUEDAS **
    gpio_reset_pin(MOTOR1_1);                               //rueda izq derecho
    gpio_set_direction(MOTOR1_1, GPIO_MODE_OUTPUT);

    gpio_reset_pin(MOTOR1_2);                               //rueda izq reversa
    gpio_set_direction(MOTOR1_2, GPIO_MODE_OUTPUT);

    gpio_reset_pin(MOTOR2_1);                                //rueda der derecho
    gpio_set_direction(MOTOR2_1, GPIO_MODE_OUTPUT);

    gpio_reset_pin(MOTOR2_2);                               //rueda der reversa
    gpio_set_direction(MOTOR2_2, GPIO_MODE_OUTPUT);

    wheelsStop();

    printf("** Llantas iniciadas con exito **\n");
}

/**
 * @brief El carro se mueve hacia adelante.
 */
void wheelsGoFoward(){
    gpio_set_level(MOTOR1_1, 1);
    gpio_set_level(MOTOR1_2, 0);
    gpio_set_level(MOTOR2_1, 1);
    gpio_set_level(MOTOR2_2, 0);
    printf("** FOWARD **\n");
}

/**
 * @brief El carro se mueve hacia atras.
 */
void wheelsGoBackward(){
    gpio_set_level(MOTOR1_1, 0);
    gpio_set_level(MOTOR1_2, 1);
    gpio_set_level(MOTOR2_1, 0);
    gpio_set_level(MOTOR2_2, 1);
    printf("** BACKWARD **\n");
}

/**
 * @brief El carro se deja de mover.
 */
void wheelsStop(){
    gpio_set_level(MOTOR1_1, 0);
    gpio_set_level(MOTOR1_2, 0);
    gpio_set_level(MOTOR2_1, 0);
    gpio_set_level(MOTOR2_2, 0);
    printf("** STOP **\n");
}

/**
 * @brief El carro rota en el sentido de la manecillas del reloj.
 */
void wheelsTurnClockwise(){
    gpio_set_level(MOTOR1_1, 1);
    gpio_set_level(MOTOR1_2, 0);
    gpio_set_level(MOTOR2_1, 0);
    gpio_set_level(MOTOR2_2, 1);
    printf("** TURN CLOCKWISE **\n");
}

/**
 * @brief El carro rota en el sentido OPUESTO de la manecillas del reloj.
 */
void wheelsTurnCounterClockwise(){
    gpio_set_level(MOTOR1_1, 0);
    gpio_set_level(MOTOR1_2, 1);
    gpio_set_level(MOTOR2_1, 1);
    gpio_set_level(MOTOR2_2, 0);
    printf("** TURN COUNTER CLOCKWISE **\n");
}