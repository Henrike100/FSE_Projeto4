#include "interface.hpp"

mutex mtx_interface;
bool programa_pode_continuar = true;
int quantidade_dispositivos_padrao = 9;

char dispositivos_cadastrados[5][18] = {0};
int quantidade_dispositivos_cadastrados = 0;

// char dispositivos_esperando[5][18] = {0};
// int quantidade_dispositivos_esperando = 0;
char dispositivos_esperando[5][18] = {
    "24:62:ab:e0:bc:14",
    "12:34:56:78:90:ab"
};
int quantidade_dispositivos_esperando = 2;

char dispositivos_cadastrados_nome[5][MAX_CARACTERES_NOME] = {0};

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

    for(int index = 1; index <= 9; ++index, linha_atual += 2) {
        wmove(menu, linha_atual, 1);
        wclrtoeol(menu);

        mvwprintw(menu, linha_atual, 2, "%02d   Ligar", index);
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

        mvwvline(menu, linha_atual+2, start+(line_size/2)/3, 0, 1);
        mvwvline(menu, linha_atual+2, start+(line_size)/3, 0, 1);

        mvwhline(menu, linha_atual+3, start, 0, line_size/2);

        if(!(index&1))
            linha_atual += 4;

        index++;
    }

    int barra_vertical = 4*(quantidade_dispositivos_padrao/2);

    if(quantidade_dispositivos_padrao&1) barra_vertical += 4;

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

void adicionar_novo_dispositivo(int index) {
    strcpy(dispositivos_cadastrados[quantidade_dispositivos_cadastrados], dispositivos_esperando[index-1]);
    quantidade_dispositivos_cadastrados++;

    for(int i = index-1; i < quantidade_dispositivos_esperando-1; ++i) {
        strcpy(dispositivos_esperando[i], dispositivos_esperando[i+1]);
    }
    quantidade_dispositivos_esperando--;
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
        
        adicionar_novo_dispositivo(opcao);
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