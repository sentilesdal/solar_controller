/*
  Title  : Arduino MKR1000
  version: V4.1
  Contact: info@tatco.cc
  Done By: TATCO Inc.
  github : https://github.com/rabee2050/arduino-mkr1000
  iOS    : https://itunes.apple.com/us/app/mkr1000-kit/id1330616500?ls=1&mt=8
  Android: https://play.google.com/store/apps/details?id=com.tatco.mkr1000

  Release Notes:
  - V1 Created 1 Jan 2018
  - V2 Skipped
  - V3 Skipped
  - V4 Updated 10 Oct 2018
  - V4.1 Updated 06 Apr 2019 / Minor Changes

*/

#include <SPI.h>
#include "wifi.h"
#include "board.h"
#include "terminal.h"
#include "realtimeclock.h"
#include "constants.h"

void setup(void)
{
  delay(5000);
  Serial.println("Initializing board.");
  Serial.begin(115200); // initialize serial communication
  Serial.println("Initialized serial communication.");

#ifndef ESP8266
  while (!Serial)
    ; // wait for serial port to connect. Needed for native USB
#endif

  setupRTC(); // initialize the real time clock
  Serial.println("Initialized real time clock.");
  printTime();
  connectToWifi(); // connect to wifi.
  Serial.println("Connected to wifi.");
  printWiFiStatus(); // you're connected now, so print out the status
  boardInit();       // initialize pins that control power
  Serial.println("Initialized board.");
}

int lastFiveSeconds = 0;

void loop(void)
{
  // process wifi commands which can override pin values
  processWifiCommands();
  printWifiSerial();
  updatePinValues();

  // print time every 5 seconds 
  DateTime now = rtc.now();
  int fiveSeconds = now.second() - now.second() % 5;
  if (fiveSeconds != lastFiveSeconds) {
    lastFiveSeconds = fiveSeconds;
    printTime();
  }
  updateLights();
  updateEvapotron();
}

int lightStatus = 0;
void updateLights()
{
  int hr = hour();
  // turn on lights from 8pm to 5am
  if ((hr >= 20 || hr < 5) && lightStatus == 0)
  {
    setLights(1);
    Serial.println("turning lights on");
    lightStatus = 1;
  }

  // turn off lights from 5am to 8pm
  if (hr >= 5 && hr < 20 && lightStatus == 1)
  {
    setLights(0);
    Serial.println("turning lights off");
    lightStatus = 0;
  }
}

int evapotronStatus = 0;
void updateEvapotron()
{
  int hr = hour();
  // turn on evapotron from 10am to 5pm
  if (hr >= 10 && hr < 17 && evapotronStatus == 0)
  {
    setEvapotron(1);
    Serial.println("turning evapotron on");
    evapotronStatus = 1;
  }

  // turn off evapotron from 5pm to 10am
  if ((hr >= 17 || hr < 10) && evapotronStatus == 1)
  {
    setEvapotron(0);
    Serial.println("turning evapotron off");
    evapotronStatus = 0;
  }
}

void setLights(int setting)
{
  digitalWrite(0, setting);
  digitalWrite(1, setting);
  digitalWrite(2, setting);
  digitalWrite(3, setting);
  digitalWrite(4, setting);
  digitalWrite(5, setting);
  digitalWrite(6, setting);
  digitalWrite(7, setting);
}

void setEvapotron(int setting)
{
  digitalWrite(8, setting);
}

void updatePinValues()
{
  for (byte i = 0; i < sizeof(pinsMode); i++)
  {
    if (pinsMode[i] == 'i')
    {
      pinsValue[i] = digitalRead(i);
    }
  }
}
