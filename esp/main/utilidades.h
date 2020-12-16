#ifndef UTILIDADES_H
#define UTILIDADES_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "cJSON.h"
#include "constantes.h"

char* mensagem_inicializacao(const char * const mac_address);
void pegar_comodo(const char * const monitor, char *comodo);
char* transformar_mensagem_para_JSON(const int identificador, const char * const  mac_address, const char* comodo, const int dado);
int novo_estado_LED(const char * const monitor);
void transformar_mac(const uint8_t *temp, char *mac_address);

#endif