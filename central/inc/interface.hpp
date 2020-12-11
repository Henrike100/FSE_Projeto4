#ifndef INTERFACE_HPP
#define INTERFACE_HPP

#include <ncurses.h>
#include <mutex>

using namespace std;

void iniciar_menu(WINDOW *menu, string titulo);

void atualizar_menu_opcoes(WINDOW *menu);
void atualizar_menu_dispositivos(WINDOW *menu);
void atualizar_menu_solicitacoes(WINDOW *menu);
void atualizar_menu_escolhas(WINDOW *menu);

#endif // INTERFACE_HPP