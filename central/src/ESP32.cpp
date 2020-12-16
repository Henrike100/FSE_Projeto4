#include "ESP32.hpp"

ESP32::ESP32() {}
ESP32::ESP32(const string& c, const string& n, const string& m) : comodo(c), nome(n), mac_adress(m) {}

string ESP32::getComodo() const { return comodo; }
string ESP32::getNome() const { return nome; }
string ESP32::getMacAdress() const { return mac_adress; }

string ESP32::getTemperatura() const {
    if(temperatura == -2)
        return "";

    return to_string(temperatura);
}

string ESP32::getUmidade() const {
    if(umidade == -2)
        return "";

    return to_string(umidade);
}

int ESP32::getEntrada() const {
    return entrada;
}

int ESP32::getSaida() const {
    return saida;
}

void ESP32::setTemperatura(const int t) {
    if(t >= 0) {
        temperatura = t;
    }
}

void ESP32::setUmidade(const int u) {
    if(u >= 0) {
        umidade = u;
    }
}

void ESP32::setEntrada(const int e) {
    if(e == 0 or e == 1) {
        entrada = e;
    }
}

void ESP32::setSaida(const int s) {
    if(s == 0 or s == 1) {
        saida = s;
    }
}