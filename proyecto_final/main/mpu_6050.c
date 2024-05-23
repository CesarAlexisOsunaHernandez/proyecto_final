#include "mpu_6050.h"

/**
 * @brief Read a sequence of bytes from a MPU6050 sensor registers
 */
esp_err_t device_register_read(uint8_t reg_addr, uint8_t *data)
{
    return i2c_master_write_read_device(0, MPU6050_ADDR, &reg_addr, 1, data, 1, 1000 / portTICK_RATE_MS);
}

/**
 * @brief Write a byte to a MPU6050 sensor register
 */
esp_err_t device_register_write_byte(uint8_t reg_addr, uint8_t data)
{
    //int ret;
    uint8_t write_buf[2] = {reg_addr, data};

    // ret = i2c_master_write_to_device(0, MPU6050_ADDR, write_buf, sizeof(write_buf), 1000 / portTICK_RATE_MS);

    // return ret;

    return i2c_master_write_to_device(0, MPU6050_ADDR, write_buf, sizeof(write_buf), 1000 / portTICK_RATE_MS);
}