#ifndef ESP32_HPP
#define ESP32_HPP

#include <string>

#define VALOR_INICIAL -2

using namespace std;

class ESP32 {
private:
    string comodo;
    string nome;
    int mac_adress;
    int temperatura = VALOR_INICIAL;
    int umidade = VALOR_INICIAL;
    int entrada = VALOR_INICIAL;
    int saida = VALOR_INICIAL;

public:
    ESP32();
    ESP32(const string& c, const string& n, const int m);

    string getComodo() const;
    string getNome() const;
    int getMacAdress() const;
    string getTemperatura() const;
    string getUmidade() const;
    int getEntrada() const;
    int getSaida() const;

    void setTemperatura(const int t);
    void setUmidade(const int u);
    void setEntrada(const int e);
    void setSaida(const int s);

};

#endif // ESP32_HPP