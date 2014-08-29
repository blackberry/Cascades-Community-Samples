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
#ifndef _BLUETOOTHCOMMAND_H
#define _BLUETOOTHCOMMAND_H

#include <stdint.h>

#include "DirectionHandler.h"
#include "LightingHandler.h"

class BluetoothManager {
public:
  BluetoothManager(DirectionHandler & direction, LightingHandler &lighting, uint32_t baud);
  ~BluetoothManager();
  
  void init();
  void check(uint64_t now);

private:
  void processLine();
  
  void processDirectionCommand();
  void processLightingCommand();
  
  int16_t readInt(uint8_t offset,uint8_t * valid = 0);
  
  DirectionHandler & direction;
  LightingHandler & lighting;
  
  uint8_t buffer[10];
  uint8_t bufferOffset;
  
  uint32_t baud;
  
};

#endif

