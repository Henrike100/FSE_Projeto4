#ifndef CONFIGURACOES_HPP
#define CONFIGURACOES_HPP

#include <stdio.h>
#include <ctime>
#include <signal.h>
#include <fcntl.h>
#include <termios.h>
#include <stdlib.h>
#include <sys/types.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <thread>

#include "utilidades.hpp"
#include "gpio.hpp"

int configurar_sensores(struct bme280_dev *dev, struct identifier *id);
int configurar_gpio();

#endif // CONFIGURACOES_HPP