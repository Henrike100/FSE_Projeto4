#include "utilidades.hpp"

void pegar_mac_address(const char * const monitor, char *mac) {
    cJSON *monitor_json = cJSON_Parse(monitor);
    if(monitor_json == NULL) {
        return;
    }

    const cJSON *id = NULL;
    id = cJSON_GetObjectItemCaseSensitive(monitor_json, "ID");

    if (!cJSON_IsString(id) or (id->valuestring == NULL)) {
        cJSON_Delete(monitor_json);
        return;
    }

    strcpy(mac, id->valuestring);

    cJSON_Delete(monitor_json);
    return;
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

int pegar_dados(const char * const monitor, string *mac_address, char *tipo, int *valor) {
    cJSON *monitor_json = cJSON_Parse(monitor);
    if(monitor_json == NULL) {
        return -4;
    }

    const cJSON *id = NULL;
    id = cJSON_GetObjectItemCaseSensitive(monitor_json, "ID");

    if (!cJSON_IsString(id) or (id->valuestring == NULL)) {
        cJSON_Delete(monitor_json);
        return -6;
    }

    string temp(id->valuestring);
    *mac_address = temp;

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

void salvar_lista_comodos(const vector<string>& lista_comodos) {
    FILE *lista_comodos_file;
    lista_comodos_file = fopen("comodos.txt", "w+");

    fprintf(lista_comodos_file, "%d\n", lista_comodos.size());

    for(int i = 0; i < lista_comodos.size(); ++i) {
        fprintf(lista_comodos_file, "%s\n", lista_comodos[i].c_str());
    }

    fclose(lista_comodos_file);
}

int8_t user_i2c_read(uint8_t reg_addr, uint8_t *data, uint32_t len, void *intf_ptr) {
    struct identifier id;

    id = *((struct identifier *)intf_ptr);

    write(id.fd, &reg_addr, 1);
    read(id.fd, data, len);

    return 0;
}

void user_delay_us(uint32_t period, void *intf_ptr) {
    usleep(period);
}

int8_t user_i2c_write(uint8_t reg_addr, const uint8_t *data, uint32_t len, void *intf_ptr) {
    return BME280_OK;
}
