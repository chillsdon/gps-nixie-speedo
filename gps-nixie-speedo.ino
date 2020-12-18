#include "TinyGPS++.h"
#include <SoftwareSerial.h>

static const int RXPin = 8, TXPin = 9;
static const int GPSBaud = 9600;

static const int _latchPin = 2;
static const int _clockPin = 1;
static const int _dataPin = 3;

int speedValue = 0;
int lastSpeed = -1;
char data[100];
bool sendOutput;

SoftwareSerial ss(RXPin, TXPin);
TinyGPSPlus gps;

void setup()
{
  //Serial.begin(9600);
  //ss.begin(GPSBaud);
}

void loop()
{
  displayCount();

  delay(250);

  speedValue =+1;

  if (speedValue == 10)
    speedValue = 0;

  /*
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
  */
}

void displayCount()
{
  int lower;
  int upper;

  if (speedValue >= 100)
  {
    upper = 1;
    lower = speedValue - 100;
  }
  else
  {
    upper = 0;
    lower = speedValue;
  }

  if (visible)
  {
    
  }
  else
  {
    
  }

  byte lowerByte = encodeNumber(lower);
  byte upperByte = encodeNumber(upper);

  digitalWrite(_latchPin, LOW);
  shiftOut(_dataPin, _clockPin, MSBFIRST, lowerByte);
  shiftOut(_dataPin, _clockPin, MSBFIRST, upperByte);
  digitalWrite(_latchPin, HIGH);
}

byte encodeNumber(int num)
{
  byte tens = num / 10;
  byte units = num - (tens * 10);

  byte data = (units << 4) | tens; 

  return
    data;
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
