#ifndef UTILIDADES_HPP
#define UTILIDADES_HPP

#include <string>
#include <string.h>
#include <vector>
#include "cJSON.hpp"

using namespace std;

void pegar_mac_address(const char * const monitor, char *mac);
char* transformar_comodo_para_JSON(string comodo);
int pegar_dados(const char * const monitor, string *mac_address, char *tipo, int *valor);
char* transformar_comando_mudar_LED_para_JSON(const int proximo_estado);
void salvar_lista_comodos(const vector<string>& lista_comodos);

#endif // UTILIDADES_HPP