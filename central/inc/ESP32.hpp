#ifndef ESP32_HPP
#define ESP32_HPP

#include <string>

using namespace std;

class ESP32 {
private:
    string comodo;
    string nome;
    string mac_adress;

public:
    ESP32();
    ESP32(const string& c, const string& n, const string& m);

    string getComodo() const;
    string getNome() const;
    string getMacAdress() const;
};

#endif // ESP32_HPP