#include "LightingHandler.h"
#include <Arduino.h>

LightingHandler::LightingHandler(uint8_t * lightPins) {
  // Copy pin numbers.
  for (uint8_t i =0; i<4; ++i) {
    this->lightPins[i] = lightPins[i];
    lightValues[i] = 255;
    lightModes[i] = random(0,2);
    for (uint8_t j=0; j<2; ++j) {
      params[i][j] = random(150,8000);
    }
  }

  // Set low level output on LED.
  for (uint8_t i=0; i<4; ++i) {
    digitalWrite(lightPins[i],0);
    pinMode(lightPins[i], OUTPUT);
    setLight(i,63);
  }
  
}

LightingHandler::~LightingHandler() {
}

void LightingHandler::setLight(uint8_t which, uint8_t value) {
  if (which>=4) {
    return;
  }
  if (value>127) {
    value=127;
  }
  if (lightModes[which]!=0 || lightValues[which]!=value) {
    analogWrite(lightPins[which],value);
  }
  lightValues[which] = value;
}

void LightingHandler::setMode(uint8_t which, uint8_t value) {
  lightModes[which] = value;
  switch(value) {
  case 0:
    setLight(which,(lightValues[which]--)+1);
    break;
  }
}

void LightingHandler::check(uint64_t now) {
  for (uint8_t i=0; i<4; ++i) {
    switch (lightModes[i]) {
    case 0:
      // do nothing.
      break;
    case 1:
      float val = (sin(now / params[i][0]) + sin(now / params[i][1]))/2/0.4;
      float lv = lightValues[i] * 0.2;
      float fin = 0.6 * lv + lv * val;
      analogWrite(lightPins[i],(uint8_t)fin);
      break;
    }
  }
}





