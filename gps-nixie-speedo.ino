#include "TinyGPS++.h"
#include <SoftwareSerial.h>

static const int RXPin = 8, TXPin = 9;
static const int GPSBaud = 9600;

int speed = 0;
int lastSpeed = -1;
char data[100];
bool sendOutput;

SoftwareSerial ss(RXPin, TXPin);
TinyGPSPlus gps;

void setup()
{
  Serial.begin(9600);
  ss.begin(GPSBaud);
}

void loop()
{
  smartDelay(50);  

  //Serial.println(F("Hello..."));

  sendOutput = false;

  if (gps.speed.isValid() && gps.speed.isUpdated())
  {
    speed = (int)gps.speed.mph();
    sendOutput = true;
  }

  if (sendOutput && speed != lastSpeed)
  {
    sprintf(data, "%d mph", speed);

    Serial.println(data);

    lastSpeed = speed;
  }

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
