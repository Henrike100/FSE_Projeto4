#ifndef INTERFACE_HPP
#define INTERFACE_HPP

#include <ncurses.h>
#include <mutex>
#include <string.h>
#include <unistd.h>
#include <thread>
#include <signal.h>
#include "constantes.hpp"
#include "ESP32.hpp"

using namespace std;

void signal_handler(int signum);
void iniciar_menu(WINDOW *menu, string titulo);
void atualizar_menu_opcoes(WINDOW *menu);
void atualizar_menu_dispositivos(WINDOW *menu);
void atualizar_menu_solicitacoes(WINDOW *menu);
void pegar_escolhas(WINDOW *menu);
void thread_atualizar_menus(WINDOW *opcoes, WINDOW *dispositivos, WINDOW *solicitacoes);

#endif // INTERFACE_HPP