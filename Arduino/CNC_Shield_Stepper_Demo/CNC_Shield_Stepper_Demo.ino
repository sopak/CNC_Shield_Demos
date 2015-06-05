//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  A demo for the CNC Shield v3.xx with steppers on Arduino
/*
    Copyright (c) 2015 Martin F. Falatic
    
    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at
    
        http://www.apache.org/licenses/LICENSE-2.0
    
    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

const int NUM_AXES = 4;
const int axes[NUM_AXES][2] = {{2,5}, {3,6}, {4,7}, {12,13}};
const int enablePin = 8;
int stepPin, dirPin;
int axisIdx, stepCnt;

const int INPUT_BUFLEN = 200;
String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete
boolean steppersEnabled = false;
boolean inputBufferTruncated = false;

void setup() {
  inputString.reserve(INPUT_BUFLEN);
  Serial.begin(9600);
  Serial.println("Stepper exerciser for CNC Shield v3.xx");
  Serial.println("Enter \"on\" to enable the steppers");
  Serial.println("Enter \"off\" to disable them (once the current test cycle is complete)");

  pinMode(enablePin, OUTPUT);
  digitalWrite(enablePin, HIGH);
  steppersEnabled = false;

  for (axisIdx = 0; axisIdx < NUM_AXES; axisIdx++) {
    stepPin = axes[axisIdx][0];
    dirPin  = axes[axisIdx][1];
    pinMode(stepPin, OUTPUT);
    pinMode(dirPin, OUTPUT);
    digitalWrite(stepPin, LOW);
  }
}

void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read(); 
    if (inChar == '\n') {
      stringComplete = true;
    }
    else {
      if (inputString.length()+2 > INPUT_BUFLEN) {
        inputBufferTruncated = true;
      }
      else {
        inputString += inChar;
      }
    }
  }
}

void loop() {
  if (stringComplete) {
    Serial.print("-- Input received: \"");
    Serial.print(inputString);
    Serial.println("\"");
    if (inputBufferTruncated) {
      Serial.println("-- Note: Input was truncated!");
      inputBufferTruncated = false;
    }
    inputString.toLowerCase();
    if (inputString == "on") {
      Serial.println("-- Enabling steppers");
      digitalWrite(enablePin, HIGH);
      steppersEnabled = true;
    }
    else if (inputString == "off") {
      Serial.println("-- Disabling steppers");
      digitalWrite(enablePin, LOW);
      steppersEnabled = false;
    }
    stringComplete = false;
    inputString = "";
  }

  if (steppersEnabled) {
    Serial.println("-- Executing test loop");
    int numSteps = 128;
    int stepDelay_L2H = 20;
    int stepDelay_H2L = 20;
    int dirChangeDelay = 500;

    Serial.println("Direction = HIGH");
    for(stepCnt = 0; stepCnt < numSteps; stepCnt++) {
      for (axisIdx = 0; axisIdx < NUM_AXES; axisIdx++) {
        stepPin = axes[axisIdx][0];
        dirPin  = axes[axisIdx][1];
        digitalWrite(dirPin, HIGH);
        digitalWrite(stepPin, HIGH);
      }
      delay(stepDelay_H2L);
      for (axisIdx = 0; axisIdx < NUM_AXES; axisIdx++) {
        stepPin = axes[axisIdx][0];
        dirPin  = axes[axisIdx][1];
        digitalWrite(stepPin, LOW);
      }
      delay(stepDelay_L2H);
    }
    delay(dirChangeDelay);

    Serial.println("Direction = LOW");
    for(stepCnt = 0; stepCnt < numSteps; stepCnt++) {
      for (axisIdx = 0; axisIdx < NUM_AXES; axisIdx++) {
        stepPin = axes[axisIdx][0];
        dirPin  = axes[axisIdx][1];
        digitalWrite(dirPin, LOW);
        digitalWrite(stepPin, HIGH);
      }
      delay(stepDelay_H2L);
      for (axisIdx = 0; axisIdx < NUM_AXES; axisIdx++) {
        stepPin = axes[axisIdx][0];
        dirPin  = axes[axisIdx][1];
        digitalWrite(stepPin, LOW);
      }
      delay(stepDelay_L2H);
    }
    delay(dirChangeDelay);
  }
}

