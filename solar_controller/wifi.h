#ifndef SolarWifi_h
#define SolarWifi_h

#include <TimeLib.h>
#include <WiFiNINA.h>

// For testing at home
const char *ssid = "TheGrove";         // your network SSID (name)
const char *pass = "2biscuits4grovey"; // your network password (use for WPA, or use as key for WEP)

// const char *ssid = "Matthew's iPhone"; // your network SSID (name)
// const char *pass = "buttsonbutts";     // your network password (use for WPA, or use as key for WEP)

// For production
// char ssid[] = "avantcard"; // your network SSID (name)
// char pass[] = "goodlife";  // your network password (use for WPA, or use as key for WEP)

int status = WL_IDLE_STATUS;
WiFiServer server(80);

// TODO: replace with RTC
unsigned long serialTimer = millis();

void connectToWifi()
{
  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED)
  {
    Serial.print("Wifi status code: ");
    Serial.println(WiFi.status());
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid); // print the network name (SSID);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    // wait 5 seconds for connection:
    delay(5000);
    server.begin();    // start the web server on port 80

  }
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
#endif