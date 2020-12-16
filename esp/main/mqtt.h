#ifndef MQTT_H
#define MQTT_H

extern char comodo[20];
extern int comodo_definido;
void mqtt_start();
void mqtt_envia_mensagem(char * topico, char * mensagem);

#endif
