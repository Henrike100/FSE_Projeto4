#include "interface.hpp"

mutex mtx_interface;
bool programa_pode_continuar = true;

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
    int index = 1;

    for(int i = 3; i <= 29; i += 2, index++) {
        wmove(menu, i, 1);
        wclrtoeol(menu);

        mvwprintw(menu, i, 2, "%02d   Ligar", index);
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
    const int quantidade_dispositivos = 9;

    int linha_atual = 3, index = 1;

    while(index <= quantidade_dispositivos) {
        int start = 1;
        if(!(index&1))
            start += (line_size/2);

        mvwprintw(menu, linha_atual, start+1, "%02d Nome do Dispositivo", index);
        mvwhline(menu, linha_atual+1, start, 0, line_size/2);

        mvwvline(menu, linha_atual+2, start+(line_size/2)/3, 0, 1);
        mvwvline(menu, linha_atual+2, start+(line_size)/3, 0, 1);

        mvwhline(menu, linha_atual+3, start, 0, line_size/2);

        if(!(index&1))
            linha_atual += 4;

        index++;
    }

    int barra_vertical = 4*(quantidade_dispositivos/2);

    if(quantidade_dispositivos&1) barra_vertical += 4;

    mvwvline(menu, 3, line_size/2, 0, barra_vertical);

    box(menu, 0, 0);
    wrefresh(menu);
    
    mtx_interface.unlock();
}

void atualizar_menu_solicitacoes(WINDOW *menu) {
    mtx_interface.lock();


    
    mtx_interface.unlock();
}

void limpar_menu_escolhas(WINDOW *menu, const int num_lines) {
    mtx_interface.lock();

    for(int i = 3; i < num_lines-1; ++i) {
        wmove(menu, i, 1);
        wclrtoeol(menu);
    }

    mtx_interface.unlock();
}

void reiniciar_menu_ecolhas(WINDOW *menu, const int num_lines) {
    limpar_menu_escolhas(menu, num_lines);

    mtx_interface.lock();

    mvwprintw(menu, 3, 2, "1. Mudar estado de um dispositivo");
    mvwprintw(menu, 4, 2, "2. Adicionar novo dispositvo");
    mvwprintw(menu, 5, 2, "0. Sair");

    mtx_interface.unlock();
}

void mudar_estado_dispositivo(WINDOW *menu, const int num_lines) {
    limpar_menu_escolhas(menu, num_lines);

    int opcao;
    bool invalid = false;

    do {
        mtx_interface.lock();

        if(invalid) {
            wmove(menu, 3, 1);
            wclrtoeol(menu);
            mvwprintw(menu, 4, 2, "Escolha deve estar entre 1 e 14");
        }

        mvwprintw(menu, 3, 2, "Digite o numero do dispositivo que deseja mudar: ");
        mtx_interface.unlock();
        mvwscanw(menu, 3, 51, "%d", &opcao);
        invalid = opcao < 1 || opcao > 14;
    } while(invalid);

    // mudar estado
}

void atualizar_menu_escolhas(WINDOW *menu) {
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
            /* code */
            break;
        default:
            programa_pode_continuar = false;
            break;
        }

    } while(programa_pode_continuar);
}
