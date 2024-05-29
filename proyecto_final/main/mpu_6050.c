#include "mpu_6050.h"

static const char *MPU = "MPU6050";

/**
 * @brief Read a sequence of bytes from a MPU6050 sensor registers
 */
esp_err_t MPU6050_register_read(uint8_t reg_addr, uint8_t *data)
{
    return i2c_master_write_read_device(0, MPU6050_ADDR, &reg_addr, 1, data, 1, 1000 / portTICK_RATE_MS);
}

/**
 * @brief Write a byte to a MPU6050 sensor register
 */
esp_err_t MPU6050_register_write_byte(uint8_t reg_addr, uint8_t data)
{
    //int ret;
    uint8_t write_buf[2] = {reg_addr, data};

    // ret = i2c_master_write_to_device(0, MPU6050_ADDR, write_buf, sizeof(write_buf), 1000 / portTICK_RATE_MS);

    // return ret;

    return i2c_master_write_to_device(0, MPU6050_ADDR, write_buf, sizeof(write_buf), 1000 / portTICK_RATE_MS);
}

void initMpu6050(void){
    /*Resetea el dispositivo MPU6050*/
    ESP_ERROR_CHECK(MPU6050_register_write_byte(PWR_MGMT_1, 0x80));
    ESP_LOGI(MPU, "Reset successfull");
    /*Configura para poner el sensor en modo CYCLE y deshabilitar el sensor de temperatura*/
    ESP_ERROR_CHECK(MPU6050_register_write_byte(PWR_MGMT_1, 0x28));
    ESP_LOGI(MPU, "Cycle mode initialized successfully");
    /*Configura para poner el sensor en modo SOLO acelerometro sin el eje Z*/
    ESP_ERROR_CHECK(MPU6050_register_write_byte(PWR_MGMT_2, 0x0F));
    ESP_LOGI(MPU, "ONLY accelerometer mode initialized successfully");
    ESP_LOGI(MPU, "MPU initialized successfully");
}

double get_accel_x_value(int16_t x){
    double sX = (double)x;

    return (sX * 90) / 16383;
}

double get_accel_y_value(int16_t y){
    double sY = (double)y;

    return (sY * 90) / 16383;
}

double read_accel_x(void){
    int16_t accel_x;
    uint8_t data[2];

    MPU6050_register_read(ACCEL_X2, data);
    accel_x = data[0] << 8;
    MPU6050_register_read(ACCEL_X1, data);
    accel_x += data[0];

    return get_accel_x_value(accel_x);
    //return accel_x;
}

double read_accel_y(void){
    int16_t accel_y;
    uint8_t data[2];

    MPU6050_register_read(ACCEL_Y2, data);
    accel_y = data[0] << 8;
    MPU6050_register_read(ACCEL_Y1, data);
    accel_y += data[0];

    return get_accel_y_value(accel_y);
    //return accel_y;
}

// uint16_t read_gyro_x(void){
//     uint16_t gyro_x;
//     uint8_t data[2];

//     MPU6050_register_read(GYRO_X2, data);
//     printf("DATA: %d\n", data[0]);
//     gyro_x = data[0] << 8;
//     MPU6050_register_read(GYRO_X1, data);
//     gyro_x += data[0];

//     //return get_gyro_x_value(gyro_x);
//     return gyro_x;
// }

// double read_gyro_y(void){
//     int16_t gyro_y;
//     uint8_t data[2];

//     MPU6050_register_read(GYRO_Y2, data);
//     gyro_y = data[0] << 8;
//     MPU6050_register_read(GYRO_Y1, data);
//     gyro_y += data[0];

//     //return get_gyro_y_value(gyro_y);
//     return gyro_y;
// }

// uint16_t read_gyro_z(void){
//     uint16_t gyro_z;
//     uint8_t data[2];

//     MPU6050_register_read(GYRO_Z2, data);
//     gyro_z = data[0] << 8;
//     MPU6050_register_read(GYRO_Z1, data);
//     gyro_z += data[0];

//     //return get_hum_value(hum);
//     return gyro_z;
// }

void print_accelerometer(void){
    printf("\nX: %d Y: %.2lf\n", (int16_t)read_accel_x(), read_accel_y());
}

// void print_gyroscope(void){
//     printf("\nX: %d Y: %d Z: %d\n", (int16_t)read_gyro_x(), (int16_t)read_gyro_y(), (int16_t)read_gyro_z());
// }

uint8_t detect_danger_angles(void){
    if (read_accel_x() > ACCEL_X_TOP || read_accel_x() < ACCEL_X_BOTTOM){
        ESP_LOGI(MPU, "Topple imminent");
        return 1;
    }
    if (read_accel_y() > ACCEL_Y_TOP || read_accel_y() < ACCEL_Y_BOTTOM){
        ESP_LOGI(MPU, "Steep incline detected");
        return 1;
    }
    return 0;
}