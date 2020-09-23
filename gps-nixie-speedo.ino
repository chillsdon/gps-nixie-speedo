#include "TinyGPS++.h"
#include <SoftwareSerial.h>

static const int RXPin = 5, TXPin = 4;
static const int GPSBaud = 9600;

int speed = 0;
int altitude = 0;
char data[100];

SoftwareSerial ss(RXPin, TXPin);
TinyGPSPlus gps;

void setup()
{
  Serial.begin(9600);
  ss.begin(GPSBaud);
}

void loop()
{
  smartDelay(100);  

  if (gps.speed.isValid() && gps.speed.isUpdated())
  {
    speed = (int)gps.speed.mph();
  }

  if (gps.altitude.isValid() && gps.altitude.isUpdated())
  {
    altitude = (int)gps.altitude.feet();
  }

  sprintf(data, "%d mph @ %d elevation (feet)", speed, altitude);
  Serial.println(data);

  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected"));
  }
}

// This custom version of delay() ensures that the gps object
// is being "fed".
static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  
  do 
  {
    while (ss.available())
      gps.encode(ss.read());
  } while (millis() - start < ms);
}
