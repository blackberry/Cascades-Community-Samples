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

