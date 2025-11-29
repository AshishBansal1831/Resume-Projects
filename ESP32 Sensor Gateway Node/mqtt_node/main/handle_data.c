#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp_system.h"
#include "mqtt_client.h"

#include "common.h"

extern QueueHandle_t config_queue;
extern esp_mqtt_client_handle_t client;
extern int connected_flag;

void configuration_task()
{
    ConfigSendQueue temp;

    while(1)
    {
        if (xQueueReceive(config_queue, &temp, portMAX_DELAY) == pdFALSE)
        {
            continue;
        }

        /*
         * Configuration data is supposed to be of the follwoing format from mqtt
         * sensor,sampling_rate
         * 
         * Example : To sample temperature at 10Hz
         * Temperature, 10
         * 
        */

        printf("Received Semething\r\n");        
        temp.string[temp.length] = 0;

        char *end_pt = memchr(temp.string, ',', temp.length);
        *end_pt = 0;
        printf("sensor - %s, Data - %d\r\n", temp.string, atoi( end_pt + 1));        

        memset(&temp, 0, sizeof(temp));

        // <! TODO : Send the config command via CAN Task here
    }
}

void publish_task()
{
    char data_string[80];
    while(1)
    {
        // <! Use Queue here to get data to send to the topic
        esp_mqtt_client_publish(client, "/rtos-can/data/temp", "data1", 6, QOS1, 0);
        vTaskDelay(100);
    }
}

