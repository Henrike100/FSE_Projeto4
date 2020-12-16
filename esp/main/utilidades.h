#ifndef UTILIDADES_H
#define UTILIDADES_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "cJSON.h"
#include "constantes.h"

char* mensagem_inicializacao(const uint8_t mac_address);
void pegar_comodo(const char * const monitor, char *comodo);
char* transformar_mensagem_para_JSON(const int identificador, uint8_t mac_address, const char* comodo, const int dado);

#endif