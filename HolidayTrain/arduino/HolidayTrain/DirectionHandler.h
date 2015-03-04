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
#ifndef _DIRECTIONHANDLER_H
#define _DIRECTIONHANDLER_H

#include <stdint.h>

class DirectionHandler {
public:
  DirectionHandler(uint8_t * sensorPins, uint8_t * drivePins);
  virtual ~DirectionHandler();

  void check(uint64_t now);

  void setDirection(int8_t dir);
  void setFutureDirection(int8_t dir);

private:
  void checkSensors(uint64_t now);
  void checkDirectionTimeout(uint64_t now);
  void checkFutureDirectionTimeout(uint64_t now);
  void checkCurrentDirectionTimeout(uint64_t now);

  uint8_t sensorPins[2];
  uint8_t drivePins[2];
  uint64_t sensorDebounce[2];

  int8_t upcomingDirection;
  uint64_t directionTimeout;

  int8_t futureDirection;
  uint64_t futureDirectionTimeout;
  
  uint64_t currentDirectionTimeout;

  uint8_t directionState;
  // 0 - doing nothing
  // 1 - waiting to set direction
  // 2 - waiting to toggle power
};

#endif

