#ifndef MPU6050_H
#define MPU6050_H

#include "freertos/FreeRTOS.h"
#include "driver/i2c.h"

#define MPU6050_ADDR 0x68
#define MPU6050_WHO_AM_I_REG_ADDR 0x75
#define PWR_MGMT_1 0x6B

esp_err_t device_register_read(uint8_t reg_addr, uint8_t *data);
esp_err_t device_register_write_byte(uint8_t reg_addr, uint8_t data);

#endif