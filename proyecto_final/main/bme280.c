#include <stdio.h>
#include "esp_log.h"
#include "driver/i2c.h"
#include "bme280.h"
#include "string.h"

esp_err_t device_register_read(uint8_t reg_addr, uint8_t *data, size_t len)
{
    return i2c_master_write_read_device(I2C_MASTER_NUM, SENSOR_ADDR, &reg_addr, 1, data, len, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS);
}

/*!
 * @brief This internal API is used to compensate the raw temperature data and
 * return the compensated temperature data in double data type.
 */
double get_temp_value(int32_t raw){
    double TempCacl[2];
    double temperature;
    double temperature_min = -40;
    double temperature_max = 85;

    TempCacl[0] = (((double)raw) / 16384.0 - ((double)dig_T1) / 1024.0);
    TempCacl[0] = TempCacl[0] * ((double)dig_T2);
    TempCacl[1] = (((double)raw) / 131072.0 - ((double)dig_T1) / 8192.0);
    TempCacl[1] = (TempCacl[1] * TempCacl[1]) * ((double)dig_T3);
    t_fine = (int32_t)(TempCacl[0] + TempCacl[1]);
    temperature = (TempCacl[0] + TempCacl[1]) / 5120.0;

    if (temperature < temperature_min)
        temperature = temperature_min;
    
    else if (temperature > temperature_max)
        temperature = temperature_max;

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
    double H_min = 0.0;
    double H_max = 100.0;
    double Calc[6] = {0};
    

    Calc[0] = ((double)t_fine) - 76800.0;
    Calc[1] = (((double)dig_H4) * 64.0 + (((double)dig_H5) / 16384.0) * Calc[0]);
    Calc[2] = adc_H - Calc[1];
    Calc[3] = ((double)dig_H2) / 65536.0;
    Calc[4] = (1.0 + (((double)dig_H3) / 67108864.0) * Calc[0]);
    Calc[5] = 1.0 + (((double)dig_H6) / 67108864.0) * Calc[0] * Calc[4];
    Calc[5] = Calc[2] * Calc[3] * (Calc[4] * Calc[5]);
    humidity = Calc[5] * (1.0 - ((double)dig_H1) * Calc[5] / 524288.0);

    if (humidity > H_max)
        humidity = H_max;
    
    else if (humidity < H_min)
        humidity = H_min;

    return humidity;
}


// Función para leer un registro de 16 bits desde el sensor
esp_err_t read_sensor_register(uint8_t reg_addr, uint16_t* reg_value) {
    uint8_t data[2];
    esp_err_t ret;

    // Leer el registro LSB
    ret = device_register_read(reg_addr, data, 1);
    if (ret != ESP_OK) {
        return ret;
    }
    *reg_value = data[0];

    // Leer el registro MSB
    ret = device_register_read(reg_addr + 1, data, 1);
    if (ret != ESP_OK) {
        return ret;
    }
    *reg_value |= (data[0] << 8);

    return ESP_OK;
}


//Funcion para configurar variables para el calculo de temperatura, presion y humedad
void set_calib_vars(){
    uint8_t data[2];

    //Temperatura
    read_sensor_register(0x89, &dig_T1);
    read_sensor_register(0x8B, &dig_T2);
    read_sensor_register(0x8D, &dig_T3);

    //Presion
    read_sensor_register(0x8E,&dig_P1);
    read_sensor_register(0x91,&dig_P2);
    read_sensor_register(0x93,&dig_P3);
    read_sensor_register(0x95,&dig_P4);
    read_sensor_register(0x97,&dig_P5);
    read_sensor_register(0x99,&dig_P6);
    read_sensor_register(0x9B,&dig_P7);
    read_sensor_register(0x9D,&dig_P8);
    read_sensor_register(0x9E,&dig_P9);

	//Humedad
    ESP_ERROR_CHECK(device_register_read(0xA1, data, 1));
    dig_H1 = data[0];

    ESP_ERROR_CHECK(device_register_read(0xE2, data, 1));
    dig_H2 = data[0]<<8;
    ESP_ERROR_CHECK(device_register_read(0xE1, data, 1));
    dig_H2 += data[0];

    ESP_ERROR_CHECK(device_register_read(0xE3, data, 1));
    dig_H3 = data[0];

    ESP_ERROR_CHECK(device_register_read(0xE4, data, 1));
    dig_H4 = data[0]<<4;
    ESP_ERROR_CHECK(device_register_read(0xE5, data, 1));
    dig_H4 += data[0] & 0x0F;

    ESP_ERROR_CHECK(device_register_read(0xE6, data, 1));
    dig_H5 = data[0]<<4;
    ESP_ERROR_CHECK(device_register_read(0xE5, data, 1));
    dig_H5 += data[0] & 0xF0;

    ESP_ERROR_CHECK(device_register_read(0xE7, data, 1));
    dig_H6 = data[0];
}
