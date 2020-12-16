#include "utilidades.hpp"

int pegar_mac_address(const char * const monitor) {
    cJSON *monitor_json = cJSON_Parse(monitor);
    if(monitor_json == NULL) {
        return -2;
    }

    const cJSON *id = NULL;
    id = cJSON_GetObjectItemCaseSensitive(monitor_json, "ID");

    if(!cJSON_IsNumber(id)) {
        cJSON_Delete(monitor_json);
        return -3;
    }

    cJSON_Delete(monitor_json);
    return id->valueint;
}

char* transformar_comodo_para_JSON(string comodo) {
    cJSON *monitor = cJSON_CreateObject();
    char *stringJSON = NULL;

    if (cJSON_AddStringToObject(monitor, "Comodo", comodo.c_str()) == NULL) {
        cJSON_Delete(monitor);
        return stringJSON;
    }

    stringJSON = cJSON_Print(monitor);

    cJSON_Delete(monitor);
    return stringJSON;
}

int pegar_dados(const char * const monitor, int *mac_address, char *tipo, int *valor) {
    cJSON *monitor_json = cJSON_Parse(monitor);
    if(monitor_json == NULL) {
        return -4;
    }

    const cJSON *id = NULL;
    id = cJSON_GetObjectItemCaseSensitive(monitor_json, "ID");

    if(!cJSON_IsNumber(id)) {
        cJSON_Delete(monitor_json);
        return -5;
    }

    *mac_address = id->valueint;

    const cJSON *name = NULL;
    name = cJSON_GetObjectItemCaseSensitive(monitor_json, "tipo");
    if (!cJSON_IsString(name) or (name->valuestring == NULL)) {
        cJSON_Delete(monitor_json);
        return -6;
    }

    strcpy(tipo, name->valuestring);

    const cJSON *val = NULL;
    val = cJSON_GetObjectItemCaseSensitive(monitor_json, "valor");

    if(!cJSON_IsNumber(val)) {
        cJSON_Delete(monitor_json);
        return -7;
    }

    *valor = val->valueint;

    cJSON_Delete(monitor_json);
    return 0;
}

char* transformar_comando_mudar_LED_para_JSON(const int proximo_estado) {
    cJSON *monitor = cJSON_CreateObject();
    char *stringJSON = NULL;

    if (cJSON_AddNumberToObject(monitor, "LED", proximo_estado) == NULL) {
        cJSON_Delete(monitor);
        return stringJSON;
    }

    stringJSON = cJSON_Print(monitor);

    cJSON_Delete(monitor);
    return stringJSON;
}
