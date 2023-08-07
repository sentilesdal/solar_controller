#ifndef TerminalCommands_h
#define TerminalCommands_h

#include <SPI.h>
#include <Servo.h>
#include "wifi.h"
#include "Arduino.h"
#include "board.h"

#define lcdSize 3 //this will define number of LCD on the phone app
String protectionPassword = ""; //This will not allow anyone to add or control your board.

Servo servoArray[53];

String lcd[lcdSize];

String httpAppJsonOk = "HTTP/1.1 200 OK \n content-type:application/json \n\n";
String httpTextPlainOk = "HTTP/1.1 200 OK \n content-type:text/plain \n\n";


void updateInput() {
  for (byte i = 0; i < sizeof(pinsMode); i++) {
    if (pinsMode[i] == 'i') {
      pinsValue[i] = digitalRead(i);
    }
  }
}

void allstatus(WiFiClient client) {
  String dataResponse;
  dataResponse += F("HTTP/1.1 200 OK \n");
  dataResponse += F("content-type:application/json \n\n");
  dataResponse += "{";
  dataResponse += "\"m\":[";//m for mode
  for (byte i = 0; i <= digitalArraySize; i++) {
    dataResponse += "\"";
    dataResponse += pinsMode[i];
    dataResponse += "\"";
    if (i != digitalArraySize)dataResponse += ",";
  }
  dataResponse += "],";

  dataResponse += "\"v\":[";//v for value
  for (byte i = 0; i <= digitalArraySize; i++) {
    dataResponse += pinsValue[i];
    if (i != digitalArraySize)dataResponse += ",";
  }
  dataResponse += "],";

  dataResponse += "\"a\":[";//a for analog value
  for (byte i = 0; i <= analogArraySize; i++) {
    dataResponse += analogRead(i);
    if (i != analogArraySize)dataResponse += ",";
  }
  dataResponse += "],";

  dataResponse += "\"l\":[";//l for LCD value
  for (byte i = 0; i <= lcdSize - 1; i++) {
    dataResponse += "\"";
    dataResponse += lcd[i];
    dataResponse += "\"";
    if (i != lcdSize - 1)dataResponse += ",";
  }
  dataResponse += "],";
  dataResponse += "\"t\":\""; //t for Board Type .
  dataResponse += boardType;
  dataResponse += "\",";
  dataResponse += "\"p\":\""; // p for Password.
  dataResponse += protectionPassword;
  dataResponse += "\"";
  dataResponse += "}";
  client.print(dataResponse);
  delay(1); client.stop();
}

void terminalCommand(WiFiClient client) {//Here you recieve data form app terminal
  String data = client.readStringUntil('/');
  client.print(httpAppJsonOk + "Ok from Arduino " + String(random(1, 100)));
  delay(1); client.stop();
  
  Serial.println(data);
}

void digitalCommand(WiFiClient client) {
  int pin, value;
  pin = client.parseInt();
  if (client.read() == '/') {
    value = client.parseInt();
    digitalWrite(pin, value);
    pinsValue[pin] = value;
    client.print(httpAppJsonOk + value);
    delay(1); client.stop();
  }
}

void pwmCommand(WiFiClient client) {
  int pin, value;
  pin = client.parseInt();
  if (client.read() == '/') {
    value = client.parseInt();
    analogWrite(pin, value);
    pinsValue[pin] = value;
    client.print(httpAppJsonOk + value);
    delay(1); client.stop();
  }
}

void servoCommand(WiFiClient client) {
  int pin, value;
  pin = client.parseInt();
  if (client.read() == '/') {
    value = client.parseInt();
    servoArray[pin].write(value);
    pinsValue[pin] = value;
    client.print(httpAppJsonOk + value);
    delay(1); client.stop();
  }
}

void modeCommand(WiFiClient client) {
  String  pinString = client.readStringUntil('/');
  int pin = pinString.toInt();
  String mode = client.readStringUntil('/');
  if (mode != "servo") {
    servoArray[pin].detach();
  };

  if (mode == "output") {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, 0);
    pinsMode[pin] = 'o';
    pinsValue[pin] = 0;
    allstatus(client);
  }
  if (mode == "push") {
    pinsMode[pin] = 'm';
    pinsValue[pin] = 0;
    pinMode(pin, OUTPUT);
    digitalWrite(pin, 0);
    allstatus(client);
  }
  if (mode == "schedule") {
    pinsMode[pin] = 'c';
    pinsValue[pin] = 0;
    pinMode(pin, OUTPUT);
    digitalWrite(pin, 0);
    allstatus(client);
  }

  if (mode == "input") {
    pinsMode[pin] = 'i';
    pinsValue[pin] = 0;
    pinMode(pin, INPUT);
    allstatus(client);
  }

  if (mode == "pwm") {
    pinsMode[pin] = 'p';
    pinsValue[pin] = 0;
    pinMode(pin, OUTPUT);
    analogWrite(pin, 0);
    allstatus(client);
  }

  if (mode == "servo") {
    pinsMode[pin] = 's';
    pinsValue[pin] = 0;
    servoArray[pin].attach(pin);
    servoArray[pin].write(0);
    allstatus(client);
  }

}

void allonoff(WiFiClient client) {
  int pin, value;
  value = client.parseInt();
  for (byte i = 0; i <= 16; i++) {
    if (pinsMode[i] == 'o') {
      digitalWrite(i, value);
      pinsValue[i] = value;
    }
  }
  client.print(httpTextPlainOk + value);
  delay(1); client.stop();
}

void changePassword(WiFiClient client) {
  String data = client.readStringUntil('/');
  protectionPassword = data;
  client.print(httpAppJsonOk);
  delay(1); client.stop();
}




void update_input() {
  for (byte i = 0; i < sizeof(pinsMode); i++) {
    if (pinsMode[i] == 'i') {
      pinsValue[i] = digitalRead(i);
    }
  }
}




void process(WiFiClient client) {
  String getString = client.readStringUntil('/');
  String arduinoString = client.readStringUntil('/');
  String command = client.readStringUntil('/');

  if (command == "digital") {
    digitalCommand(client);
  }

  if (command == "pwm") {
    pwmCommand(client);
  }

  if (command == "servo") {
    servoCommand(client);
  }

  if (command == "terminal") {
    terminalCommand(client);
  }

  if (command == "mode") {
    modeCommand(client);
  }

  if (command == "allonoff") {
    allonoff(client);
  }

  if (command == "password") {
    changePassword(client);
  }

  if (command == "allstatus") {
    allstatus(client);
  }
}

void processWifiCommands()
{
  lcd[0] = "Test 1 LCD";// you can send any data to your mobile phone.
  lcd[1] = "Test 2 LCD";// you can send any data to your mobile phone.
  lcd[2] = String(analogRead(1));//  send analog value of A1

  WiFiClient client = server.available();
  if (client)
  { // if you get a client,
    while (client.connected()) { // loop while the client's connected,
      if (client.available()) { // if there's bytes to read from the client,
        process(client); // then process the command.
        break;
      }
    }
  }
}

#endif