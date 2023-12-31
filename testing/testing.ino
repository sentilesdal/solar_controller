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
#include <WiFi101.h>
#include <Servo.h>
#include <TimeLib.h>
#include "RTClib.h"

RTC_DS1307 rtc;
#include <arduino-timer.h>

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

auto timer = timer_create_default(); // create a timer with default settings
Timer<> default_timer;               // save as above

// string manipulation stuff
#define SPTR_SIZE 20
char *sPtr[SPTR_SIZE];

// constants
int HOUR_IN_MS = 60 * 60 * 1000;
int O_CLOCK = HOUR_IN_MS;
int DAY_IN_MS = 24 * HOUR_IN_MS;
// int TEN_HOURS = HOUR_IN_MS * 10;
// int FOURTEEN_HOURS = HOUR_IN_MS * 14;
int TEN_HOURS = 100 * 10;
int FOURTEEN_HOURS = 100 * 14;
int SEVENTEEN_HOURS = HOUR_IN_MS * 17;
int SEVEN_HOURS = HOUR_IN_MS * 7;

char ssid[] = "avantcard"; // your network SSID (name)
char pass[] = "goodlife";  // your network password (use for WPA, or use as key for WEP)

int status = WL_IDLE_STATUS;
WiFiServer server(80);

#define lcdSize 3               // this will define number of LCD on the phone app
String protectionPassword = ""; // This will not allow anyone to add or control your board.
String boardType;

char pinsMode[54];
int pinsValue[54];
Servo servoArray[53];

String lcd[lcdSize];
unsigned long serialTimer = millis();
byte digitalArraySize, analogArraySize;

String httpAppJsonOk = "HTTP/1.1 200 OK \n content-type:application/json \n\n";
String httpTextPlainOk = "HTTP/1.1 200 OK \n content-type:text/plain \n\n";

void setup(void)
{
  Serial.begin(115200); // initialize serial communication
#ifndef ESP8266
  while (!Serial); // wait for serial port to connect. Needed for native USB
#endif

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    abort();
  }

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED)
  {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid); // print the network name (SSID);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    // wait 10 seconds for connection:
    delay(10000);
  }

  server.begin();     // start the web server on port 80
  printWiFiStatus();  // you're connected now, so print out the status
  boardInit();        // Init the board
  setTime(20 * 3600); // default to 8pm
}

void loop(void)
{
  WiFiClient client = server.available();
  if (client)
  { // if you get a client,
    while (client.connected())
    { // loop while the client's connected
      if (client.available())
      { // if there's bytes to read from the client,
        process(client);
        break;
      }
    }
  }
  update_input();
  printWifiSerial();

  delay(500);
  printTime();
  delay(500);
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
  // for (int i; i < 8; i++)
  // {
  digitalWrite(0, setting);
  digitalWrite(1, setting);
  digitalWrite(2, setting);
  digitalWrite(3, setting);
  digitalWrite(4, setting);
  digitalWrite(5, setting);
  digitalWrite(6, setting);
  digitalWrite(7, setting);
  // }
}

void setEvapotron(int setting)
{
  digitalWrite(8, setting);
}

void process(WiFiClient client)
{
  String getString = client.readStringUntil('/');
  String arduinoString = client.readStringUntil('/');
  String command = client.readStringUntil('/');

  if (command == "digital")
  {
    digitalCommand(client);
  }

  if (command == "pwm")
  {
    pwmCommand(client);
  }

  if (command == "servo")
  {
    servoCommand(client);
  }

  if (command == "terminal")
  {
    terminalCommand(client);
  }

  if (command == "mode")
  {
    modeCommand(client);
  }

  if (command == "allonoff")
  {
    allonoff(client);
  }

  if (command == "password")
  {
    changePassword(client);
  }

  if (command == "allstatus")
  {
    allstatus(client);
  }

  Serial.print(getString);
  Serial.print(arduinoString);
  Serial.print(command);
}

void terminalCommand(WiFiClient client)
{ // Here you recieve data form app terminal
  String data = client.readStringUntil('/');
  int N = separate(data, sPtr, SPTR_SIZE);

  String command = sPtr[0];
  String param1 = sPtr[1];
  String param2 = sPtr[2];

  Serial.println(" ");
  Serial.print("command: ");
  Serial.println(command);
  Serial.println(" ");

  Serial.print("param1: ");
  Serial.println(param1);
  Serial.println(" ");

  Serial.print("param2: ");
  Serial.println(param2);
  Serial.println(" ");

  client.print(httpAppJsonOk + "Ok from Arduino " + String(random(1, 100)));

  if (command == "time")
  {
    // adjust time to Pacific Daylight Time
    setTime(param1.toInt() - 7 * 3600);
  }

  delay(1);
  client.stop();
}

void digitalCommand(WiFiClient client)
{
  int pin, value;
  pin = client.parseInt();
  if (client.read() == '/')
  {
    value = client.parseInt();
    digitalWrite(pin, value);
    pinsValue[pin] = value;
    client.print(httpAppJsonOk + value);
    delay(1);
    client.stop();
  }
}

void pwmCommand(WiFiClient client)
{
  int pin, value;
  pin = client.parseInt();
  if (client.read() == '/')
  {
    value = client.parseInt();
    analogWrite(pin, value);
    pinsValue[pin] = value;
    client.print(httpAppJsonOk + value);
    delay(1);
    client.stop();
  }
}

void servoCommand(WiFiClient client)
{
  int pin, value;
  pin = client.parseInt();
  if (client.read() == '/')
  {
    value = client.parseInt();
    servoArray[pin].write(value);
    pinsValue[pin] = value;
    client.print(httpAppJsonOk + value);
    delay(1);
    client.stop();
  }
}

void modeCommand(WiFiClient client)
{
  String pinString = client.readStringUntil('/');
  int pin = pinString.toInt();
  String mode = client.readStringUntil('/');
  if (mode != "servo")
  {
    servoArray[pin].detach();
  };

  if (mode == "output")
  {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, 0);
    pinsMode[pin] = 'o';
    pinsValue[pin] = 0;
    allstatus(client);
  }
  if (mode == "push")
  {
    pinsMode[pin] = 'm';
    pinsValue[pin] = 0;
    pinMode(pin, OUTPUT);
    digitalWrite(pin, 0);
    allstatus(client);
  }
  if (mode == "schedule")
  {
    pinsMode[pin] = 'c';
    pinsValue[pin] = 0;
    pinMode(pin, OUTPUT);
    digitalWrite(pin, 0);
    allstatus(client);
  }

  if (mode == "input")
  {
    pinsMode[pin] = 'i';
    pinsValue[pin] = 0;
    pinMode(pin, INPUT);
    allstatus(client);
  }

  if (mode == "pwm")
  {
    pinsMode[pin] = 'p';
    pinsValue[pin] = 0;
    pinMode(pin, OUTPUT);
    analogWrite(pin, 0);
    allstatus(client);
  }

  if (mode == "servo")
  {
    pinsMode[pin] = 's';
    pinsValue[pin] = 0;
    servoArray[pin].attach(pin);
    servoArray[pin].write(0);
    allstatus(client);
  }
}

void allonoff(WiFiClient client)
{
  int pin, value;
  value = client.parseInt();
  for (byte i = 0; i <= 16; i++)
  {
    if (pinsMode[i] == 'o')
    {
      digitalWrite(i, value);
      pinsValue[i] = value;
    }
  }
  client.print(httpTextPlainOk + value);
  delay(1);
  client.stop();
}

void changePassword(WiFiClient client)
{
  String data = client.readStringUntil('/');
  protectionPassword = data;
  client.print(httpAppJsonOk);
  delay(1);
  client.stop();
}

void allstatus(WiFiClient client)
{
  String dataResponse;
  dataResponse += F("HTTP/1.1 200 OK \n");
  dataResponse += F("content-type:application/json \n\n");
  dataResponse += "{";
  dataResponse += "\"m\":["; // m for mode
  for (byte i = 0; i <= digitalArraySize; i++)
  {
    dataResponse += "\"";
    dataResponse += pinsMode[i];
    dataResponse += "\"";
    if (i != digitalArraySize)
      dataResponse += ",";
  }
  dataResponse += "],";

  dataResponse += "\"v\":["; // v for value
  for (byte i = 0; i <= digitalArraySize; i++)
  {
    dataResponse += pinsValue[i];
    if (i != digitalArraySize)
      dataResponse += ",";
  }
  dataResponse += "],";

  dataResponse += "\"a\":["; // a for analog value
  for (byte i = 0; i <= analogArraySize; i++)
  {
    dataResponse += analogRead(i);
    if (i != analogArraySize)
      dataResponse += ",";
  }
  dataResponse += "],";

  dataResponse += "\"l\":["; // l for LCD value
  for (byte i = 0; i <= lcdSize - 1; i++)
  {
    dataResponse += "\"";
    dataResponse += lcd[i];
    dataResponse += "\"";
    if (i != lcdSize - 1)
      dataResponse += ",";
  }
  dataResponse += "],";
  dataResponse += "\"t\":\""; // t for Board Type .
  dataResponse += boardType;
  dataResponse += "\",";
  dataResponse += "\"p\":\""; // p for Password.
  dataResponse += protectionPassword;
  dataResponse += "\"";
  dataResponse += "}";
  client.print(dataResponse);
  delay(1);
  client.stop();
}

void update_input()
{
  for (byte i = 0; i < sizeof(pinsMode); i++)
  {
    if (pinsMode[i] == 'i')
    {
      pinsValue[i] = digitalRead(i);
    }
  }
}

void boardInit()
{
  for (byte i = 0; i <= 14; i++)
  {
    pinsMode[i] = 'o';
    pinsValue[i] = 0;
    pinMode(i, OUTPUT);
  }
  boardType = "mkr1000";
  digitalArraySize = 14;
  analogArraySize = 6;
}

void printWiFiStatus()
{
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

void printWifiSerial()
{
  if (Serial.read() > 0)
  {
    if (millis() - serialTimer > 3000)
    {
      printWiFiStatus();
    }
    serialTimer = millis();
  }
}

int separate(
    String str,
    char **p,
    int size)
{
  int n;
  char s[100];

  strcpy(s, str.c_str());

  *p++ = strtok(s, ".");
  for (n = 1; NULL != (*p++ = strtok(NULL, ".")); n++)
    if (size == n)
      break;

  return n;
}

void printTime () {
    DateTime now = rtc.now();

    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" (");
    Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    Serial.print(") ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();

    Serial.print(" since midnight 1/1/1970 = ");
    Serial.print(now.unixtime());
    Serial.print("s = ");
    Serial.print(now.unixtime() / 86400L);
    Serial.println("d");
    Serial.println();
}
