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

