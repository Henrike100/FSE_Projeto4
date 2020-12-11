#include "interface.hpp"

mutex mtx_interface;

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


    
    mtx_interface.unlock();
}

void atualizar_menu_dispositivos(WINDOW *menu) {
    mtx_interface.lock();


    
    mtx_interface.unlock();
}

void atualizar_menu_solicitacoes(WINDOW *menu) {
    mtx_interface.lock();


    
    mtx_interface.unlock();
}

void atualizar_menu_escolhas(WINDOW *menu) {
    mtx_interface.lock();


    
    mtx_interface.unlock();
}
