#include <bcm2835.h>
#include <stdio.h>

#ifndef GPIO_H
#define GPIO_H

#define PULLUP 1
#define PULLOFF 2
#define PULLDOWN 0

#define INPUTM 0
#define OUTM 1

void GPIOMode(char PIN,char Mode,char Res);
uint8_t digitalRead(uint8_t PIN);

#endif