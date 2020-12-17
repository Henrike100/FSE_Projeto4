#ifndef INTERFACE_HPP
#define INTERFACE_HPP

#include <ncurses.h>
#include <mutex>
#include <string.h>
#include <unistd.h>
#include <thread>
#include <signal.h>
#include <stdint.h>
#include <iostream>
#include <vector>
#include "constantes.hpp"
#include "ESP32.hpp"
#include "MQTTClient.h"
#include "utilidades.hpp"

using namespace std;

void signal_handler(int signum);
int iniciar_MQTT();
void desativar_MQTT();
void pegar_comodos_ja_cadastrados();
void adicionar_novo_dispositivo(const int index, const string comodo, const string nome, const string mac_adress);
void iniciar_menu(WINDOW *menu, string titulo);
void atualizar_menu_opcoes(WINDOW *menu);
void atualizar_menu_dispositivos(WINDOW *menu);
void atualizar_menu_solicitacoes(WINDOW *menu);
void pegar_escolhas(WINDOW *menu);
void thread_atualizar_menus(WINDOW *opcoes, WINDOW *dispositivos, WINDOW *solicitacoes);

void thread_alarme();

int abrir_csv();
void atualizar_csv(const int opcao, const int index, const int ligou);
void fechar_csv();

#endif // INTERFACE_HPP