#ifndef _BLINKY_H
#define _BLINKY_H

#include <stdint.h>

class Blinky {
  public: Blinky(uint8_t pin);
  virtual ~Blinky();
  
  void check(uint64_t now);
  
  private:
  uint8_t pin;
  uint64_t nextChange;
};

#endif
