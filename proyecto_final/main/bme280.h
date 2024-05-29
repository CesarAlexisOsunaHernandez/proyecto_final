#ifndef BME280_H
#define BME280_H

#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "driver/i2c.h"

#define BME280_ADDR 0x76
#define BME280_WHO_AM_I_REG_ADDR 0xD0

#define RESET 0xE0

#define CTRL_MEAS 0xF4
#define CTRL_HUM 0xF2
#define CONFIG 0xF5

#define HUM_LSB 0xFE
#define HUM_MSB 0xFD

#define TEMP_XLSB 0xFC
#define TEMP_LSB 0xFB
#define TEMP_MSB 0xFA

#define PRESS_XLSB 0xF9
#define PRESS_LSB 0xF8
#define PRESS_MSB 0xF7

unsigned short dig_T1;
signed short dig_T2;
signed short dig_T3;

unsigned short dig_P1;
signed short dig_P2;
signed short dig_P3;
signed short dig_P4;
signed short dig_P5;
signed short dig_P6;
signed short dig_P7;
signed short dig_P8;
signed short dig_P9;

unsigned char dig_H1;
signed short dig_H2;
unsigned char dig_H3;
signed short dig_H4;
signed short dig_H5;
signed char dig_H6;

int32_t t_fine;

esp_err_t device_register_read(uint8_t reg_addr, uint8_t *data);
esp_err_t device_register_write_byte(uint8_t reg_addr, uint8_t data);
void initBme280();
void set_calib_vars();
double get_temp_value(int32_t raw);
double get_press_value(int32_t adc_P);
double get_hum_value(int32_t adc_H);
double read_humidity();
double read_temperature();
double read_pressure();
void printBME280();

#endif