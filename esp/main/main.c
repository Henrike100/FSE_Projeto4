#include <stdio.h>
#include <string.h>
#include "nvs.h"
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "wifi.h"
#include "mqtt.h"
#include "dht11.h"
#include "utilidades.h"

xQueueHandle filaDeInterrupcao;
xSemaphoreHandle conexaoWifiSemaphore;
xSemaphoreHandle conexaoMQTTSemaphore;
char mac_address[18];

char topicoTemperatura[50], topicoUmidade[50];
char topicoEntrada[50];

static void IRAM_ATTR gpio_isr_handler(void *args) {
    int pino = (int)args;
    xQueueSendFromISR(filaDeInterrupcao, &pino, NULL);
}

void trataInterrupcaoBotao(void *params) {
    int pino;

    while(true) {
        if(xQueueReceive(filaDeInterrupcao, &pino, portMAX_DELAY)) {
            // De-bouncing
            int estado = gpio_get_level(pino);
            if(estado == 1) {
                if(comodo_definido) {
                    mqtt_envia_mensagem(
                        topicoEntrada,
                        transformar_mensagem_para_JSON(IDENTIFICADOR_ESTADO_ENTRADA, mac_address, comodo, 0)
                    );
                }

                gpio_isr_handler_remove(pino);
                while(gpio_get_level(pino) == estado) {
                    vTaskDelay(50 / portTICK_PERIOD_MS);
                }

                if(comodo_definido) {
                    mqtt_envia_mensagem(
                        topicoEntrada,
                        transformar_mensagem_para_JSON(IDENTIFICADOR_ESTADO_ENTRADA, mac_address, comodo, 1)
                    );
                }

                // Habilitar novamente a interrupção
                vTaskDelay(50 / portTICK_PERIOD_MS);
                gpio_isr_handler_add(pino, gpio_isr_handler, (void *) pino);
            }
        }
    }
}

void conectadoWifi(void * params) {
    while(true) {
        if(xSemaphoreTake(conexaoWifiSemaphore, portMAX_DELAY)) {
            // Processamento Internet
            mqtt_start();
        }
    }
}

void le_valor_nvs(char *comodo, int *status) {
    nvs_handle particao_padrao_handle;
    esp_err_t res_nvs = nvs_open("armazenamento", NVS_READONLY, &particao_padrao_handle);
    if(res_nvs == ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGE("NVS", "Namespace: armazenamento, não encontrado");
        *status = 1;
    }
    else {
        size_t required_size;
        nvs_get_str(particao_padrao_handle, "comodo", NULL, &required_size);
        char* temp = malloc(required_size);
        esp_err_t res = nvs_get_str(particao_padrao_handle, "comodo", temp, &required_size);
        switch (res) {
        case ESP_OK:
            break;
        case ESP_ERR_NOT_FOUND:
            ESP_LOGE("NVS", "Valor não encontrado");
            *status = 2;
            return;
        default:
            ESP_LOGE("NVS", "Erro ao acessar o NVS (%s)", esp_err_to_name(res));
            *status = 3;
            return;
        }

        nvs_close(particao_padrao_handle);
        strcpy(comodo, temp);
    }
}

void salva_comodo() {
    nvs_handle particao_padrao_handle;
    esp_err_t res_nvs = nvs_open("armazenamento", NVS_READWRITE, &particao_padrao_handle);

    if(res_nvs == ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGE("NVS", "Namespace: armazenamento, não encontrado");
    }

    esp_err_t res = nvs_set_str(particao_padrao_handle, "comodo", comodo);
    if(res != ESP_OK) {
        ESP_LOGE("NVS", "Não foi possível escrever no NVS (%s)", esp_err_to_name(res));
    }

    nvs_commit(particao_padrao_handle);
    nvs_close(particao_padrao_handle);
}

void trataComunicacaoComServidor(void * params) {
    if(xSemaphoreTake(conexaoMQTTSemaphore, portMAX_DELAY)) {
        sprintf(topicoTemperatura, "fse2020/%s/%s/temperatura", MATRICULA, comodo);
        sprintf(topicoUmidade, "fse2020/%s/%s/umidade", MATRICULA, comodo);
        sprintf(topicoEntrada, "fse2020/%s/%s/estado", MATRICULA, comodo);

        salva_comodo();

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
    uint8_t temp_mac[6];
    ret = esp_efuse_mac_get_default(temp_mac);
    transformar_mac(&temp_mac[0], mac_address);

    // Inicia LED
    gpio_pad_select_gpio(LED);
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);

    // Inicia Botao
    gpio_pad_select_gpio(BOTAO);
    gpio_set_direction(BOTAO, GPIO_MODE_INPUT);
    gpio_pulldown_en(BOTAO);
    gpio_pullup_dis(BOTAO);
    gpio_set_intr_type(BOTAO, GPIO_INTR_POSEDGE);
    filaDeInterrupcao = xQueueCreate(10, sizeof(int));

    conexaoWifiSemaphore = xSemaphoreCreateBinary();
    conexaoMQTTSemaphore = xSemaphoreCreateBinary();

    int status;
    le_valor_nvs(comodo, &status);

    if(status == 0) {
        comodo_definido = 1;
        xSemaphoreGive(conexaoMQTTSemaphore);
    }
    else {
        printf("Erro (NVS): %d\n", status);
    }

    wifi_start();

    xTaskCreate(trataInterrupcaoBotao, "TrataBotao", 2048, NULL, 1, NULL);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(BOTAO, gpio_isr_handler, (void *) BOTAO);

    xTaskCreate(&conectadoWifi,  "Conexão ao MQTT", 4096, NULL, 1, NULL);
    xTaskCreate(&trataComunicacaoComServidor, "Comunicação com Broker", 4096, NULL, 1, NULL);
}
