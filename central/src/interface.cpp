#include "interface.hpp"

FILE *file;

mutex mtx_interface;
mutex mtx_csv;
bool programa_pode_continuar = true;
const int quantidade_dispositivos_padrao = 9;

ESP32 dispositivos_cadastrados[5];
int quantidade_dispositivos_cadastrados = 0;

char dispositivos_esperando[5][18] = {0};
int quantidade_dispositivos_esperando = 0;

vector<string> lista_comodos;

MQTTClient client_ESP;
MQTTClient client_comodos[5];

float temperatura = -1.0f;
float umidade = -1.0f;

struct bme280_dev dev;
struct identifier id;

int valores[] = {
    0, // Alarme
    0, // Lampada Cozinha
    0, // Lampada Sala
    0, // Sensor Sala
    0, // Sensor Cozinha
    0, // Sensor Porta Cozinha
    0, // Sensor Janela Cozinha
    0, // Sensor Porta Sala
    0, // Sensor Janela Sala
};

int modos[] = {
    SAIDA, // Alarme
    SAIDA, // Lampada Cozinha
    SAIDA, // Lampada Sala
    ENTRADA, // Sensor Sala
    ENTRADA, // Sensor Cozinha
    ENTRADA, // Sensor Porta Cozinha
    ENTRADA, // Sensor Janela Cozinha
    ENTRADA, // Sensor Porta Sala
    ENTRADA, // Sensor Janela Sala
};

void signal_handler(int signum) {
    programa_pode_continuar = false;
}

void delivered(void *context, MQTTClient_deliveryToken dt) {
    // fazer nada
}

void delivered_comodo(void *context, MQTTClient_deliveryToken dt) {
    // fazer nada
}

void connlost(void *context, char *cause) {
    programa_pode_continuar = false;
}

void enviar_comodo_para_ESP(const string comodo, const string mac_adress) {
    char topico[100];
    sprintf(topico, "fse2020/%s/dispositivos/%s", MATRICULA, mac_adress.c_str());

    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    MQTTClient_deliveryToken token;

    char mensagem[100];
    strcpy(mensagem, transformar_comodo_para_JSON(comodo));

    pubmsg.payload = mensagem;
    pubmsg.payloadlen = strlen(mensagem);
    pubmsg.qos = 1;
    pubmsg.retained = 0;

    MQTTClient_publishMessage(client_ESP, topico, &pubmsg, &token);
}

int msgarrvd_comodo(void *context, char *topicName, int topicLen, MQTTClient_message *message) {
    char *payloadptr = reinterpret_cast<char*>(message->payload);

    string mac_address;
    int valor;
    char tipo[50];
    int erro = pegar_dados(payloadptr, &mac_address, tipo, &valor);

    if(erro) {
        return 1;
    }

    int i = 0;
    for(i = 0; i < quantidade_dispositivos_cadastrados; ++i) {
        if(dispositivos_cadastrados[i].getMacAdress() == mac_address)
            break;
    }

    if(i == quantidade_dispositivos_cadastrados) {
        FILE *lista;
        lista = fopen("esps.txt", "r");

        if(lista == NULL) {
            return 1;
        }

        int quantidade;
        fscanf(lista, " %d", &quantidade);

        char mac[18], comodo[20], nome[100];

        int j = 0;
        for(j = 0; j < quantidade; ++j) {
            fscanf(lista, " %[^\n]", mac);
            fscanf(lista, " %[^\n]", comodo);
            fscanf(lista, " %[^\n]", nome);

            string temp_mac(mac);

            if(temp_mac == mac_address)
                break;
        }

        if(j == quantidade) {
            return 1;
        }
        else {
            string temp_mac(mac), temp_comodo(comodo), temp_nome(nome);
            adicionar_novo_dispositivo(-1, temp_comodo, temp_nome, temp_mac);
        }
    }

    if(strcmp(tipo, "temperatura") == 0) {
        dispositivos_cadastrados[i].setTemperatura(valor);
    }
    else if(strcmp(tipo, "umidade") == 0) {
        dispositivos_cadastrados[i].setUmidade(valor);
    }
    else if(strcmp(tipo, "entrada") == 0) {
        dispositivos_cadastrados[i].setEntrada(valor);
    }
    else if(strcmp(tipo, "saida") == 0) {
        dispositivos_cadastrados[i].setSaida(valor);
    }

    return 1;
}

void inscrever_no_comodo(const string comodo) {
    int quantidade_comodos = lista_comodos.size();
    for(int i = 0; i < quantidade_comodos; ++i) {
        if(lista_comodos[i] == comodo)
            return;
    }

    char clientID[15];
    sprintf(clientID, "fse2020-%d", quantidade_comodos);

    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    MQTTClient_create(
        &client_comodos[quantidade_comodos],
        ADDRESS,
        clientID,
        MQTTCLIENT_PERSISTENCE_NONE,
        NULL
    );

    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;

    MQTTClient_setCallbacks(client_comodos[quantidade_comodos], NULL, connlost, msgarrvd_comodo, delivered);

    int rc;

    if ((rc = MQTTClient_connect(client_comodos[quantidade_comodos], &conn_opts)) != MQTTCLIENT_SUCCESS) {
        programa_pode_continuar = false;
        return;
    }

    char topico_comodo[50];
    sprintf(topico_comodo, "fse2020/%s/%s/#", MATRICULA, comodo.c_str());

    MQTTClient_subscribe(client_comodos[quantidade_comodos], topico_comodo, 1);

    lista_comodos.push_back(comodo);
    salvar_lista_comodos(lista_comodos);
}

void pegar_comodos_ja_cadastrados() {
    FILE *lista_comodos_file;
    lista_comodos_file = fopen("comodos.txt", "r");

    if(lista_comodos_file == NULL) {
        return;
    }

    char comodos[20];

    int quantidade;
    fscanf(lista_comodos_file, " %d", &quantidade);

    for(int i = 0; i < quantidade; ++i) {
        fscanf(lista_comodos_file, " %[^\n]", comodos);
        string temp(comodos);
        inscrever_no_comodo(temp);
    }

    fclose(lista_comodos_file);
}

void salvar_dispositivos_cadastrados() {
    FILE *lista_esps;
    lista_esps = fopen("esps.txt", "w+");

    fprintf(lista_esps, "%d\n", quantidade_dispositivos_cadastrados);

    for(int i = 0; i < quantidade_dispositivos_cadastrados; ++i) {
        ESP32 esp = dispositivos_cadastrados[i];

        fprintf(lista_esps, "%s\n", esp.getMacAdress().c_str());
        fprintf(lista_esps, "%s\n", esp.getComodo().c_str());
        fprintf(lista_esps, "%s\n", esp.getNome().c_str());
    }

    fclose(lista_esps);
}

void adicionar_novo_dispositivo(const int index, const string comodo, const string nome, const string mac_adress) {
    enviar_comodo_para_ESP(comodo, mac_adress);
    inscrever_no_comodo(comodo);

    dispositivos_cadastrados[quantidade_dispositivos_cadastrados] = ESP32(comodo, nome, mac_adress);
    quantidade_dispositivos_cadastrados++;

    if(index == -1)
        return;

    for(int i = index-1; i < quantidade_dispositivos_esperando-1; ++i) {
        strcpy(dispositivos_esperando[i], dispositivos_esperando[i+1]);
    }
    quantidade_dispositivos_esperando--;

    salvar_dispositivos_cadastrados();
}

int msgarrvd_inicializacao(void *context, char *topicName, int topicLen, MQTTClient_message *message) {

    char *payloadptr = reinterpret_cast<char*>(message->payload);

    char mac_address[18];
    pegar_mac_address(payloadptr, mac_address);

    if(strlen(mac_address) != 17) {
        return 1;
    }

    strcpy(dispositivos_esperando[quantidade_dispositivos_esperando], mac_address);
    quantidade_dispositivos_esperando++;

    return 1;
}

void connlost_comodo(void *context, char *cause) {
    programa_pode_continuar = false;
}

int iniciar_MQTT() {
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    MQTTClient_create(&client_ESP, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;

    int rc;

    MQTTClient_setCallbacks(client_ESP, NULL, connlost, msgarrvd_inicializacao, delivered);

    if((rc = MQTTClient_connect(client_ESP, &conn_opts)) != MQTTCLIENT_SUCCESS) {
        programa_pode_continuar = false;
        return rc;
    }

    MQTTClient_subscribe(client_ESP, TOPIC, 1);

    return 0;
}

void desativar_MQTT() {
    MQTTClient_disconnect(client_ESP, 10000);
    MQTTClient_destroy(&client_ESP);
}

int abrir_csv() {
    file = fopen("arquivo.csv", "w+");
    if(file == NULL) {
        return 1;
    }

    if(fprintf(file, "Data/Hora, Fonte, Ocorrido\n") <= 0)
        return 2;

    return 0;
}

void fechar_csv() {
    fclose(file);
}

void limpar_menu(WINDOW *menu, const int num_lines) {
    mtx_interface.lock();

    for(int i = 3; i < num_lines-1; ++i) {
        wmove(menu, i, 1);
        wclrtoeol(menu);
    }

    mtx_interface.unlock();
}

void iniciar_menu(WINDOW *menu, string titulo) {
    mtx_interface.lock();

    const int line_size = getmaxx(menu);
    const string spaces((line_size-titulo.size())/2, ' ');

    mvwprintw(menu, 1, 1, "%s%s%s", spaces.c_str(), titulo.c_str(), spaces.c_str());
    mvwhline(menu, 2, 0, 0, line_size);
    box(menu, 0, 0);
    wrefresh(menu);

    mtx_interface.unlock();
}

void atualizar_menu_opcoes(WINDOW *menu) {
    mtx_interface.lock();
    const int line_size = getmaxx(menu);
    const int num_lines = getmaxy(menu);
    mtx_interface.unlock();

    limpar_menu(menu, num_lines);

    mtx_interface.lock();

    int linha_atual = 3;

    for(int index = 1; index <= quantidade_dispositivos_padrao; ++index, linha_atual += 2) {
        wmove(menu, linha_atual, 1);
        wclrtoeol(menu);

        if(modos[index-1] == ENTRADA) {
            mvwprintw(menu, linha_atual, 2, "%02d   --------", index);
        }
        else {
            mvwprintw(menu, linha_atual, 2, "%02d   %s", index, valores[index-1] == 0 ? "Ligar" : "Desligar");
        }
    }

    for(int i = 0; i < quantidade_dispositivos_cadastrados; ++i, linha_atual += 2) {
        wmove(menu, linha_atual, 1);
        wclrtoeol(menu);

	    ESP32 esp = dispositivos_cadastrados[i];
        int saida = esp.getSaida();

        mvwprintw(menu, linha_atual, 2, "%02d   %s", i+1+quantidade_dispositivos_padrao, saida < 0 ? " " : saida == 0 ? "Ligar" : "Desligar");
    }

    mvwvline(menu, 3, 5, 0, 35);
    for(int i = 3; i <= 29; i += 2) {
        mvwhline(menu, i+1, 0, 0, line_size);
    }

    box(menu, 0, 0);
    wrefresh(menu);
    
    mtx_interface.unlock();
}

void atualizar_menu_dispositivos(WINDOW *menu) {
    mtx_interface.lock();
    const int line_size = getmaxx(menu);
    const int num_lines = getmaxy(menu);
    mtx_interface.unlock();

    limpar_menu(menu, num_lines);
    box(menu, 0, 0);
    wrefresh(menu);

    mtx_interface.lock();

    int linha_atual = 3, index = 1;

    while(index <= quantidade_dispositivos_padrao) {
        int start = 1;
        if(!(index&1))
            start += (line_size/2);

        int name_size = strlen(dispositivos_padrao[index-1]);
        const string spaces(((line_size/2)-name_size)/2 - 3, ' ');

        mvwprintw(menu, linha_atual, start+1, "%02d%s%s%s", index, spaces.c_str(), dispositivos_padrao[index-1], spaces.c_str());
        mvwhline(menu, linha_atual+1, start, 0, line_size/2);

        if(modos[index-1] == ENTRADA) {
            mvwprintw(menu, linha_atual+2, start, "%s", valores[index-1] == 0 ? "Desligado" : "Ligado");
        }
        else {
            mvwprintw(menu, linha_atual+2, start, "---");
        }

        mvwvline(menu, linha_atual+2, start-1+(line_size)/8, 0, 1);

        if(modos[index-1] == SAIDA) {
            mvwprintw(menu, linha_atual+2, start+(line_size)/8, "%s", valores[index-1] == 0 ? "Desligado" : "Ligado");
        }
        else {
            mvwprintw(menu, linha_atual+2, start+(line_size)/8, "---");
        }

        mvwvline(menu, linha_atual+2, start-1+(line_size)/4, 0, 1);

        if(temperatura > 0) {
            mvwprintw(menu, linha_atual+2, start+(line_size)/4, "%.1f °C", temperatura);
        }
        else {
            mvwprintw(menu, linha_atual+2, start+(line_size)/4, " ");
        }

        mvwvline(menu, linha_atual+2, start-2+(3*line_size)/8, 0, 1);

        if(umidade > 0) {
            mvwprintw(menu, linha_atual+2, start-1+(3*line_size)/8, "%.1f ", umidade);
        }
        else {
            mvwprintw(menu, linha_atual+2, start-1+(3*line_size)/8, " ");
        }

        mvwhline(menu, linha_atual+3, start, 0, line_size/2);

        if(!(index&1))
            linha_atual += 4;

        index++;
    }

    int i = 1;

    while(i <= quantidade_dispositivos_cadastrados) {
        index = i+quantidade_dispositivos_padrao;
        int start = 1;
        if(!(index&1))
            start += (line_size/2);

        ESP32 esp = dispositivos_cadastrados[i-1];

        string name = esp.getNome();
        string entrada = esp.getEntrada() < 0 ? " " : esp.getEntrada() == 0 ? "Desligado" : "Ligado";
	    string saida = esp.getSaida() < 0 ? " " : esp.getSaida() == 0 ? "Desligado" : "Ligado";
	    string temperatura = esp.getTemperatura() + " °C";
	    string umidade = esp.getUmidade() + " %";

        const string spaces(((line_size/2)-name.size())/2 - 3, ' ');

        mvwprintw(menu, linha_atual, start+1, "%02d%s%s%s", index, spaces.c_str(), name.c_str(), spaces.c_str());
        mvwhline(menu, linha_atual+1, start, 0, line_size/2);

        mvwprintw(menu, linha_atual+2, start, "%s", entrada.c_str());

        mvwvline(menu, linha_atual+2, start-1+(line_size)/8, 0, 1);
        mvwprintw(menu, linha_atual+2, start+(line_size)/8, "%s", saida.c_str());

        mvwvline(menu, linha_atual+2, start-1+(line_size)/4, 0, 1);
        mvwprintw(menu, linha_atual+2, start+(line_size)/4, "%s", temperatura.c_str());

        mvwvline(menu, linha_atual+2, start-2+(3*line_size)/8, 0, 1);
        mvwprintw(menu, linha_atual+2, start-1+(3*line_size)/8, "%s", umidade.c_str());

        mvwhline(menu, linha_atual+3, start, 0, line_size/2);

        if(!(index&1))
            linha_atual += 4;

        i++;
    }

    int total_dispositivos = quantidade_dispositivos_padrao + quantidade_dispositivos_cadastrados;

    int barra_vertical = 4*(total_dispositivos/2);

    if(total_dispositivos&1) barra_vertical += 4;

    mvwvline(menu, 3, line_size/2, 0, barra_vertical);

    box(menu, 0, 0);
    wrefresh(menu);
    
    mtx_interface.unlock();
}

void atualizar_menu_solicitacoes(WINDOW *menu) {
    mtx_interface.lock();
    const int line_size = getmaxx(menu);
    const int num_lines = getmaxy(menu);
    mtx_interface.unlock();

    limpar_menu(menu, num_lines);

    mtx_interface.lock();

    // MAC ADRESS
    const string spaces((line_size-8-10)/2, ' ');
    const string spaces2((line_size-26)/2, ' ');

    mvwprintw(menu, 3, 1, " ID |%sMAC ADRESS%s", spaces.c_str(), spaces.c_str());
    mvwhline(menu, 4, 1, 0, line_size);

    int linha_atual = 5;

    for(int i = 0; i < quantidade_dispositivos_esperando; ++i, linha_atual += 2) {
        wmove(menu, linha_atual, 1);
        wclrtoeol(menu);

        mvwprintw(menu, linha_atual, 2, "%02d |%s%s%s", i+1, spaces2.c_str(), dispositivos_esperando[i], spaces2.c_str());
        mvwhline(menu, linha_atual+1, 0, 0, line_size);
    }

    box(menu, 0, 0);
    wrefresh(menu);
    
    mtx_interface.unlock();
}

void reiniciar_menu_ecolhas(WINDOW *menu, const int num_lines) {
    limpar_menu(menu, num_lines);

    mtx_interface.lock();

    mvwprintw(menu, 3, 2, "1. Mudar estado de um dispositivo");
    mvwprintw(menu, 4, 2, "2. Adicionar novo dispositvo");
    mvwprintw(menu, 5, 2, "0. Sair");

    mtx_interface.unlock();
}

void mudar_LED(int idx) {
    ESP32 esp = dispositivos_cadastrados[idx];
    char topico[100];
    sprintf(topico, "fse2020/%s/dispositivos/%s", MATRICULA, esp.getMacAdress().c_str());

    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    MQTTClient_deliveryToken token;

    char mensagem[100];
    strcpy(mensagem, transformar_comando_mudar_LED_para_JSON(1-esp.getSaida()));

    pubmsg.payload = mensagem;
    pubmsg.payloadlen = strlen(mensagem);
    pubmsg.qos = 1;
    pubmsg.retained = 0;

    MQTTClient_publishMessage(client_ESP, topico, &pubmsg, &token);
}

void mudar_saida(int opcao) {
    if(opcao == 1) {
        valores[0] = 1 - valores[0];
    }
    else if(opcao == 2) {
        valores[1] = 1 - valores[1];
        bcm2835_gpio_write(GPIO_LAMPADA_COZINHA, valores[1]);
    }
    else {
        valores[2] = 1 - valores[2];
        bcm2835_gpio_write(GPIO_LAMPADA_SALA, valores[2]);
    }
}

void mudar_estado_dispositivo(WINDOW *menu, const int num_lines) {
    limpar_menu(menu, num_lines);

    int opcao;
    bool invalid = false;

    do {
        mtx_interface.lock();

        const int total_dispositivos = quantidade_dispositivos_padrao + quantidade_dispositivos_cadastrados;

        if(invalid) {
            wmove(menu, 3, 1);
            wclrtoeol(menu);
            mvwprintw(menu, 4, 2, "Escolha deve estar entre 1 e %d", total_dispositivos);
        }

        mvwprintw(menu, 3, 2, "Digite o numero do dispositivo que deseja mudar: ");
        mtx_interface.unlock();
        mvwscanw(menu, 3, 51, "%d", &opcao);
        invalid = opcao < 1 || opcao > total_dispositivos;
    } while(invalid);

    if(opcao <= quantidade_dispositivos_padrao) {
        if(opcao <= 3) {
            mudar_saida(opcao);
            atualizar_csv(1, opcao, 1-valores[opcao-1]);
        }
    }
    else {
        int idx = opcao-1-quantidade_dispositivos_padrao;
        mudar_LED(idx);
        atualizar_csv(1, opcao, 1-dispositivos_cadastrados[idx].getSaida());
    }
}

string pegar_comodo_dispositivo(WINDOW *menu, const int num_lines) {
    limpar_menu(menu, num_lines);

    char comodo[MAX_CARACTERES_COMODO+1];

    mtx_interface.lock();
    mvwprintw(menu, 3, 2, "Digite o nome do comodo (max %d caracteres): ", MAX_CARACTERES_COMODO);
    mtx_interface.unlock();
    
    mvwscanw(menu, 3, 47, " %[^\n]", &comodo);

    string c(comodo);
    return c;
}

string pegar_nome_dispositivo(WINDOW *menu, const int num_lines) {
    limpar_menu(menu, num_lines);

    char nome[MAX_CARACTERES_COMODO+1];

    mtx_interface.lock();
    mvwprintw(menu, 3, 2, "Digite o nome do dispositivo (max %d caracteres): ", MAX_CARACTERES_NOME);
    mtx_interface.unlock();
    
    mvwscanw(menu, 3, 53, " %[^\n]", &nome);

    string n(nome);
    return n;
}

void escolher_dispositivo(WINDOW *menu, const int num_lines) {
    limpar_menu(menu, num_lines);

    if(quantidade_dispositivos_esperando == 0) {
        mtx_interface.lock();
        mvwprintw(menu, 3, 2, "Nao ha dispositivo esperando solicitacao.");
        mvwprintw(menu, 4, 2, "Pressione qualquer tecla para continuar.");
        wrefresh(menu);
        mtx_interface.unlock();
        getchar();
    }
    else if(quantidade_dispositivos_cadastrados == 5) {
        mtx_interface.lock();
        mvwprintw(menu, 3, 2, "Nao ha espaco disponivel para um novo dispositivo.");
        mvwprintw(menu, 4, 2, "Pressione qualquer tecla para continuar.");
        wrefresh(menu);
        mtx_interface.unlock();
        getchar();
    }
    else {
        int opcao;
        bool invalid = false;

        do {
            mtx_interface.lock();

            if(invalid) {
                wmove(menu, 3, 1);
                wclrtoeol(menu);
                mvwprintw(menu, 4, 2, "Escolha deve estar entre 1 e %d", quantidade_dispositivos_esperando);
            }

            mvwprintw(menu, 3, 2, "Digite o identificador do dispositivo que deseja adicionar (1 a %d): ", quantidade_dispositivos_esperando);
            mtx_interface.unlock();
            mvwscanw(menu, 3, 70, "%d", &opcao);
            invalid = opcao < 1 || opcao > quantidade_dispositivos_esperando;
        } while (invalid);
        
        string comodo = pegar_comodo_dispositivo(menu, num_lines);
        string nome = pegar_nome_dispositivo(menu, num_lines);
        
        adicionar_novo_dispositivo(opcao, comodo, nome, dispositivos_esperando[opcao-1]);
        atualizar_csv(2, quantidade_dispositivos_cadastrados-1, -1);
    }
}

void pegar_escolhas(WINDOW *menu) {
    if(!programa_pode_continuar) {
        return;
    }
    mtx_interface.lock();
    const int num_lines = getmaxy(menu);
    mtx_interface.unlock();

    int opcao;
    bool invalid = false;

    do {
        reiniciar_menu_ecolhas(menu, num_lines);
        do {
            opcao = -1;
            mtx_interface.lock();

            if(invalid) {
                wmove(menu, num_lines-3, 1);
                wclrtoeol(menu);
                mvwprintw(menu, num_lines-2, 2, "Escolha deve estar entre 0 e 2");
            }

            box(menu, 0, 0);
            wrefresh(menu);
            mvwprintw(menu, num_lines-3, 2, "Escolha uma opcao: ");
            mtx_interface.unlock();
            mvwscanw(menu, num_lines-3, 21, "%d", &opcao);
            invalid = opcao < 0 || opcao > 2;
        } while(invalid);

        switch (opcao) {
        case 1:
            mudar_estado_dispositivo(menu, num_lines);
            break;
        case 2:
            escolher_dispositivo(menu, num_lines);
            break;
        default:
            programa_pode_continuar = false;
            break;
        }
    } while(programa_pode_continuar);
}

void leitura_sensores_bme280() {
    struct bme280_data comp_data;

    if (bme280_set_sensor_mode(BME280_FORCED_MODE, &dev) == BME280_OK) {
        if (bme280_get_sensor_data(BME280_ALL, &comp_data, &dev) == BME280_OK) {
            float temp_lida = comp_data.temperature;
            float umid_lida = comp_data.humidity;
            if(0 <= temp_lida && temp_lida <= 50) {
                temperatura = temp_lida;
            }
            if(0 <= umid_lida && umid_lida <= 100) {
                umidade = umid_lida;
            }
        }
    }
}

void leitura_sensores_gpio() {
    valores[3] = (bcm2835_gpio_lev(GPIO_SENSOR_SALA) ? 1 : 0);
    valores[4] = (bcm2835_gpio_lev(GPIO_SENSOR_COZINHA) ? 1 : 0);
    valores[5] = (bcm2835_gpio_lev(GPIO_SENSOR_PORTA_COZINHA) ? 1 : 0);
    valores[6] = (bcm2835_gpio_lev(GPIO_SENSOR_JANELA_COZINHA) ? 1 : 0);
    valores[7] = (bcm2835_gpio_lev(GPIO_SENSOR_PORTA_SALA) ? 1 : 0);
    valores[8] = (bcm2835_gpio_lev(GPIO_SENSOR_JANELA_SALA) ? 1 : 0);
}

void atualiza_sensores() {
    leitura_sensores_bme280();
    leitura_sensores_gpio();
}

void thread_atualizar_menus(WINDOW *opcoes, WINDOW *dispositivos, WINDOW *solicitacoes) {
    int erro = configurar_gpio();
    if(erro) {
        programa_pode_continuar = false;
        return;
    }

    erro = configurar_sensores(&dev, &id);
    if(erro) {
        programa_pode_continuar = false;
        return;
    }
    
    while(programa_pode_continuar) {
        atualiza_sensores();
        atualizar_menu_opcoes(opcoes);
        atualizar_menu_dispositivos(dispositivos);
        atualizar_menu_solicitacoes(solicitacoes);
        sleep(1);
    }

    close(id.fd);
}

void thread_alarme() {
    while(programa_pode_continuar) {
        if(valores[0]) {
            bool tocar = (
                valores[3] or
                valores[4] or
                valores[5] or
                valores[6] or
                valores[7] or
                valores[8]
            );

            for(int i = 0; i < quantidade_dispositivos_cadastrados; ++i) {
                tocar = tocar or (dispositivos_cadastrados[i].getEntrada() == 1);
            }

            if(tocar) {
                // esta função causa erros com o ncurses
                system("omxplayer alarme.mp3 > /dev/null");
                time_t now = time(0);
                tm *ltm = localtime(&now);

                for(int i = 3; i < 9; ++i) {
                    if(valores[i]) {
                        mtx_csv.lock();
                        fprintf(file, "%02d/%02d/%d %02d:%02d:%02d, Alarme, %s\n",
                            ltm->tm_mday,
                            ltm->tm_mon+1,
                            ltm->tm_year+1900,
                            ltm->tm_hour,
                            ltm->tm_min,
                            ltm->tm_sec,
                            dispositivos_padrao[i]
                        );
                        mtx_csv.unlock();
                    }
                }

                for(int i = 0; i < quantidade_dispositivos_cadastrados; ++i) {
                    if(dispositivos_cadastrados[i].getEntrada()) {
                        mtx_csv.lock();
                        fprintf(file, "%02d/%02d/%d %02d:%02d:%02d, Alarme, %s\n",
                            ltm->tm_mday,
                            ltm->tm_mon+1,
                            ltm->tm_year+1900,
                            ltm->tm_hour,
                            ltm->tm_min,
                            ltm->tm_sec,
                            dispositivos_cadastrados[i].getNome().c_str()
                        );
                        mtx_csv.unlock();
                    }
                }
            }
        }
        sleep(1);
    }
}

void atualizar_csv(const int opcao, const int index, const int ligou) {
    time_t now = time(0);
    tm *ltm = localtime(&now);

    if(opcao == 1) {
        mtx_csv.lock();
        fprintf(file, "%02d/%02d/%d %02d:%02d:%02d, Usuário, %s%s\n",
            ltm->tm_mday,
            ltm->tm_mon+1,
            ltm->tm_year+1900,
            ltm->tm_hour,
            ltm->tm_min,
            ltm->tm_sec,
            ligou == 1 ? "Ligar " : "Desligar ",
            index <= quantidade_dispositivos_padrao ?
            dispositivos_padrao[index-1] :
            dispositivos_cadastrados[index-1-quantidade_dispositivos_padrao].getNome().c_str()
        );
        mtx_csv.unlock();
    }
    else if(opcao == 2) {
        mtx_csv.lock();
        fprintf(file, "%02d/%02d/%d %02d:%02d:%02d, Usuário, Adicionou %s\n",
            ltm->tm_mday,
            ltm->tm_mon+1,
            ltm->tm_year+1900,
            ltm->tm_hour,
            ltm->tm_min,
            ltm->tm_sec,
            dispositivos_cadastrados[index].getNome().c_str()
        );
        mtx_csv.unlock();
    }
}
