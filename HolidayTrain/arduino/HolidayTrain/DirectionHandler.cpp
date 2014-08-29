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
#include "DirectionHandler.h"

#include <Arduino.h>

DirectionHandler::DirectionHandler(uint8_t * sensorPins, uint8_t * drivePins) : 
upcomingDirection(0), directionTimeout(0), futureDirection(0),futureDirectionTimeout(0),directionState(0),currentDirectionTimeout(0) {
  for (uint8_t i=0; i<2; ++i) {
    this->sensorPins[i] = sensorPins[i];
    this->drivePins[i] = drivePins[i];
    sensorDebounce[i] = 0;
  }

  // Turn off drive pins.
  for (uint8_t i=0; i<2; ++i) {
    digitalWrite(drivePins[i],0);
    pinMode(drivePins[i],OUTPUT);
  }

  // Sensor pins
  for (uint8_t i=0; i<2; ++i) {
    pinMode(sensorPins[i],INPUT);
    digitalWrite(sensorPins[i],1);
  }

  if (digitalRead(sensorPins[0])==1 && digitalRead(sensorPins[1])==1) {
    setDirection(1);
  }
}


DirectionHandler::~DirectionHandler() {
  setDirection(0);
}

void DirectionHandler::check(uint64_t now) {
  checkSensors(now);
  checkDirectionTimeout(now);
  checkFutureDirectionTimeout(now);
  checkCurrentDirectionTimeout(now);
}

void DirectionHandler::checkSensors(uint64_t now) {
  for (uint8_t i=0; i<2; ++i) {
    if (now-sensorDebounce[i] > 100) {
      uint8_t pressed = !digitalRead(sensorPins[i]);
      if (pressed) {
        uint64_t lastDebounce = sensorDebounce[i];
        uint8_t wasPressed = sensorDebounce[i]!=0;
        sensorDebounce[i] = now;
        if (!wasPressed) {
//          Serial.print("Pressed: ");
//          Serial.print(i);
//          Serial.print(" (db)");
//          Serial.print((int32_t)lastDebounce);
//          Serial.print(" (sd)");
//          Serial.print((int32_t)sensorDebounce[i]);
//          Serial.print(" ");
//          Serial.print(" (now)");
//          Serial.println((int32_t)now);
          if (upcomingDirection==0) {
            setDirection(i==0?1:-1);
          } 
          else {
            int8_t newDir = -upcomingDirection;
            setDirection(0);
            setFutureDirection(newDir);
          }
        }
      } 
      else {
        if (sensorDebounce[i]!=0) {
//          Serial.print("Sensor Up");
//          Serial.println(i);
        }
        sensorDebounce[i] = 0;
      }
    } 
    else {
      // Less than 100ms since that sensor was touched.
    }
  }
}

void DirectionHandler::checkFutureDirectionTimeout(uint64_t now) {
  if (futureDirectionTimeout==0) {
    return;
  }
  if (now>=futureDirectionTimeout) {
    setDirection(futureDirection);
  }
}

void DirectionHandler::setFutureDirection(int8_t direction) {
  futureDirectionTimeout = millis() + random(2000,10000);
  futureDirection=direction;
}

void DirectionHandler::setDirection(int8_t dir) {
//  Serial.print("d");
//  Serial.println(dir);
  uint64_t now = millis();

  futureDirectionTimeout=0;

  currentDirectionTimeout = now + 45000;

  if (dir==upcomingDirection) {
    return;
  }

  upcomingDirection = dir;
  digitalWrite(drivePins[0],0);

  directionState = 1;
  directionTimeout = now + 10;
}

void DirectionHandler::checkDirectionTimeout(uint64_t now) {
  if (directionTimeout==0) {
    return;
  }
  if (now<directionTimeout) {
    return;
  }

  directionTimeout = 0;
  if (directionState==0) {
    return;
  }

  switch(directionState) {
  case 1:
    digitalWrite(drivePins[1],upcomingDirection<0);
    if (upcomingDirection!=0) {
      directionState = 2;
      directionTimeout = now+10;
    }
    break;
  case 2:
    digitalWrite(drivePins[0],upcomingDirection!=0);
    directionState = 0;
    break;
  }
}

void DirectionHandler::checkCurrentDirectionTimeout(uint64_t now) {
  if (currentDirectionTimeout==0) {
    return;
  }

  if (now >= currentDirectionTimeout) {
    currentDirectionTimeout = 0;
    uint8_t current = upcomingDirection;
    if (current==0) {
      return;
    }
    setDirection(0);
    setFutureDirection(-current);
  }
}






