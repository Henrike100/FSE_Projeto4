#include "utilidades.h"

char* transformar_mensagem_para_JSON(const int identificador, uint8_t mac_address, const char* comodo, const int dado) {
    char *string = NULL;
    cJSON *monitor = cJSON_CreateObject();

    if (cJSON_AddNumberToObject(monitor, "ID", mac_address) == NULL) {
        cJSON_Delete(monitor);
        return "";
    }

    if (cJSON_AddNumberToObject(monitor, identificador == IDENTIFICADOR_TEMPERATURA ? "T" : "U", dado) == NULL) {
        cJSON_Delete(monitor);
        return "";
    }

    string = cJSON_Print(monitor);
    if (string == NULL) {
        return "";
    }

    cJSON_Delete(monitor);
    return string;
}

/* Modelo de mensagens

Temperatura:
{
    "ID": mac_address
    "T": dado
}

Umidade:
{
    "ID": mac_address
    "U": dado
}

Entrada:
{
    "ID": mac_address
    "E": dado
}

Saida:
{
    "ID": mac_address
    "S": dado
}

*/