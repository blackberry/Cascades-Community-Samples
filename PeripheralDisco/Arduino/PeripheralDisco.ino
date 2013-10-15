/**
 * Copyright (c) 2013 BlackBerry Limited.
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

/*
 *
 * Code designed to work with JAM804 Cascades App.
 *
 * Servo data pin should be connected to Arduino pin 7.
 *
 */
#include <serial.h>
#include <Servo.h>

int led = 13;

int position = 90;

uint8_t connected = 0;
unsigned long lastTime = 0;
unsigned long lastFlash = 0;

Servo servo;

char buffer[20];
uint32_t bufferOffset = 0;

void setup() {
  // Set baud to 9600
  Serial.begin(9600);
  
  // Make sure we can blink the built in LED.
  pinMode(led, OUTPUT);
  
  // Attach to a servo on pin 7.
  servo.attach(7);
  servo.write(position);
  
  lastFlash = millis();
}

uint8_t equals(char * val) {
  for (int i=0; ; ++i) {
    if (val[i]==0) {
      return i==bufferOffset;
    }
    if (i==bufferOffset) {
      return false;
    }
    if (val[i]!=buffer[i]) {
      return false;
    }
  }
  return false;
}

uint8_t startsWith(char * val) {
  for (int i=0; ; ++i) {
    if (val[i]==0) {
      return true;
    }
    if (i==bufferOffset) {
      return false;
    }
    if (val[i]!=buffer[i]) {
      return false;
    }
  }
  return false;
}

void reset() {
  Serial.println("Arduino BlackBerry 10 USB Demo");
  connected = 1;
  lastTime = lastFlash = millis();
  position = 90;
  servo.write(position);
}

void bye() {
  Serial.println("So long boys!");
  connected = 0;
  position = 90;
  servo.write(position);
}

void set() {
  debug("Set servo position");
  int val = (buffer[4]&0xff);
  servo.write(val);
}

void debug(char * message) {
//  Serial.println(message);
}

void doRead() {
  debug("doRead");
  while(Serial.available()>0) {
    debug("while");
    if (bufferOffset>=20) {
      bufferOffset = 0;
    }
    char next = Serial.read();
    buffer[bufferOffset++] = next;
    if (next=='\n' || next=='\r') {
      break;
    }
  }
  
  if (bufferOffset>0 && (buffer[bufferOffset-1]=='\n') || buffer[bufferOffset-1]=='\r') {
    debug("Line");
    bufferOffset--;
    if (equals("RESET")) {
      reset();
    } else if (startsWith("SET ")) {
      if (bufferOffset==5) {
        set();
      } else {
        Serial.println("Malformed SET");
      }
    } else if (equals("BYE")) {
      bye();
    } else {
      Serial.print("Unknown command: ");
      for (int i=0; i< bufferOffset; ++i) {
        Serial.print(buffer[i]);
      }
      Serial.println();
      debug("Unknown Command");
    }
    bufferOffset = 0;
  } else {
    debug("No line...");
  }
}

void loop() {
  if (Serial.available() > 0) { 
    doRead();
  }
  unsigned long time = millis();
  unsigned long delta = time - lastTime;
  if (connected) {
    if (delta >= 10000 && delta <= 120000) {
      lastTime = lastTime + 10000;
      Serial.write('.');
    }
  }
  
  delta = time - lastFlash;
  int period = connected ? 500 : 50;
  
  if (delta >= period && delta <= 120000) {
    lastFlash = lastFlash + period;
    digitalWrite(led,1 - digitalRead(led));
  }
}  

