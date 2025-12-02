#ifndef LIGHT_BEE_H
#define LIGHT_BEE_H

#include <Arduino.h>

void light_bee_init();
void ABEE_ON();
void ABEE_OFF();
void ALIGHT_ON();
void ALIGHT_OFF();

#define BEE_PIN 27
#define BEE_ON LOW
#define BEE_OFF HIGH

#define LIGHT_PIN 4
#define LIGHT_ON HIGH
#define LIGHT_OFF LOW

#endif // LIGHT_BEE_H