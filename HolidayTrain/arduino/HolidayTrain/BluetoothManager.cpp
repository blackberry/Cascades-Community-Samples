#include "BluetoothManager.h"

#include <Arduino.h>
#include <Serial.h>

BluetoothManager::BluetoothManager(DirectionHandler & d, LightingHandler & l, uint32_t baud) : 
direction(d), lighting(l), bufferOffset(0), baud(baud) {
}

BluetoothManager::~BluetoothManager() { 
}

void BluetoothManager::init() {
  Serial.begin(baud); //Set BluetoothBee BaudRate to default baud rate 38400
  Serial.print("\r\n+STWMOD=0\r\n"); //set the bluetooth work in slave mode
  Serial.print("\r\n+STNA=Railway\r\n"); //set the bluetooth name as "SeeedBTSlave"
  Serial.print("\r\n+STOAUT=1\r\n"); // Permit Paired device to connect me
  Serial.print("\r\n+STAUTO=0\r\n"); // Auto-connection should be forbidden here
  delay(2000); // This delay is required.
  Serial.print("\r\n+INQ=1\r\n"); //make the slave bluetooth inquirable 
  //  Serial.println("The slave bluetooth is inquirable!");
  delay(2000); // This delay is required.
  Serial.flush();
}

void BluetoothManager::check(uint64_t now) {
  while(Serial.available()!=0) {
    uint8_t next = Serial.read();
    if (next<' ') {
      processLine();
    } 
    else {
      buffer[bufferOffset++] = next;
      bufferOffset%=sizeof(buffer);
    }
  }
}

void BluetoothManager::processLine() {
  if (bufferOffset>0) {
    switch(buffer[0]) {
    case 'l':
      processLightingCommand();
      break;
    case 'd':
      processDirectionCommand();
      break;
    default:
//      Serial.print("ERR - unknown command: ");
//      for (uint8_t i = 0; i<bufferOffset; ++i) {
//        Serial.write(buffer[i]);
//      }
//      Serial.println();
      break;
    }
  }
  bufferOffset = 0;
}

void BluetoothManager::processDirectionCommand() {
  if (bufferOffset<2) {
    Serial.println("ERR - command too short");
    return;
  }

  uint8_t valid;
  int16_t newDirection = readInt(1,&valid);
  if (!valid) {
    Serial.println("ERR - not valid integer");
    return;
  }
  if (newDirection<-1 || newDirection>1) {
    Serial.println("ERR - direction out of range");
    return;
  }
  direction.setDirection(newDirection);
}

void BluetoothManager::processLightingCommand() {
  // lv<which><number> - set the light value.
  // lm<which><mode> - set the light mode.
  if (bufferOffset<4) {
    Serial.println("ERR - command too short");
    return;
  }

  uint8_t which = buffer[2];
  if (which>='a' && which <='d') {
    which-=32;
  }
  if (which<'A' || which >'D') {
    Serial.print("ERR - ");
    Serial.write(which);
    Serial.println(" is not a valid light");
    return;
  }
  which -= 'A';
  uint8_t valid = 0;
  int16_t value = readInt(3,&valid);

  if (!valid) {
    Serial.println("ERR - invalid integer");
    return;
  }

  switch(buffer[1]) {
  case 'v':
    if (value<0 || value > 127) {
      Serial.println("ERR - invalid value");
      return;
    }
    lighting.setLight(which,value);
    break;
  case 'm':
    if (value<0) {
      Serial.println("ERR - invalid mode");
      return;
    }
    lighting.setMode(which,value);
    break;
  default:
    Serial.println("ERR - Unknown lighting command");
  }
}

int16_t BluetoothManager::readInt(uint8_t offset,uint8_t * valid) {
  int16_t toReturn = 0;
  int16_t mult = 1;

  uint8_t bad = false;
  uint8_t any = false;

  if (buffer[offset]=='-') {
    mult = -1;
    ++offset;
  }

  for ( ; offset < bufferOffset; ++offset) {
    toReturn *= 10;
    uint8_t next = buffer[offset];
    if (next>='0' && next<='9') {
      toReturn += (next-'0');
      any = true;
    } 
    else {
      bad = true;
    }
  }

  toReturn *= mult;

  if (valid!=NULL) {
    if (!bad && any) {
      *valid = true;
    } 
    else 
    {
      *valid = false;
    }
  }

  return toReturn;
}








