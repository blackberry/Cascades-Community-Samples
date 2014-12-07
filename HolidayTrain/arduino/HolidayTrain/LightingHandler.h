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
#ifndef _LIGHTINGHANDLER_H
#define _LIGHTINGHANDLER_H

#include <stdint.h>

class LightingState {
public:
  LightingState();
  
  void setPin(uint8_t pin);
  void setMode(uint8_t mode);
  void setValue(uint8_t value);
  void check(uint64_t now);
  void setMonitor();
private:

  void sparkleNextState();
  
  uint8_t mPin;
  uint8_t mValue;
  uint8_t mMode;
  
  uint8_t mSparkleState;
  uint32_t mSparkleStartTime;
  uint32_t mSparkleEndTime;
  uint8_t mSparkleStartValue;
  uint8_t mSparkleTargetValue;
  
  int64_t mRampLastTime;
  
  uint8_t mMonitor;
};

class LightingHandler {
public:
  LightingHandler(uint8_t * lightPins);
  virtual ~LightingHandler();

  void setLight(uint8_t which, uint8_t value);
  void setMode(uint8_t which, uint8_t mode);

  void check(uint64_t now);

private:
  LightingState lights[4];
};


#endif

