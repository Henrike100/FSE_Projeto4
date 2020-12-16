#include "utilidades.h"

char* mensagem_inicializacao(const uint8_t mac_address) {
    char *string = NULL;
    cJSON *monitor = cJSON_CreateObject();

    if (cJSON_AddNumberToObject(monitor, "ID", mac_address) == NULL) {
        cJSON_Delete(monitor);
        return "";
    }

    string = cJSON_Print(monitor);
    if (string == NULL) {
        cJSON_Delete(monitor);
        return "";
    }

    cJSON_Delete(monitor);
    return string;
}

void pegar_comodo(const char * const monitor, char *comodo) {
    cJSON *monitor_json = cJSON_Parse(monitor);
    if (monitor_json == NULL) {
        cJSON_Delete(monitor_json);
        return;
    }

    const cJSON *name = NULL;
    name = cJSON_GetObjectItemCaseSensitive(monitor_json, "Comodo");

    if (!cJSON_IsString(name) || (name->valuestring == NULL)) {
        printf("Erro\n");
        cJSON_Delete(monitor_json);
        return;
    }

    strcpy(comodo, name->valuestring);
    cJSON_Delete(monitor_json);
}

char* transformar_mensagem_para_JSON(const int identificador, uint8_t mac_address, const char* comodo, const int dado) {
    char *string = NULL;
    cJSON *monitor = cJSON_CreateObject();

    if (cJSON_AddNumberToObject(monitor, "ID", mac_address) == NULL) {
        cJSON_Delete(monitor);
        return "";
    }

    if (cJSON_AddStringToObject(monitor, "tipo", IDENTIFICADOR_TEMPERATURA ? "temperatura" : "umidade") == NULL) {
        cJSON_Delete(monitor);
        return "";
    }

    if (cJSON_AddNumberToObject(monitor, "valor", dado) == NULL) {
        cJSON_Delete(monitor);
        return "";
    }

    string = cJSON_Print(monitor);
    if (string == NULL) {
        cJSON_Delete(monitor);
        return "";
    }

    cJSON_Delete(monitor);
    return string;
}

/* Modelo de mensagens

Inicializacao (enviar):
{
    "ID": mac_address
}

Inicializacao (receber):
{
    "Comodo": comodo
}

Temperatura:
{
    "ID": mac_address,
    "tipo": "temperatura",
    "valor": dado
}

Umidade:
{
    "ID": mac_address,
    "tipo": "umidade",
    "valor": dado
}

Entrada:
{
    "ID": mac_address,
    "tipo": "entrada",
    "valor": dado
}

Saida:
{
    "ID": mac_address,
    "tipo": "saida",
    "valor": dado
}

*/