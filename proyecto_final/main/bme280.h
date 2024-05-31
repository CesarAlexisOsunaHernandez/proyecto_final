#ifndef BME280_H
#define BME280_H

#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "driver/i2c.h"

#define I2C_MASTER_SCL_IO           22      /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO           21      /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_NUM              0                          /*!< I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip */
#define I2C_MASTER_FREQ_HZ          400000                     /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS       1000


#define SENSOR_ADDR 0x76
#define WHO_AM_I_REG_ADDR 0xD0

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

unsigned short dig_T1, dig_T2, dig_T3;

unsigned short dig_P1, dig_P2, dig_P3, dig_P4, dig_P5, dig_P6, dig_P7, dig_P8, dig_P9;

unsigned char dig_H1;
signed short dig_H2;
unsigned char dig_H3;
signed short dig_H4, dig_H5, dig_H6;

int32_t t_fine;

esp_err_t device_register_read(uint8_t reg_addr, uint8_t *data, size_t len);
double get_temp_value(int32_t raw);
double get_press_value(int32_t adc_P);
double get_hum_value(int32_t adc_H);
esp_err_t read_sensor_register(uint8_t reg_addr, uint16_t* reg_value);
void set_calib_vars();

#endif