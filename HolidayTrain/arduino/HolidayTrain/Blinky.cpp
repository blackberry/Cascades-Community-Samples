#include "Blinky.h"

#include <Arduino.h>

Blinky::Blinky(uint8_t p) : 
pin(p), nextChange(millis()+2000) {
  pinMode(pin,OUTPUT);
  digitalWrite(pin,HIGH);
}

Blinky::~Blinky() {
}

void Blinky::check(uint64_t now) {
  if (now>=nextChange) {
    nextChange += 2000;
    digitalWrite(pin,!digitalRead(pin));
  }
}

