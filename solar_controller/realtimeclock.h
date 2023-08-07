#ifndef RealTimeClock_h
#define RealTimeClock_h

#include "RTClib.h"
RTC_DS1307 rtc;

void setupRTC(void)
{
  if (!rtc.begin())
  {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    abort();
  }
  Serial.println("found RTC");
  // keep while developing
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  if (!rtc.isrunning())
  {
    Serial.println("RTC is NOT running, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  // When time needs to be re-set on a previously configured device, the
  // following line sets the RTC to the date & time this sketch was compiled
  // This line sets the RTC with an explicit date & time, for example to set
  // January 21, 2014 at 3am you would call:
  // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));

  // attempt to connect to WiFi network:
  // while (status != WL_CONNECTED)
  // {
  //   Serial.print("Attempting to connect to Network named: ");
  //   Serial.println(ssid); // print the network name (SSID);
  //   // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
  //   status = WiFi.begin(ssid, pass);
  //   // wait 10 seconds for connection:
  //   delay(10000);
  // }
}

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

void printTime()
{
  DateTime now = rtc.now();

  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(" (");
  Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
  Serial.print(") ");
  Serial.print(now.hour() % 12, DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();

  // Serial.print(" since midnight 1/1/1970 = ");
  // Serial.print(now.unixtime());
  // Serial.print("s = ");
  // Serial.print(now.unixtime() / 86400L);
  // Serial.println("d");
  // Serial.println();
}
#endif