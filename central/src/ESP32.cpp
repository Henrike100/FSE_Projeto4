#include "ESP32.hpp"

ESP32::ESP32() {}
ESP32::ESP32(const string& c, const string& n, const string& m) : comodo(c), nome(n), mac_adress(m) {}

string ESP32::getComodo() const { return comodo; }
string ESP32::getNome() const { return nome; }
string ESP32::getMacAdress() const { return mac_adress; }