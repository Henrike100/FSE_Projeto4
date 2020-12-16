#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
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
#include "constantes.h"
#include "utilidades.h"

#include "mqtt.h"

#define TAG "MQTT"

extern xSemaphoreHandle conexaoMQTTSemaphore;
extern uint8_t mac_address;
char topico_inicializacao[50];
char comodo[20];
int comodo_definido = 0;

esp_mqtt_client_handle_t client;

static esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event) {
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;

    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            //ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            if(!comodo_definido) {
                msg_id = esp_mqtt_client_publish(client, topico_inicializacao, mensagem_inicializacao(mac_address), 0, 1, 0);
                msg_id = esp_mqtt_client_subscribe(client, topico_inicializacao, 0);
            }
            break;
        case MQTT_EVENT_DISCONNECTED:
            //ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            break;
        case MQTT_EVENT_SUBSCRIBED:
            //ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            //ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:
            //ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_DATA:
            //ESP_LOGI(TAG, "MQTT_EVENT_DATA");

            // Se o servidor central mandou o comodo do dispositivo
            if(!comodo_definido) {
                pegar_comodo(event->data, comodo);
                comodo_definido = 1;

                // Agora o dispositivo pode publicar
                xSemaphoreGive(conexaoMQTTSemaphore);
            }
            // Caso contrário, o servidor central pediu para ligar ou desligar o LED
            else {
                // pegar comando de ligar/desligar
                
                // mudar LED

                // mandar resposta
            }
            break;
        case MQTT_EVENT_ERROR:
            //ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            break;
        default:
            //ESP_LOGI(TAG, "Other event id:%d", event->event_id);
            break;
    }
    return ESP_OK;
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    //ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    mqtt_event_handler_cb(event_data);
}

void mqtt_start() {
    esp_mqtt_client_config_t mqtt_config = {
        .uri = "mqtt://mqtt.eclipseprojects.io",
    };
    sprintf(topico_inicializacao, "fse2020/%s/dispositivos/%d", MATRICULA, mac_address);
    client = esp_mqtt_client_init(&mqtt_config);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
    esp_mqtt_client_start(client);
}

void mqtt_envia_mensagem(char * topico, char * mensagem) {
    int message_id = esp_mqtt_client_publish(client, topico, mensagem, 0, 1, 0);
    //ESP_LOGI(TAG, "Mesnagem enviada, ID: %d", message_id);
}
