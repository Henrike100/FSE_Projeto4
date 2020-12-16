#include <stdio.h>
#include <string.h>
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "freertos/semphr.h"

#include "wifi.h"
#include "mqtt.h"
#include "dht11.h"
#include "utilidades.h"

xSemaphoreHandle conexaoWifiSemaphore;
xSemaphoreHandle conexaoMQTTSemaphore;
uint8_t mac_address;

char topicoTemperatura[50], topicoUmidade[50];

void conectadoWifi(void * params) {
    while(true) {
        if(xSemaphoreTake(conexaoWifiSemaphore, portMAX_DELAY)) {
            // Processamento Internet
            mqtt_start();
        }
    }
}

void trataComunicacaoComServidor(void * params) {
    if(xSemaphoreTake(conexaoMQTTSemaphore, portMAX_DELAY)) {
        sprintf(topicoTemperatura, "fse2020/%s/%s/temperatura", MATRICULA, comodo);
        sprintf(topicoUmidade, "fse2020/%s/%s/umidade", MATRICULA, comodo);

        while(true) {
            struct dht11_reading dado = DHT11_read();

            // Envia temperatura
            mqtt_envia_mensagem(
                topicoTemperatura,
                transformar_mensagem_para_JSON(IDENTIFICADOR_TEMPERATURA, mac_address, comodo, dado.temperature)
            );

            // Envia Umidade
            mqtt_envia_mensagem(
                topicoUmidade,
                transformar_mensagem_para_JSON(IDENTIFICADOR_UMIDADE, mac_address, comodo, dado.humidity)
            );

            vTaskDelay(TEMPO_ENVIO_MQTT * 1000 / portTICK_PERIOD_MS);
        }
    }
}

void app_main(void) {
    // Inicializa o NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Inicializa Sensor DHT11
    DHT11_init(SENSOR_DHT11);

    // Pega MAC_ADDRESS
    ret = esp_efuse_mac_get_default(&mac_address);

    conexaoWifiSemaphore = xSemaphoreCreateBinary();
    conexaoMQTTSemaphore = xSemaphoreCreateBinary();
    wifi_start();

    xTaskCreate(&conectadoWifi,  "Conexão ao MQTT", 4096, NULL, 1, NULL);
    xTaskCreate(&trataComunicacaoComServidor, "Comunicação com Broker", 4096, NULL, 1, NULL);
}
