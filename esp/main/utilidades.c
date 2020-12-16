#include "utilidades.h"

char* mensagem_inicializacao(const char * const mac_address) {
    char *string = NULL;
    cJSON *monitor = cJSON_CreateObject();

    if (cJSON_AddStringToObject(monitor, "ID", mac_address) == NULL) {
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

char* transformar_mensagem_para_JSON(const int identificador, const char * const  mac_address, const char* comodo, const int dado) {
    char *string = NULL;
    cJSON *monitor = cJSON_CreateObject();

    if (cJSON_AddStringToObject(monitor, "ID", mac_address) == NULL) {
        cJSON_Delete(monitor);
        return "";
    }

    if (cJSON_AddStringToObject(
        monitor, "tipo",
        identificador == IDENTIFICADOR_TEMPERATURA ? "temperatura" : 
        identificador == IDENTIFICADOR_UMIDADE ? "umidade" :
        identificador == IDENTIFICADOR_ESTADO_ENTRADA ? "entrada" : "saida") == NULL) {
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

int novo_estado_LED(const char * const monitor) {
    cJSON *monitor_json = cJSON_Parse(monitor);
    if (monitor_json == NULL) {
        cJSON_Delete(monitor_json);
        return 0;
    }

    const cJSON *proximo_estado = NULL;
    proximo_estado = cJSON_GetObjectItemCaseSensitive(monitor_json, "LED");

    if(!cJSON_IsNumber(proximo_estado)) {
        cJSON_Delete(monitor_json);
        return 0;
    }

    cJSON_Delete(monitor_json);
    return proximo_estado->valueint;
}

void transformar_mac(const uint8_t *temp, char *mac_address) {
    sprintf(mac_address, "%02x:%02x:%02x:%02x:%02x:%02x",
        temp[0], temp[1], temp[2], temp[3], temp[4], temp[5]
    );
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

Comando de mudar LED:
{
    "LED": proximo_estado
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