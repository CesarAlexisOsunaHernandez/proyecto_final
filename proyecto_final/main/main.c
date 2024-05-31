#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_task_wdt.h"
#include "esp_log.h"
#include "driver/i2c.h"
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "mqtt_client.h"

#include "buzzer.h"
#include "wheels.h"
#include "mpu_6050.h"
#include "bme280.h"

#define I2C_MASTER_SCL_IO           22      /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO           21      /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_NUM              0                          /*!< I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip */
#define I2C_MASTER_FREQ_HZ          400000                     /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS       1000

static const char *TAG = "MQTT_EXAMPLE";
esp_mqtt_client_handle_t client;
char command[40] = "";
uint8_t distance = 0;
uint8_t direction = 0;
uint32_t movementTime = 0;

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

void commandManager(char cmd[]){
    char method[15];
    char payload[6];
    uint8_t goal = 0;
    uint8_t i = 0;
    uint8_t j = 0;
    while (cmd[i] != '\0'){
        if (goal == 3){
            method[j] = cmd[i];
            j++;
        }else if (goal == 4){
            method[j - 1] = '\0';
            break;
        }
        if (cmd[i] == '"')
            goal++;
        
        i++;
    }
    goal = 0;
    j = 0;

    printf("method: %s\n", method);

    if (!strcmp("setState", method) || !strcmp("car1", method) || !strcmp("lightState", method)){
        while (cmd[i] != '}'){
            if (goal){
                payload[j] = cmd[i];
                j++;
            }
            if (cmd[i] == ':')
                goal++;
            
            i++;
        }
        payload[j] = '\0';
        printf("payload: %s\n", payload);

        // if (!strcmp("setState", method)){
        //     distance =  atoi(payload);
        //     printf("distance: %d\n", distance);
        // }else if (!strcmp("car1", method)){

        // }else 
        if (!strcmp("lightState", method)){
            if (!strcmp("false", payload)){
                gpio_set_level(23, 0);
                gpio_set_level(32, 0);
            }else{
                gpio_set_level(23, 1);
                gpio_set_level(32, 1);
            }
        }
        
    // }else if (!strcmp("foward", method)){
    //     direction = 1;
    // }else if (!strcmp("left", method)){
    //     direction = 3;
    // }else if (!strcmp("right", method)){
    //     direction = 4;
    // }else if (!strcmp("back", method)){
    //     direction = 2;
    }
    
    
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    esp_mqtt_event_handle_t event = event_data;
    //esp_mqtt_client_handle_t client_han = event->client;
    //int msg_id;
    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        //msg_id = esp_mqtt_client_publish(client, "/topic/qos1", "data_3", 0, 1, 0);
        //ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);

        esp_mqtt_client_subscribe(client, "v1/devices/me/rpc/request/+", 1);
        gpio_set_level(13, 1);
        //ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

        // msg_id = esp_mqtt_client_subscribe(client, "/topic/qos1", 1);
        // ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

        // msg_id = esp_mqtt_client_unsubscribe(client, "/topic/qos1");
        // ESP_LOGI(TAG, "sent unsubscribe successful, msg_id=%d", msg_id);
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        gpio_set_level(13, 0);
        break;

    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        //msg_id = esp_mqtt_client_publish(client, "/topic/qos0", "data", 0, 0, 0);
        //ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
        printf("DATA=%.*s\r\n", event->data_len, event->data);
        commandManager(event->data);
        //strncpy(command, event->data, event->data_len);
        //printf("command(naka): %s\n", command);
        // command[0] = event->data[30];
        // command[1] = event->data[31];
        // command[2] = event->data[32];
        // command[3] = event->data[33];
        // command[4] = event->data[34];
        // printf("command(naka): %s\n", command);
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        gpio_set_level(2, 0);
        // if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
        //     log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
        //     log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
        //     log_error_if_nonzero("captured as transport's socket errno",  event->error_handle->esp_transport_sock_errno);
        //     ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));

        // }
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        //msg_id = esp_mqtt_client_publish(client, "/topic/qos1", "data_3", 0, 1, 0);
        break;
    }
}

static void mqtt_app_start(void)
{
    //printf("1\n");
    esp_mqtt_client_config_t mqtt_cfg = {
        .uri = "mqtt://mqtt.thingsboard.cloud",
        .username = "67ZcjHjBKMOrtW6dajjU",
        .port = 1883,
    };
    //printf("2\n");
    client = esp_mqtt_client_init(&mqtt_cfg);
    //printf("3\n");
    /* The last argument may be used to pass data to the event handler, in this example mqtt_event_handler */
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    //printf("4\n");
    esp_mqtt_client_start(client);
    //printf("5\n");
}

void app_main(void)
{
    nvs_flash_init();
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = "INFINITUM05A2_2.4",
            .password = "26fafzT4NJ",
        },
    };
    
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    esp_wifi_start();
    esp_wifi_connect();

    mqtt_app_start();
    
    esp_task_wdt_init(3600,true);
    uint8_t i = 0;
    volatile uint16_t j = 0;
    uint8_t danger = 0;
    uint8_t buzz = 0;
    uint8_t fre = 0;
    float temp = 0;
    float press = 0;
    float hum = 0;

    //** SENORES INFRAROJOS **
    // gpio_reset_pin(26);                                  //sensor infrarojo 1
    // gpio_set_direction(26, GPIO_MODE_INPUT);

    // gpio_reset_pin(25);                                  //sensor infrarojo 2
    // gpio_set_direction(25, GPIO_MODE_INPUT);

    // gpio_reset_pin(33);                                  //sensor infrarojo 3
    // gpio_set_direction(33, GPIO_MODE_INPUT);

    gpio_reset_pin(13);
    gpio_set_direction(13, GPIO_MODE_OUTPUT);

    gpio_reset_pin(5);
    gpio_set_direction(5, GPIO_MODE_OUTPUT);

    gpio_reset_pin(23);
    gpio_set_direction(23, GPIO_MODE_OUTPUT);

    gpio_reset_pin(32);
    gpio_set_direction(32, GPIO_MODE_OUTPUT);

    //initWheels();
    i2c_master_init();
    initMpu6050();

    /* Resetea el sensor*/
    ESP_ERROR_CHECK(device_register_write_byte(RESET, 0xB6));

    while (1){
        
        //esp_mqtt_client_publish(client, "v1/devices/me/telemetry", "{temperature:30}", 0, 1, 0);

        if (i % 40 == 0 && i > 0 && j == 0){ //Cada 6 segundos
            initBme280();
            //printf("i: %d\n", i);
            set_calib_vars();
            // if (!detect_danger_angles()){
            //     danger = 0;
            //     // if(gpio_get_level(33) && gpio_get_level(26) && gpio_get_level(25))
            //     //     wheelsGoFoward();
            //     // else{
            //     //     if(!gpio_get_level(26) && gpio_get_level(25))
            //     //         wheelsTurnCounterClockwise();
            //     //     else if (gpio_get_level(26) && !gpio_get_level(25))
            //     //         wheelsTurnClockwise();
            //     //     else
            //     //         wheelsStop();
            //     // }
            // }else{
            //     //wheelsStop();
            //     danger = 1;
            // }

            temp += read_temperature();
            press += read_pressure();
            hum += read_humidity();

            //printBME280();
            //print_accelerometer();
        }else if (i == 200 && j == 1){
            i = 0;
            temp /= 5;
            press /= 5;
            hum /= 5;

            char aux[30] = "";
            uint8_t len = sprintf(aux, "{temperature1:%0.2f}", temp);
            esp_mqtt_client_publish(client, "v1/devices/me/telemetry", aux, len, 1, 0);
            temp = 0;

            len = sprintf(aux, "{humidity1:%.2f}", hum);
            esp_mqtt_client_publish(client, "v1/devices/me/telemetry", aux, len, 1, 0);
            hum = 0;

            len = sprintf(aux, "{pressure1:%.2f}", press);
            esp_mqtt_client_publish(client, "v1/devices/me/telemetry", aux, len, 1, 0);
            press = 0;

            len = sprintf(aux, "{accelerometer1_x:%.2lf}", read_accel_x());
            esp_mqtt_client_publish(client, "v1/devices/me/telemetry", aux, len, 1, 0);

            len = sprintf(aux, "{accelerometer1_y:%.2lf}", read_accel_y());
            esp_mqtt_client_publish(client, "v1/devices/me/telemetry", aux, len, 1, 0);
        }

        if (i % 6 == 0 && i > 0 && j == 0){ //Cada segundo
            if (!detect_danger_angles()){
                danger = 0;
                // if(gpio_get_level(33) && gpio_get_level(26) && gpio_get_level(25))
                //     wheelsGoFoward();
                // else{
                //     if(!gpio_get_level(26) && gpio_get_level(25))
                //         wheelsTurnCounterClockwise();
                //     else if (gpio_get_level(26) && !gpio_get_level(25))
                //         wheelsTurnClockwise();
                //     else
                //         wheelsStop();
                // }
            }else{
                //wheelsStop();
                
                danger = 1;
                char aux[30] = "";
                uint8_t len = sprintf(aux, "{accelerometer1_x:%.2lf}", read_accel_x());
                esp_mqtt_client_publish(client, "v1/devices/me/telemetry", aux, len, 1, 0);

                len = sprintf(aux, "{accelerometer1_y:%.2lf}", read_accel_y());
                esp_mqtt_client_publish(client, "v1/devices/me/telemetry", aux, len, 1, 0);
            }
        }
        
        if (danger){
            if (!(i % 3)){
                if (fre == 10){
                    fre = 0;
                    gpio_set_level(5, buzz);
                    buzz = !buzz;
                }
                fre++;
            }else{
                fre = 0;
                gpio_set_level(5, 0);
            }
        }else{
            gpio_set_level(5, 0);
        }


        // switch (direction){
        // case 1:
        //     if(movementTime < direction * 100000){
        //         wheelsGoFoward();
        //         movementTime++;
        //     }
        //     else{
        //         movementTime = 0;
        //         direction = 0;
        //         wheelsStop();
        //     }
            
        //     break;
        // case 2:
        //     if(movementTime < direction * 100000){
        //         wheelsGoBackward();
        //         movementTime++;
        //     }
        //     else{
        //         movementTime = 0;
        //         direction = 0;
        //         wheelsStop();
        //     }
            
        //     break;

        // case 3:
        //     if(movementTime < 50000){
        //         wheelsTurnClockwise();
        //         movementTime++;
        //     }
        //     else{
        //         movementTime = 0;
        //         direction = 0;
        //         wheelsStop();
        //     }
            
        //     break;

        // case 4:
        //     if(movementTime < 50000){
        //         wheelsTurnCounterClockwise();
        //         movementTime++;
        //     }
        //     else{
        //         movementTime = 0;
        //         direction = 0;
        //         wheelsStop();
        //     }
            
        //     break;
        
        // default:
        //     break;
        // }

        
        //print_gyroscope();
        delay10Us();
        j++;
        if(j == 15000){
            i++;
            j = 0;
        }
        //vTaskDelay(2000 / portTICK_PERIOD_MS);                                                                                                                                                                                                              
    }
    
}