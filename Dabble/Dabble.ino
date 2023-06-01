#define CUSTOM_SETTINGS
#define INCLUDE_GAMEPAD_MODULE
#include <Dabble.h>

long watchdog;  // 
long timer;     // Timer for 2 second reporting

void setup() {
  // put your setup code here, to run once:
  Serial.begin(57600);      // make sure your Serial Monitor is also set at this baud rate.
  Dabble.begin(9600);       //Enter baudrate of your bluetooth.Connect bluetooth on Bluetooth port present on evive.
  Serial.println("Started");
  timer = millis();
}

void loop() {
  // put your main code here, to run repeatedly:
  Dabble.processInput();

  if ((GamePad.isTrianglePressed()) || (GamePad.isStartPressed()))
  {
    Serial.println("GreenMode ON");
  }

  if (GamePad.isCrossPressed())
  {
    Serial.println("GreenMode OFF");
  }
}
