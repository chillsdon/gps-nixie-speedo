#include "TinyGPS++.h"
#include <SoftwareSerial.h>

static const int GPSBaud = 9600;

static const int _greenLEDPin = 0;
static const int _redLEDPin = 1;

static const int _dataPin = 2;
static const int _latchPin = 3;
static const int _clockPin = 4;

static const int _rxPin = 8; 
static const int _txPin = 9;

int speedValue = 0;
int lastSpeed = -1;

bool greenOn = true;

SoftwareSerial ss(_rxPin, _txPin);
TinyGPSPlus gps;

void setup()
{
  Serial.begin(9600);
  ss.begin(GPSBaud);

  // Set up the outputs for the shift register
  pinMode(_greenLEDPin, OUTPUT);
  pinMode(_redLEDPin, OUTPUT);
  pinMode(_latchPin, OUTPUT);
  pinMode(_clockPin, OUTPUT);
  pinMode(_dataPin, OUTPUT);
}

void loop()
{
  if (greenOn)
  {
    digitalWrite(_greenLEDPin, HIGH);
    digitalWrite(_redLEDPin, LOW);  
  }
  else
  {
    digitalWrite(_greenLEDPin, LOW);
    digitalWrite(_redLEDPin, HIGH);
  }
  
  greenOn = !greenOn;
  
  //byte a = speedValue;
  //byte b = speedValue;
  //byte c = speedValue;
  //byte d = speedValue;

  //byte enable = 3;
  byte hundreds = speedValue / 100;
  byte tens = (speedValue - (hundreds * 100)) / 10;
  byte units = speedValue - (hundreds * 100) - (tens * 10);

  byte enable = 0;

  if (hundreds > 0)
  {
    enable = 3;
  }
  else if (tens > 0)
  {
    enable = 2;
  }

  //byte first = (a << 4) | b;
  //byte last = (c << 4) | d;

  byte first = (units << 4) | tens;
  byte last = (hundreds << 4) | enable;

  digitalWrite(_latchPin, LOW);
  shiftOut(_dataPin, _clockPin, MSBFIRST, first);
  shiftOut(_dataPin, _clockPin, MSBFIRST, last);
  digitalWrite(_latchPin, HIGH);

  speedValue += 1;

  if (speedValue > 999)
    speedValue = 0;

  delay(250);
  
  /*
  Serial.print("speedValue: ");
  Serial.println(speedValue);
  
  displayCount();

  delay(3000);

  speedValue += 1;

  if (speedValue == 10)
    speedValue = 0;
  */

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

// This custom version of delay() ensures that the gps object
// is being "fed".
static void smartDelay(unsigned long ms)
{
  /*
  unsigned long start = millis();
  
  do 
  {
    while (ss.available())
      gps.encode(ss.read());
  } while (millis() - start < ms);
  */
}
