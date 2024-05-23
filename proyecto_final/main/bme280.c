#include "bme280.h"

/**
 * @brief Read a sequence of bytes from a MPU6050 sensor registers
 */
esp_err_t device_register_read(uint8_t reg_addr, uint8_t *data)
{
    return i2c_master_write_read_device(0, BME280_ADDR, &reg_addr, 1, data, 1, 1000 / portTICK_RATE_MS);
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

    return i2c_master_write_to_device(0, BME280_ADDR, write_buf, sizeof(write_buf), 1000 / portTICK_RATE_MS);
}

/*!
 * @brief This internal API is used to compensate the raw temperature data and
 * return the compensated temperature data in double data type.
 */
double get_temp_value(int32_t raw){
    double var1;
    double var2;
    double temperature;
    double temperature_min = -40;
    double temperature_max = 85;

    var1 = (((double)raw) / 16384.0 - ((double)dig_T1) / 1024.0);
    var1 = var1 * ((double)dig_T2);
    var2 = (((double)raw) / 131072.0 - ((double)dig_T1) / 8192.0);
    var2 = (var2 * var2) * ((double)dig_T3);
    t_fine = (int32_t)(var1 + var2);
    temperature = (var1 + var2) / 5120.0;

    if (temperature < temperature_min)
    {
        temperature = temperature_min;
    }
    else if (temperature > temperature_max)
    {
        temperature = temperature_max;
    }

    return temperature;
}

double get_press_value(int32_t adc_P)
{
    int64_t var1, var2, p;
    var1 = ((int64_t)t_fine) - 128000; var2 = var1 * var1 * (int64_t)dig_P6;
    var2 = var2 + ((var1*(int64_t)dig_P5)<<17); var2 = var2 + (((int64_t)dig_P4)<<35);
    var1 = ((var1 * var1 * (int64_t)dig_P3)>>8) + ((var1 * (int64_t)dig_P2)<<12); var1 = (((((int64_t)1)<<47)+var1))*((int64_t)dig_P1)>>33;
    if (var1 == 0)
    {
    return 0; // avoid exception caused by division by zero
    }
    p = 1048576-adc_P;
    p = (((p<<31)-var2)*3125)/var1;
    var1 = (((int64_t)dig_P9) * (p>>13) * (p>>13)) >> 25;
    var2 = (((int64_t)dig_P8) * p) >> 19;
    p = ((p + var1 + var2) >> 8) + (((int64_t)dig_P7)<<4); return ((double)p) / 25600;
}

double get_hum_value(int32_t adc_H)
{
	double humidity;
    double humidity_min = 0.0;
    double humidity_max = 100.0;
    double var1;
    double var2;
    double var3;
    double var4;
    double var5;
    double var6;

    var1 = ((double)t_fine) - 76800.0;
    var2 = (((double)dig_H4) * 64.0 + (((double)dig_H5) / 16384.0) * var1);
    var3 = adc_H - var2;
    var4 = ((double)dig_H2) / 65536.0;
    var5 = (1.0 + (((double)dig_H3) / 67108864.0) * var1);
    var6 = 1.0 + (((double)dig_H6) / 67108864.0) * var1 * var5;
    var6 = var3 * var4 * (var5 * var6);
    humidity = var6 * (1.0 - ((double)dig_H1) * var6 / 524288.0);

    if (humidity > humidity_max)
    {
        humidity = humidity_max;
    }
    else if (humidity < humidity_min)
    {
        humidity = humidity_min;
    }

    return humidity;
}

/*
Funcion para configurar variables para el calculo de temperatura, presion y humedad
*/
void set_calib_vars(){
    uint8_t data[2];

    //Variables para la temperatura
    ESP_ERROR_CHECK(device_register_read(0x89, data));
    dig_T1 = data[0]<<8;
    ESP_ERROR_CHECK(device_register_read(0x88, data));
    dig_T1 += data[0];

    ESP_ERROR_CHECK(device_register_read(0x8B, data));
    dig_T2 = data[0]<<8;
    ESP_ERROR_CHECK(device_register_read(0x8A, data));
    dig_T2 += data[0];

    ESP_ERROR_CHECK(device_register_read(0x8D, data));
    dig_T3 = data[0]<<8;
    ESP_ERROR_CHECK(device_register_read(0x8C, data));
    dig_T3 += data[0];

    //Variables para la presion
    ESP_ERROR_CHECK(device_register_read(0x8E, data));
    dig_P1 = data[0]<<8;
    ESP_ERROR_CHECK(device_register_read(0x8F, data));
    dig_P1 += data[0];

    ESP_ERROR_CHECK(device_register_read(0x91, data));
    dig_P2 = data[0]<<8;
    ESP_ERROR_CHECK(device_register_read(0x90, data));
    dig_P2 += data[0];

    ESP_ERROR_CHECK(device_register_read(0x93, data));
    dig_P3 = data[0]<<8;
    ESP_ERROR_CHECK(device_register_read(0x92, data));
    dig_P3 += data[0];

    ESP_ERROR_CHECK(device_register_read(0x95, data));
    dig_P4 = data[0]<<8;
    ESP_ERROR_CHECK(device_register_read(0x94, data));
    dig_P4 += data[0];

    ESP_ERROR_CHECK(device_register_read(0x97, data));
    dig_P5 = data[0]<<8;
    ESP_ERROR_CHECK(device_register_read(0x96, data));
    dig_P5 += data[0];

    ESP_ERROR_CHECK(device_register_read(0x99, data));
    dig_P6 = data[0]<<8;
    ESP_ERROR_CHECK(device_register_read(0x98, data));
    dig_P6 += data[0];

    ESP_ERROR_CHECK(device_register_read(0x9B, data));
    dig_P7 = data[0]<<8;
    ESP_ERROR_CHECK(device_register_read(0x9A, data));
    dig_P7 += data[0];

    ESP_ERROR_CHECK(device_register_read(0x9D, data));
    dig_P8 = data[0]<<8;
    ESP_ERROR_CHECK(device_register_read(0x9C, data));
    dig_P8 += data[0];

    ESP_ERROR_CHECK(device_register_read(0x9E, data));
    dig_P9 = data[0]<<8;
    ESP_ERROR_CHECK(device_register_read(0x9F, data));
    dig_P9 += data[0];

	//Variables para la humedad
    ESP_ERROR_CHECK(device_register_read(0xA1, data));
    dig_H1 = data[0];

    ESP_ERROR_CHECK(device_register_read(0xE2, data));
    dig_H2 = data[0]<<8;
    ESP_ERROR_CHECK(device_register_read(0xE1, data));
    dig_H2 += data[0];

    ESP_ERROR_CHECK(device_register_read(0xE3, data));
    dig_H3 = data[0];

    ESP_ERROR_CHECK(device_register_read(0xE4, data));
    dig_H4 = data[0]<<4;
    ESP_ERROR_CHECK(device_register_read(0xE5, data));
    dig_H4 += data[0] & 0x0F;

    ESP_ERROR_CHECK(device_register_read(0xE6, data));
    dig_H5 = data[0]<<4;
    ESP_ERROR_CHECK(device_register_read(0xE5, data));
    dig_H5 += data[0] & 0xF0;

    ESP_ERROR_CHECK(device_register_read(0xE7, data));
    dig_H6 = data[0];
}

double read_humidity(){
    uint32_t hum;
    uint8_t data[2];

    device_register_read(HUM_MSB, data);
    hum = data[0] << 8;
    device_register_read(HUM_LSB, data);
    hum += data[0];

    return get_hum_value(hum);
}

double read_temperature(){
    uint32_t tem;
    uint8_t data[2];

    device_register_read(TEMP_MSB, data);
    tem = data[0]<<12;
    device_register_read(TEMP_LSB, data);
    tem += data[0]<<4;
    device_register_read(TEMP_XLSB, data);
    tem += data[0]>>4;

    return get_temp_value(tem);
}

double read_pressure(){
    uint32_t pres;
    uint8_t data[2];

    device_register_read(PRESS_MSB, data);
    pres = data[0]<<12;
    device_register_read(PRESS_LSB, data);
    pres += data[0]<<4;
    device_register_read(PRESS_XLSB, data);
    pres += data[0]>>4;

    return get_press_value(pres);
}