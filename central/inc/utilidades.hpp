#ifndef UTILIDADES_HPP
#define UTILIDADES_HPP

#include <string>
#include <string.h>
#include "cJSON.hpp"

using namespace std;

int pegar_mac_address(const char * const monitor);
char* transformar_comodo_para_JSON(string comodo);
int pegar_dados(const char * const monitor, int *mac_address, char *tipo, int *valor);

#endif // UTILIDADES_HPP