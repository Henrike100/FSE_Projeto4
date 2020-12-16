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

char topico[50];

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
        while(true) {
            struct dht11_reading dado = DHT11_read();

            // Envia temperatura
            mqtt_envia_mensagem(
                topico,
                transformar_mensagem_para_JSON(IDENTIFICADOR_TEMPERATURA, mac_address, "sala", dado.temperature)
            );

            // Envia Umidade
            mqtt_envia_mensagem(
                topico,
                transformar_mensagem_para_JSON(IDENTIFICADOR_UMIDADE, mac_address, "sala", dado.humidity)
            );

            vTaskDelay(3000 / portTICK_PERIOD_MS);
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

    sprintf(topico, "fse2020/%s/dispositivos/%d", MATRICULA, mac_address);
    
    conexaoWifiSemaphore = xSemaphoreCreateBinary();
    conexaoMQTTSemaphore = xSemaphoreCreateBinary();
    wifi_start();

    xTaskCreate(&conectadoWifi,  "Conexão ao MQTT", 4096, NULL, 1, NULL);
    xTaskCreate(&trataComunicacaoComServidor, "Comunicação com Broker", 4096, NULL, 1, NULL);
}
