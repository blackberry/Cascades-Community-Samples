#ifndef _LIGHTINGHANDLER_H
#define _LIGHTINGHANDLER_H

#include <stdint.h>

class LightingHandler {
public:
  LightingHandler(uint8_t * lightPins);
  virtual ~LightingHandler();

  void setLight(uint8_t which, uint8_t value);
  void setMode(uint8_t which, uint8_t mode);

  void check(uint64_t now);

private:
  uint8_t lightPins[4];
  uint8_t lightValues[4];
  uint8_t lightModes[4];
  
  float params[4][2];
};


#endif

