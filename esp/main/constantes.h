#ifndef CONSTANTES_H
#define CONSTANTES_H

#include "driver/gpio.h"

#define BOTAO                    0
#define LED                      2
#define SENSOR_DHT11    GPIO_NUM_4

#define MATRICULA       "170050394"

#define IDENTIFICADOR_TEMPERATURA       0
#define IDENTIFICADOR_UMIDADE           1
#define IDENTIFICADOR_ESTADO_ENTRADA    2
#define IDENTIFICADOR_ESTADO_SAIDA      3

#define TEMPO_ENVIO_MQTT    3 // segundos

#endif