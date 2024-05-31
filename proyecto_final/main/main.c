/* i2c - Simple example

   Simple I2C example that shows how to initialize I2C
   as well as reading and writing from and to registers for a sensor connected over I2C.

   The sensor used in this example is a MPU9250 inertial measurement unit.

   For other examples please check:
   https://github.com/espressif/esp-idf/tree/master/examples

   See README.md file to get detailed usage of this example.

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "esp_log.h"
#include "driver/i2c.h"
#include "bme280.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_log.h"
#include "mqtt_client.h"
#include "esp_tls.h"
#include "esp_ota_ops.h"
#include <sys/param.h>
#include "connect_wifi.h"

#include "mpu_6050.h"
#include "buzzer.h"
#include "wheels.h"

#include "driver/adc.h"
#include "driver/ledc.h"
#include "cJSON.h"

static const char *TAG = "i2c-simple-example";

#define I2C_MASTER_SCL_IO           22      /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO           21      /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_NUM              0                          /*!< I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip */
#define I2C_MASTER_FREQ_HZ          400000                     /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS       1000

// Definir la resolución del duty cycle
#define LEDC_DUTY_RES LEDC_TIMER_13_BIT

char bme_Values[3][30];
esp_mqtt_client_handle_t client;
uint32_t lectura;
uint32_t buzzer_time=0;
uint32_t Millis = 0;
uint8_t buzzerEn = 0;
char metodoMQTT[10]; 
char valorMQTT[10];

static esp_err_t device_register_write_byte(uint8_t reg_addr, uint8_t data)
{
    int ret;
    uint8_t write_buf[2] = {reg_addr, data};

    ret = i2c_master_write_to_device(I2C_MASTER_NUM, SENSOR_ADDR, write_buf, sizeof(write_buf), I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS);

    return ret;
}

/**
 * @brief i2c master initialization
 */
static esp_err_t i2c_master_init(void)
{
    int i2c_master_port = I2C_MASTER_NUM;

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };

    i2c_param_config(i2c_master_port, &conf);

    return i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    esp_mqtt_event_handle_t event = event_data;
    //esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        msg_id = esp_mqtt_client_subscribe(client, "v1/devices/me/rpc/request/+", 1);
        //mosquitto_sub -h thingsboard.cloud -p 1883 -t v1/devices/me/rpc/request/+ -u "0tniWSad3JNvcaY7a6Rm" -d
        ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        break;
    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        printf("DATA=%.*s\r\n", event->data_len, event->data);

            char data[100]; 
            snprintf(data, event->data_len + 1, "%.*s", event->data_len, event->data);

            // Analizar la cadena JSON
            cJSON *json = cJSON_Parse(data);
            if (json == NULL) {
                ESP_LOGE(TAG, "Error al analizar JSON");
                break;
            }

            // Extraer el valor del campo "method"
            cJSON *method = cJSON_GetObjectItem(json, "method");
            if (cJSON_IsString(method) && (method->valuestring != NULL)) {
                strcpy(metodoMQTT,method->valuestring);
                printf("Método: %s\n", metodoMQTT);
            }

            // Extraer el valor del campo "params"
            cJSON *params = cJSON_GetObjectItem(json, "params");
            if (cJSON_IsString(params) && (params->valuestring != NULL)) {
                strcpy(valorMQTT,params->valuestring);
                printf("Parámetros: %s\n",valorMQTT);
            }

            // Liberar la memoria del objeto JSON
            cJSON_Delete(json);
            break;
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
            ESP_LOGI(TAG, "Last error code reported from esp-tls: 0x%x", event->error_handle->esp_tls_last_esp_err);
            ESP_LOGI(TAG, "Last tls stack error number: 0x%x", event->error_handle->esp_tls_stack_err);
            ESP_LOGI(TAG, "Last captured errno : %d (%s)",  event->error_handle->esp_transport_sock_errno,
                     strerror(event->error_handle->esp_transport_sock_errno));
        } else if (event->error_handle->error_type == MQTT_ERROR_TYPE_CONNECTION_REFUSED) {
            ESP_LOGI(TAG, "Connection refused error: 0x%x", event->error_handle->connect_return_code);
        } else {
            ESP_LOGW(TAG, "Unknown error type: 0x%x", event->error_handle->error_type);
        }
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}

static void mqtt_app_start(void)
{
    const esp_mqtt_client_config_t mqtt_cfg = {
        .uri = "mqtt://mqtt.thingsboard.cloud", // URI del servidor ThingsBoard
        .username = "0tniWSad3JNvcaY7a6Rm",        // Nombre de usuario
        .password = NULL,                          // Contraseña (puede ser NULL si no es requerida)
    };

    ESP_LOGI(TAG, "[APP] Free memory: %d bytes", esp_get_free_heap_size());
    client = esp_mqtt_client_init(&mqtt_cfg);
    /* The last argument may be used to pass data to the event handler, in this example mqtt_event_handler */
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);
}

void app_main(void)
{
    uint8_t data[3];

    uint16_t humidity = 0;
    int32_t temperature = 0;
    uint32_t pressure = 0;
    uint16_t time = 0;

    // Configurar los pines GPIO como salidas
    gpio_pad_select_gpio(LED1_GPIO_PIN);
    gpio_set_direction(LED1_GPIO_PIN, GPIO_MODE_OUTPUT);
    gpio_pad_select_gpio(LED2_GPIO_PIN);
    gpio_set_direction(LED2_GPIO_PIN, GPIO_MODE_OUTPUT);

    ESP_ERROR_CHECK(i2c_master_init());
    ESP_LOGI(TAG, "I2C initialized successfully");

    /* Resetea el sensor*/
    ESP_ERROR_CHECK(device_register_write_byte(RESET, 0xB6));

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    connect_wifi();
    mqtt_app_start();
    initWheels();
    
    while (1){
		
		ESP_ERROR_CHECK(device_register_write_byte(CTRL_HUM, 0x05));/*Oversampling X16*/
    	ESP_ERROR_CHECK(device_register_write_byte(CTRL_MEAS, 0xB5));//Forced Mode
    	ESP_ERROR_CHECK(device_register_write_byte(CONFIG, 0x10));/*Filtro con coeficiente: 16*/
        set_calib_vars();

        ESP_ERROR_CHECK(device_register_read(WHO_AM_I_REG_ADDR, data, 1));

        ESP_ERROR_CHECK(device_register_read(HUM_MSB, data, 1));
        humidity = data[0] << 8;
        ESP_ERROR_CHECK(device_register_read(HUM_LSB, data, 1));
        humidity += data[0];
		
        ESP_ERROR_CHECK(device_register_read(TEMP_MSB, data, 3));
        temperature = ((int32_t)data[0] << 12) | ((int32_t)data[1] << 4) | (data[2] >> 4);
        
        ESP_ERROR_CHECK(device_register_read(PRESS_MSB, data, 3));
        pressure = ((uint32_t)data[0] << 12) | ((uint32_t)data[1] << 4) | (data[2] >> 4);
        

        if(time > 3000){//Cada 30 seg se guardan valores
            sprintf(bme_Values[0],"{temperature: %.2f}",get_temp_value(temperature));
            sprintf(bme_Values[1],"{humidity: %.2f}",get_hum_value(humidity));
            sprintf(bme_Values[2],"{pressure: %.2f}",2*get_press_value(pressure));
            printf("Temp: %s - Hum: %s - Press: %s\n",bme_Values[0],bme_Values[1],bme_Values[2]);
            esp_mqtt_client_publish(client, "v1/devices/me/telemetry", bme_Values[0], 0, 1, 0);
            esp_mqtt_client_publish(client, "v1/devices/me/telemetry", bme_Values[1], 0, 1, 0);
            esp_mqtt_client_publish(client, "v1/devices/me/telemetry", bme_Values[2], 0, 1, 0);
            time = 0;
        }

        if(!strcmp(metodoMQTT,"setState")){
            if(!strcmp(valorMQTT,"up")){
                wheelsGoFoward();
            }
            else if(!strcmp(valorMQTT,"down")){
                wheelsGoBackward();
            }
            else if(!strcmp(valorMQTT,"right")){
                wheelsTurnClockwise();
            }
            else if(!strcmp(valorMQTT,"left")){
                wheelsTurnCounterClockwise();
            }
            else if(!strcmp(valorMQTT,"off")){
                wheelsStop();
            }
            strcpy(metodoMQTT,"");
        }

        if(!strcmp(metodoMQTT,"setLeds")){
            if(!strcmp(valorMQTT,"true")){
                buzzerEn = 1;                
            }
            if(!strcmp(valorMQTT,"false")){
                buzzerEn = 0;
            }
            strcpy(metodoMQTT,"");
        }

        if(buzzerEn){
            buzzer_play(&buzzer_time);
        }
        
        time++;
        Millis++;
        buzzer_time++;
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}