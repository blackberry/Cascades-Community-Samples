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
#include "LightingHandler.h"
#include <Arduino.h>
#include <Serial>

#define LOW_VALUE 32
#define HIGH_VALUE 240

LightingState::LightingState() {
  mPin = 0xff;
  mMode = 0xff;
  mValue = 1;
  setValue(0);
  mMonitor = 0;
}

void LightingState::setMonitor() {
  mMonitor = 1;
}

void LightingState::setPin(uint8_t pin) {
  mPin = pin;
  digitalWrite(pin,0);
  pinMode(pin,OUTPUT);
}

void LightingState::setMode(uint8_t mode) {
  mMode = mode;
  switch(mode) {
    case 0:
      // do nothing
      break;
    case 1:
      // clear buffers
      mSparkleState = 3;
      mSparkleEndTime = 0;
      break;
    case 2:
      mRampLastTime = 0;
    default:
      break;
  }
}

void LightingState::setValue(uint8_t value) {
  if (value>240) {
    value = 240;
  }
  if (value!=mValue) {
    mValue = value;
    analogWrite(mPin,mValue);
    if (mMonitor) {
      Serial.println(mValue);
    }
  }
}

void LightingState::check(uint64_t now) {
  if (mPin==0xff) {
    return;
  }
  switch (mMode) {
    case 0:
      analogWrite(mPin,mValue);
      break;
    case 1:
      if (now > mSparkleEndTime) {
        mSparkleState += 1;
        mSparkleState %= 4;
        mSparkleStartTime = now;
        
        switch(mSparkleState) {
          case 0:
             mSparkleStartValue = LOW_VALUE;
             mSparkleTargetValue = LOW_VALUE;
             mSparkleEndTime = now + 1000 + random(10000);
             break;
           case 1:
             mSparkleStartValue = LOW_VALUE;
             mSparkleTargetValue = HIGH_VALUE;
             mSparkleEndTime = now + 1000 + random(1000);
             break;
           case 2:
             mSparkleStartValue = HIGH_VALUE;
             mSparkleTargetValue = HIGH_VALUE;
             mSparkleEndTime = now + 500 + random(1000);
             break;
           case 3:
             mSparkleStartValue = HIGH_VALUE;
             mSparkleTargetValue = LOW_VALUE;
             mSparkleEndTime = now + 1000 + random(1000);
        }
        
        if (mMonitor) {
          Serial.println("===");
          Serial.println(mSparkleStartValue);
          Serial.println(mSparkleTargetValue);
          Serial.println(mSparkleEndTime);
          Serial.println("---");
        }
      }
      
      // Sparkle Calcs
      {
        int16_t range = (int16_t)mSparkleTargetValue - mSparkleStartValue;
        int32_t delta = mSparkleEndTime - mSparkleStartTime;
        int32_t at = now - mSparkleStartTime;
        uint8_t target = (uint8_t)(1.0 * at / delta * range + mSparkleStartValue);
        if (target!=mValue) {
          setValue(target);
        }
      }
      
      break;
    case 2:
      if (mRampLastTime==0) {
        mRampLastTime = now;
      }
      if (now>=mRampLastTime+200) {
        mRampLastTime += 200;
        if (mValue==240) {
          setValue(0);
        } else {
          setValue(1+mValue);
        }
      }
      break;
    case 0xff:
    default:
      setValue(0);
  }
}

LightingHandler::LightingHandler(uint8_t * lightPins) {
  // Copy pin numbers.
  for (uint8_t i =0; i<4; ++i) {
    lights[i].setPin(lightPins[i]);
    lights[i].setValue(16);
    lights[i].setMode(1);
  }
  
  //   lights[1].setMonitor();  
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
  lights[which].setValue(value);
}

void LightingHandler::setMode(uint8_t which, uint8_t value) {
  if (which>=4) {
    return;
  }
  lights[which].setMode(value);
}

void LightingHandler::check(uint64_t now) {
  for (uint8_t i=0; i<4; ++i) {
    lights[i].check(now);
  }
}





