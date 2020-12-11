#include <stdio.h>
#include "interface.hpp"

int main() {
    int size_x, size_y;
    initscr();

    getmaxyx(stdscr, size_y, size_x);
    const int min_colunas = 140, min_linhas = 39;

    if(size_y < min_linhas or size_x < min_colunas) {
        endwin();
        printf("Para uma melhor experiência, ajuste o tamanho do terminal para, no mínimo:\n");
        printf("%d colunas e %d linhas (%dx%d)\n", min_colunas, min_linhas, min_colunas, min_linhas);
        printf("Atual: %dx%d\n", size_x, size_y);
        return 0;
    }

    WINDOW *opcoes = newwin(30, size_x/4, 0, 0),
           *dispositivos = newwin(30, size_x/2, 0, size_x/4),
           *solicitacoes = newwin(30, size_x/4, 0, 3*size_x/4),
           *escolhas = newwin(size_y-30, size_x, 30, 0);
    
    box(opcoes, 0, 0);
    box(dispositivos, 0, 0);
    box(solicitacoes, 0, 0);
    box(escolhas, 0, 0);
    refresh();
    wrefresh(opcoes);
    wrefresh(dispositivos);
    wrefresh(solicitacoes);
    wrefresh(escolhas);

    iniciar_menu(opcoes, "OPCOES");
    iniciar_menu(dispositivos, "DISPOSITIVOS");
    iniciar_menu(solicitacoes, "SOLICITACOES");
    iniciar_menu(escolhas, "ESCOLHAS");

    atualizar_menu_opcoes(opcoes);
    atualizar_menu_dispositivos(dispositivos);
    atualizar_menu_solicitacoes(solicitacoes);
    atualizar_menu_escolhas(escolhas);
    
    getchar();

    delwin(opcoes);
    delwin(dispositivos);
    delwin(solicitacoes);
    delwin(escolhas);

    endwin();

    return 0;
}