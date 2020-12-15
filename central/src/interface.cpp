#include "interface.hpp"

FILE *file;

mutex mtx_interface;
mutex mtx_csv;
bool programa_pode_continuar = true;
const int quantidade_dispositivos_padrao = 9;

ESP32 dispositivos_cadastrados[5];
int quantidade_dispositivos_cadastrados = 0;

// char dispositivos_esperando[5][18] = {0};
// int quantidade_dispositivos_esperando = 0;
char dispositivos_esperando[5][18] = {
    "24:62:ab:e0:bc:14",
    "12:34:56:78:90:ab"
};
int quantidade_dispositivos_esperando = 2;

void signal_handler(int signum) {
    programa_pode_continuar = false;
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

        mvwprintw(menu, linha_atual, 2, "%02d   Ligar", index);
    }

    for(int i = 0; i < quantidade_dispositivos_cadastrados; ++i, linha_atual += 2) {
        wmove(menu, linha_atual, 1);
        wclrtoeol(menu);

        mvwprintw(menu, linha_atual, 2, "%02d   Ligar", i+1+quantidade_dispositivos_padrao);
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

        mvwvline(menu, linha_atual+2, start-1+(line_size)/8, 0, 1);
        mvwvline(menu, linha_atual+2, start-1+(line_size)/4, 0, 1);
        mvwvline(menu, linha_atual+2, start-2+(3*line_size)/8, 0, 1);

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

        string name = dispositivos_cadastrados[i-1].getNome();

        const string spaces(((line_size/2)-name.size())/2 - 3, ' ');

        mvwprintw(menu, linha_atual, start+1, "%02d%s%s%s", index, spaces.c_str(), name.c_str(), spaces.c_str());
        mvwhline(menu, linha_atual+1, start, 0, line_size/2);

        mvwvline(menu, linha_atual+2, start-1+(line_size)/8, 0, 1);
        mvwvline(menu, linha_atual+2, start-1+(line_size)/4, 0, 1);
        mvwvline(menu, linha_atual+2, start-2+(3*line_size)/8, 0, 1);

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

    // mudar estado
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
    dispositivos_cadastrados[quantidade_dispositivos_cadastrados] = ESP32(comodo, nome, mac_adress);
    quantidade_dispositivos_cadastrados++;

    for(int i = index-1; i < quantidade_dispositivos_esperando-1; ++i) {
        strcpy(dispositivos_esperando[i], dispositivos_esperando[i+1]);
    }
    quantidade_dispositivos_esperando--;

    salvar_dispositivos_cadastrados();
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
        string mac_adress(dispositivos_esperando[opcao-1]);
        
        adicionar_novo_dispositivo(opcao, comodo, nome, mac_adress);
    }
}

void pegar_escolhas(WINDOW *menu) {
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

void thread_atualizar_menus(WINDOW *opcoes, WINDOW *dispositivos, WINDOW *solicitacoes) {
    while(programa_pode_continuar) {
        atualizar_menu_opcoes(opcoes);
        atualizar_menu_dispositivos(dispositivos);
        atualizar_menu_solicitacoes(solicitacoes);
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
        // salvar
        mtx_csv.unlock();
    }
}