#include "TinyGPS++.h"
#include <SoftwareSerial.h>

static const int RXPin = 9, TXPin = 8;
static const int GPSBaud = 9600;

int speed = 0;

SoftwareSerial ss(RXPin, TXPin);
TinyGPSPlus gps;

void setup()
{
  Serial.begin(115200);
  ss.begin(GPSBaud);
}

void loop()
{
  smartDelay(250);  

  if (gps.speed.isValid())
  {
    speed = (int)gps.speed.mph();

    Serial.print(speed);
    Serial.println(" mph");
  }

  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
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
