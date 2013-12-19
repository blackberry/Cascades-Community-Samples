#include <Serial>
#include "DirectionHandler.h"
#include "LightingHandler.h"
#include "BluetoothManager.h"
#include "Blinky.h"

// #define DEBUG true

#ifdef DEBUG 

#define BAUDRATE 9600

#else

#define BAUDRATE 38400

#endif

uint8_t sensorPins[] = { 
  8,12 };

uint8_t drivePins[] = {
  A2,A3};

uint8_t lightPins[] = { 
  5,6,10,11 };

DirectionHandler direction(sensorPins,drivePins);
LightingHandler lighting(lightPins);

BluetoothManager bluetooth(direction,lighting,BAUDRATE);

Blinky blinky(13);

void setup() {
  bluetooth.init();
}

void loop() {
  uint64_t now = millis();
  direction.check(now);
  lighting.check(now);
  bluetooth.check(now);
  blinky.check(now);
}



