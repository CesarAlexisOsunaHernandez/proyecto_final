#ifndef MPU6050_H
#define MPU6050_H

#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "driver/i2c.h"

#define MPU6050_ADDR 0x68
#define MPU6050_WHO_AM_I_REG_ADDR 0x75
#define PWR_MGMT_1 0x6B
#define PWR_MGMT_2 0x6C

#define ACCEL_X2 0x3B
#define ACCEL_X1 0x3C
#define ACCEL_Y2 0x3D
#define ACCEL_Y1 0x3E
#define ACCEL_Z2 0x3F
#define ACCEL_Z1 0x40

#define GYRO_X2  0x43
#define GYRO_X1  0x44
#define GYRO_Y2  0x45
#define GYRO_Y1  0x46
#define GYRO_Z2  0x47
#define GYRO_Z1  0x48

#define ACCEL_X_TOP 45
#define ACCEL_X_BOTTOM -45

#define ACCEL_Y_TOP 45
#define ACCEL_Y_BOTTOM -45

//#define ACCEL_X_TOP 45
//#define ACCEL_X_BOTTOM -45

esp_err_t MPU6050_register_read(uint8_t reg_addr, uint8_t *data);
esp_err_t MPU6050_register_write_byte(uint8_t reg_addr, uint8_t data);
void initMpu6050(void);
double get_accel_x_value(int16_t x);
double get_accel_y_value(int16_t y);
double read_accel_x(void);
double read_accel_y(void);
//uint16_t read_gyro_x(void);
//double read_gyro_y(void);
//uint16_t read_gyro_z(void);
void print_accelerometer(void);
//void print_gyroscope(void);
uint8_t detect_danger_angles(void);
#endif