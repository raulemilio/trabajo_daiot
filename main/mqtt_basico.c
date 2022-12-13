/* MQTT (over TCP) Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "pemkey.h"
#include "gcp_min_ca.h"

#include "mqtt_basico.h"
#include <string.h>
#include "esp_system.h"
#include "esp_event.h"

#include "esp_netif.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "mqtt_client.h"

#include "esp_wifi.h"

#include "cJSON.h"

#include "sntp_time.h"

#include "jwt_token_gcp.h"

#include "esp_random.h"
#include "esp_sleep.h"

#include <stdio.h>

#include <bmp280.h>

#include "driver/gpio.h"
#include "led_strip.h"

/***BMP280****************************************************************************/
#define CONFIG_EXAMPLE_I2C_MASTER_SDA 4
#define CONFIG_EXAMPLE_I2C_MASTER_SCL 5
/***BMP280****************************************************************************/

#define BLINK_GPIO 0  // 8 led interno 0 para externo

static const char *TAG = "MQTT MODULE: ";

#define MOSQUITO_USER_NAME              "daiot"
#define MOSQUITO_USER_PASSWORD          "daiot"
//#define MOSQUITO_USER_PASSWORD          ""

esp_mqtt_client_handle_t cliente = NULL;

int RTC_DATA_ATTR last_error_count = 0;
int RTC_DATA_ATTR last_error_code = 0;
static unsigned char RTC_DATA_ATTR forzar_espera_sntp = 0;
unsigned int RTC_DATA_ATTR last_on_time_seconds = 0;
int sntp_response_time_seconds = 0;
unsigned int tph_on_time = 0;
time_t wake_up_timestamp = 0;


char MQTT_suffix[200];
char MQTT_topic[250];
char MQTT_payload[200];
size_t MQTT_payload_length = 200;
char MQTT_valor[100];
size_t MQTT_values_count;

esp_mqtt_client_handle_t mqtt_client_handle = NULL;
esp_mqtt_client_config_t mqtt_client_config = { };
char* GCP_JWT = NULL;
bool mqtt_client_connected = false;
bool mqtt_disconnected_event_flag = false;


static bool mqtt_client_configure(void);

int T = 0, P = 0, H = 0; // las declaro global para probar rapidamente
uint8_t id_sensor_recibido;


static esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event)
{
    BaseType_t xStatus;// agregado
    cliente = event->client;
    switch (event->event_id) {
            case MQTT_EVENT_CONNECTED:
                ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");

                // Suscribirse a tema 'config' de Google Cloud IoT
                esp_mqtt_client_subscribe(event->client, "/topic/dispositivos/estadoElectrovalvula", 0);

                // Suscribirse a tema 'commands' de Google Cloud IoT
                esp_mqtt_client_subscribe(event->client, "/topic/qos0", 0);
                mqtt_client_connected = true;
                break;

            case MQTT_EVENT_DISCONNECTED:
                ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
                //ESP_LOGI(TAG, "Resetting MQTT Config");
                //mqtt_client_reset_config();
                mqtt_client_connected = false;
                mqtt_disconnected_event_flag = true;
                break;

            case MQTT_EVENT_SUBSCRIBED:
                ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);		
                break;

            case MQTT_EVENT_UNSUBSCRIBED:
                ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
                break;

            case MQTT_EVENT_PUBLISHED:
                ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
                last_error_count = 0;
                last_error_code = 0;
                last_on_time_seconds = 0;
                forzar_espera_sntp = 0;
                sntp_response_time_seconds = 0;
                break;

            case MQTT_EVENT_DATA:
                ESP_LOGI(TAG, "MQTT_EVENT_DATA: ");
                printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
                printf("DATA=%.*s\r\n", event->data_len, event->data);
                /******************************/
                char *pcStringToSend;
                char *estado="1";
                pcStringToSend=(char *)event->data;
                int electroState=0;
                if(strncmp(pcStringToSend, estado,1)==0)
                {
                	electroState=1;
                }else
                {
                	electroState=0;
                }
                xStatus = xQueueSendToBack( xQueue1, &electroState, 300);
					if( xStatus != pdPASS )
					{
					/* The send operation could not complete because the queue was full -
					this must be an error as the queue should never contain more than
					one item! */
					printf( "Could not send to the queue.\n" );
					} 
				/******************************/
                break;

            case MQTT_EVENT_ERROR:
                ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
                // Si se presento error en la conexion MQTT, tal vez esta mal el horario del token.
                // Forzar espera de sincronizacion en el proximo reinicio.
                forzar_espera_sntp = 1;
                last_error_count ++;
                last_error_code |= ERROR_CODE_MQTT;
                break;

            default:
                ESP_LOGI(TAG, "Other event id:%d", event->event_id);
                break;
        }
        return ESP_OK;
}


static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    mqtt_event_handler_cb(event_data);
}


void mqtt_app_main_task(void * parm)
{
    ESP_LOGI(TAG, "Ingresa a mqtt_app_main_task()");

    mqtt_client_configure();

    esp_mqtt_client_handle_t mqtt_client_handle = esp_mqtt_client_init(&mqtt_client_config);
    esp_mqtt_client_register_event(mqtt_client_handle, ESP_EVENT_ANY_ID, mqtt_event_handler, mqtt_client_handle);

    ESP_LOGI(TAG, "Arrancando MQTT client... ");
    esp_mqtt_client_start(mqtt_client_handle);


	while (1) {

		while(!mqtt_disconnected_event_flag) vTaskDelay(1000 / portTICK_PERIOD_MS);

		ESP_LOGI(TAG, "Actualizando configuracion Cliente MQTT... ");
		if (mqtt_client_configure()) {
			ESP_LOGI(TAG, "Seteando configuracion Cliente MQTT... ");
			esp_mqtt_set_config(mqtt_client_handle, &mqtt_client_config);
			ESP_LOGI(TAG, "Reseteando flag evento disconnected. ");		
			mqtt_disconnected_event_flag = false;			
		}

	}

	vTaskDelete(NULL);

}


static bool mqtt_client_configure(void) {

//	ESP_LOGI(TAG, "Generando JWT Token... ");
   // if (GCP_JWT != NULL) free(GCP_JWT);
	
//	GCP_JWT = createGCPJWT(IOTCORE_PROJECTID, GCP_PEM_KEY, strlen(GCP_PEM_KEY), IOTCORE_TOKEN_EXPIRATION_TIME_MINUTES);
/*
    if (GCP_JWT == 0) {
        last_error_count ++;
        last_error_code |= ERROR_CODE_JWT;
		ESP_LOGI(TAG, "Error al generar JWT Token... ");
		return false;
    }*/

    mqtt_client_config.uri = BROKER_URI;
    mqtt_client_config.username = MOSQUITO_USER_NAME;
    mqtt_client_config.password = MOSQUITO_USER_PASSWORD; 
   // mqtt_client_config.cert_pem = GCP_MIN_CA;
	//mqtt_client_config.disable_auto_reconnect = true;

    //printf("pass JWT: %s-FIN", GCP_JWT);
   // mqtt_client_config.client_id = IOTCORE_CLIENTID;

	ESP_LOGI(TAG, "JWT Token generado... ");
	return true;
}

void publicar_temperatura_task(void * parm)
{
    ESP_LOGI(TAG, "Ingresa a publicar_temperatura_task()");

    BaseType_t xStatus;
    const TickType_t xTicksToWait = pdMS_TO_TICKS( 1000 );
    
    Data_t receive_dataSensor_queue; // estructura que almacena los datos a enviar
	
	char bufferJson[300];
	char bufferTopic[350];
	int msg_id;
	char buffer_temp_txt[15];
	char buffer_press_txt[15];
	char buffer_rssi_txt[15];
	char buffer_electroState_txt[15];
	
	float temp=24;
	float press=100000;
    int8_t rssi = 0;
    int8_t electroState = 0; // se recibe desde la cola 
    
	wifi_ap_record_t ap_info;
	    /* Configure the peripheral according to the LED type */
	while(1)
	{
	
        if (mqtt_client_connected == true) {
        // INICIO CICLO DE LECTURAS y publicaciones.
        vTaskDelay(TEMP_PUBLISH_INTERVAL_SECONDS * 1000 / portTICK_PERIOD_MS);
                        
        if( uxQueueMessagesWaiting( xQueue ) != 0 )
		{
		// verificamos si se recibieron datos en la cola desde bmp280_test
		printf( "Queue should have been empty!\n" );
		}  
	   
	    xStatus = xQueueReceive( xQueue, &receive_dataSensor_queue, xTicksToWait );
	 	
	 	if( xStatus == pdPASS )
		{
		/* Data was successfully received from the queue, print out the received value. */
		temp=receive_dataSensor_queue.temperature;
		press=receive_dataSensor_queue.pressure;
		printf( "Lectura desde Queue-> Pressure: %.2f Pa, Temperature: %.2f C", press,temp);
		printf("\n");
		}
		else
		{
		/* Data was not received */
		printf( "Could not receive from the queue.\n" );
		}
		/*******************************************************/
		if( uxQueueMessagesWaiting( xQueue1 ) != 0 )
		{
		// verificamos si se recibieron datos en la cola desde bmp280_test
		printf( "Queue should have been empty!\n" );
		}  
	    xStatus = xQueueReceive( xQueue1, &electroState, xTicksToWait );
	 	
	 	if( xStatus == pdPASS )
		{
		/* Data was successfully received from the queue, print out the received value. */
		printf( "Activar electroválvula=%i\r\n",electroState);
		
		xStatus = xQueueSendToBack( xQueue2, &(electroState), 300);
		if( xStatus != pdPASS )
		{
		/* The send operation could not complete because the queue was full -
		this must be an error as the queue should never contain more than
		one item! */
		printf( "Could not send to the queue.\n" );
		} 

		}
		else
		{
		/* Data was not received */
		printf( "Could not receive from the queue.\n" );
		}  
		/*******************************************************/
		  
		        
            // Consulto al modulo el nivel de señal que esta recibiendo.
            esp_wifi_sta_get_ap_info(&ap_info);
            rssi = ap_info.rssi;
            ESP_LOGI(TAG, "RSSI: %d ", ap_info.rssi);

            // Convertir a texto los valores.
            snprintf(buffer_temp_txt, sizeof(buffer_temp_txt), "%.2f", temp);
            snprintf(buffer_press_txt, sizeof(buffer_press_txt), "%.2f", press);
            snprintf(buffer_rssi_txt, sizeof(buffer_rssi_txt), "%d", rssi);
            snprintf(buffer_electroState_txt, sizeof(buffer_electroState_txt), "%d", electroState);
            ESP_LOGI("Datos a enviar: ", "T %s - Pa %s - RSSI %s - electroEstate %s ", buffer_temp_txt, buffer_press_txt, buffer_rssi_txt, buffer_electroState_txt);
          
                bufferJson[0] = 0;

                strcat(bufferJson, "{ \"dev_id\": ");
                strcat(bufferJson, IOTCORE_DEVICE_NUMBER);
                strcat(bufferJson, ", \"temperatura\": ");
                strcat(bufferJson, buffer_temp_txt);
                strcat(bufferJson, ", \"pressure\": ");
                strcat(bufferJson, buffer_press_txt);
                strcat(bufferJson, ", \"rssi\": ");
                strcat(bufferJson, buffer_rssi_txt);
                strcat(bufferJson, ", \"electroState\": ");
                strcat(bufferJson, buffer_electroState_txt);
                strcat(bufferJson, " }");

                ESP_LOGI("JSON enviado:", " %s ", bufferJson);

                bufferTopic[0] = 0;
                //strcat(bufferTopic, "/topic/qos1");
                //strcat(bufferTopic, IOTCORE_DEVICEID);
                strcat(bufferTopic, "/topic/dispositivos");
                ESP_LOGI("JSON enviado:", " %s ", bufferTopic);
                msg_id = esp_mqtt_client_publish(cliente, bufferTopic, bufferJson, 0, 1, 0);
                
                ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
            // FIN CICLO LECTURA Y ENVIO    
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
    vTaskDelete(NULL);
}

void bmp280_test(void *pvParameters)
{
    BaseType_t xStatus;
    Data_t send_dataSensor_queue; // estructura para almacenar los datos a enviar a la cola

    bmp280_params_t params;
    bmp280_init_default_params(&params);
    bmp280_t dev;
    memset(&dev, 0, sizeof(bmp280_t));

    ESP_ERROR_CHECK(bmp280_init_desc(&dev, BMP280_I2C_ADDRESS_0, 0, CONFIG_EXAMPLE_I2C_MASTER_SDA, CONFIG_EXAMPLE_I2C_MASTER_SCL));
    ESP_ERROR_CHECK(bmp280_init(&dev, &params));

    bool bme280p = dev.id == BME280_CHIP_ID;
    printf("BMP280: found %s\n", bme280p ? "BME280" : "BMP280");

    float pressure, temperature, humidity;

    while (1)
    {   	
        vTaskDelay(pdMS_TO_TICKS(500));
        if (bmp280_read_float(&dev, &temperature, &pressure, &humidity) != ESP_OK)
        {
            printf("Temperature/pressure reading failed\n");
            continue;
        }
    	//cargamos los datos de presión y temperatura a la cola
   	 	send_dataSensor_queue.temperature=temperature;
		send_dataSensor_queue.pressure=pressure;
		xStatus = xQueueSendToBack( xQueue, &(send_dataSensor_queue), 300);
		if( xStatus != pdPASS )
		{
		/* The send operation could not complete because the queue was full -
		this must be an error as the queue should never contain more than
		one item! */
		printf( "Could not send to the queue.\n" );
		} 
	
   		printf("Lectura del sensor-> Pressure: %.2f Pa, Temperature: %.2f C", pressure, temperature);
    	if (bme280p)
        printf(", Humidity: %.2f\n", humidity);
    	else
    	printf("\n");
    }
}

void led_task(void *pvParameters)
{
    BaseType_t xStatus;
    static uint8_t s_led_state = 0;
    const TickType_t xTicksToWait = pdMS_TO_TICKS( 1000 );
    gpio_reset_pin(BLINK_GPIO);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);

    while (1) {
    /* Set the GPIO level according to the state (LOW or HIGH)*/
     if( uxQueueMessagesWaiting( xQueue2 ) != 0 )
		{
		// verificamos si se recibieron datos en la cola desde bmp280_test
		printf( "Queue should have been empty!\n" );
		}  
	   
	    xStatus = xQueueReceive( xQueue2, &s_led_state, xTicksToWait );
	 	
	 	if( xStatus == pdPASS )
		{
		/* Data was successfully received from the queue, print out the received value. */

		printf("\n");
		}
		else
		{
		/* Data was not received */
		printf( "Could not receive from the queue.\n" );
		}
		    gpio_set_level(BLINK_GPIO, s_led_state);
    }
}

