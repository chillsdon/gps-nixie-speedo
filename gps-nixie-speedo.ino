//#include "TinyGPS++.h"
//#include <SoftwareSerial.h>

static const int RXPin = 8, TXPin = 9;
static const int GPSBaud = 9600;

static const int _latchPin = 3;
static const int _clockPin = 2;
static const int _dataPin = 4;

int speedValue = 0;
int lastSpeed = -1;
char data[100];
bool sendOutput;
byte speedBytes[2];

//SoftwareSerial ss(RXPin, TXPin);
//TinyGPSPlus gps;

void setup()
{
  Serial.begin(9600);
  //ss.begin(GPSBaud);

  // Set up the outputs for the shift register
  pinMode(_latchPin, OUTPUT);
  pinMode(_clockPin, OUTPUT);
  pinMode(_dataPin, OUTPUT);
}

void loop()
{
  byte a = 1;
  byte b = 2;
  byte c = 3;
  byte d = 4;

  byte first = (a << 4) | b;
  byte last = (c << 4) | d;

  digitalWrite(_latchPin, LOW);
  shiftOut(_dataPin, _clockPin, MSBFIRST, first);
  shiftOut(_dataPin, _clockPin, MSBFIRST, last);
  digitalWrite(_latchPin, HIGH);
  
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

void displayCount()
{
  encodeSpeed(speedValue);

  //speedBytes[0] = (2 << 4) | 0;
  //speedBytes[1] = (0 << 4) | 0;

  digitalWrite(_latchPin, LOW);
  shiftOut(_dataPin, _clockPin, MSBFIRST, speedBytes[0]);
  shiftOut(_dataPin, _clockPin, MSBFIRST, speedBytes[1]);
  digitalWrite(_latchPin, HIGH);
}

void encodeSpeed(int value)
{
  byte enable = 0;
  
  byte hundreds = value / 100;
  byte tens = (value - (hundreds * 100)) / 10;
  byte units = value - (hundreds * 100) - (tens * 10);

  if (hundreds > 0)
    enable = enable | 1;

  if (tens > 0)
    enable = enable | 2;

  enable = 15;

  //if (units % 2 == 0)
  //  enable = 1;

  Serial.print("Value: ");
  Serial.println(value);
  Serial.print("Hundreds: ");
  Serial.println(hundreds);
  Serial.print("Tens: ");
  Serial.println(tens);
  Serial.print("Units: ");
  Serial.println(units);

  speedBytes[0] = (units << 4) | tens;
  speedBytes[1] = (hundreds << 4) | enable;
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
