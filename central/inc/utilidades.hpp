#ifndef UTILIDADES_HPP
#define UTILIDADES_HPP

#include <string>
#include <string.h>
#include <vector>
#include <unistd.h>
#include "bme280.hpp"
#include "cJSON.hpp"

using namespace std;

struct identifier {
    uint8_t dev_addr;
    int8_t fd;
};

void pegar_mac_address(const char * const monitor, char *mac);
char* transformar_comodo_para_JSON(string comodo);
int pegar_dados(const char * const monitor, string *mac_address, char *tipo, int *valor);
char* transformar_comando_mudar_LED_para_JSON(const int proximo_estado);
void salvar_lista_comodos(const vector<string>& lista_comodos);

int8_t user_i2c_read(uint8_t reg_addr, uint8_t *data, uint32_t len, void *intf_ptr);
void user_delay_us(uint32_t period, void *intf_ptr);
int8_t user_i2c_write(uint8_t reg_addr, const uint8_t *data, uint32_t len, void *intf_ptr);

#endif // UTILIDADES_HPP