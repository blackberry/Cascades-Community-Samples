/*
 * Copyright (c) 2011-2013 BlackBerry Limited.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
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



