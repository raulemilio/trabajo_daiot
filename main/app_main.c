#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "smart_config.h"
#include "nvs_flash.h"
#include "sntp_time.h"
#include "mqtt_basico.h"
#include <bmp280.h>

#ifndef APP_CPU_NUM
#define APP_CPU_NUM PRO_CPU_NUM
#endif

extern bool time_sinc_ok;

void app_main(void)
{
    ESP_ERROR_CHECK( nvs_flash_init() );
    
    ESP_ERROR_CHECK(i2cdev_init());
    
    /*
    Cola para enviar las mediciones obtenidas del sensor (función bmp280_test)
    a la función publicar_temperatura_task que envia por mqtt
    */
    xQueue = xQueueCreate( 1, sizeof( Data_t ) );
    xQueue1 = xQueueCreate( 1, sizeof( int) );
    xQueue2 = xQueueCreate( 1, sizeof( int) );
    
    initialise_wifi();

    initialize_sntp();

    while (!time_sinc_ok) vTaskDelay(100 * 1);
    
    xTaskCreate(mqtt_app_main_task, "mqtt_app_task", 4096 * 8, NULL, 3, NULL);
    
    xTaskCreate(led_task, "led_task", 4096 * 8, NULL, 3, NULL);

    xTaskCreate(publicar_temperatura_task, "temp_pub_task", 4096 * 8, NULL, 3, NULL);
    
    xTaskCreatePinnedToCore(bmp280_test, "bmp280_test", configMINIMAL_STACK_SIZE * 8, NULL, 5, NULL, APP_CPU_NUM); // lectura del sensor
    
}

