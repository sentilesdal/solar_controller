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

String getDateTimeString()
{
  DateTime now = rtc.now();
  String dateTimeStr = "";

  // Add the year, month, day to the string
  dateTimeStr += String(now.year(), DEC);
  dateTimeStr += "/";
  dateTimeStr += String(now.month(), DEC);
  dateTimeStr += "/";
  dateTimeStr += String(now.day(), DEC);

  // Add the day of the week
  dateTimeStr += " (";
  dateTimeStr += daysOfTheWeek[now.dayOfTheWeek()];
  dateTimeStr += ") ";

  // convert 24hr to 12hr clock
  int hour = now.hour() % 12;
  if (hour == 0)
  {
    hour = 12;
  }
  // Add the hour, minute, and second to the string
  dateTimeStr += String(hour, DEC);
  dateTimeStr += ":";
  dateTimeStr += String(now.minute(), DEC);
  dateTimeStr += ":";
  dateTimeStr += String(now.second(), DEC);

  return dateTimeStr;
}

void printTime()
{
  String dateTime = getDateTimeString();
  Serial.println(dateTime);
}

#endif