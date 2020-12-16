#include <stdio.h>
#include "interface.hpp"

int main() {
    signal(SIGHUP, signal_handler);
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    int size_x, size_y;
    initscr();

    getmaxyx(stdscr, size_y, size_x);
    const int min_colunas = 160, min_linhas = 41;

    if(size_y < min_linhas or size_x < min_colunas) {
        endwin();
        printf("Para uma melhor experiência, ajuste o tamanho do terminal para, no mínimo:\n");
        printf("%d colunas e %d linhas (%dx%d)\n", min_colunas, min_linhas, min_colunas, min_linhas);
        printf("Atual: %dx%d\n", size_x, size_y);
        return 0;
    }

    WINDOW *opcoes = newwin(31, size_x/4, 0, 0),
           *dispositivos = newwin(31, size_x/2, 0, size_x/4),
           *solicitacoes = newwin(31, size_x/4, 0, 3*size_x/4),
           *escolhas = newwin(size_y-31, size_x, 31, 0);
    
    box(opcoes, 0, 0);
    box(dispositivos, 0, 0);
    box(solicitacoes, 0, 0);
    box(escolhas, 0, 0);
    refresh();
    wrefresh(opcoes);
    wrefresh(dispositivos);
    wrefresh(solicitacoes);
    wrefresh(escolhas);

    int erro;

    if((erro = iniciar_MQTT()) != 0) {
        endwin();
        printf("Erro ao iniciar MQTT: %d\n", erro);
        return 0;
    }

    iniciar_menu(opcoes, "OPCOES");
    iniciar_menu(dispositivos, "DISPOSITIVOS");
    iniciar_menu(solicitacoes, "SOLICITACOES");
    iniciar_menu(escolhas, "ESCOLHAS");

    pegar_comodos_ja_cadastrados();

    thread thread_menus(thread_atualizar_menus, opcoes, dispositivos, solicitacoes);
    pegar_escolhas(escolhas);

    thread_menus.join();

    desativar_MQTT();

    delwin(opcoes);
    delwin(dispositivos);
    delwin(solicitacoes);
    delwin(escolhas);

    endwin();

    return 0;
}