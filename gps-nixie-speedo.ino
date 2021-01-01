#include "TinyGPS++.h"
#include <SoftwareSerial.h>

enum speedUnit {
  milesPerHour,
  kilometersPerHour
};

static const int GPSBaud = 9600;

static const int _mphLEDPin = 0;
static const int _kphLEDPin = 1;
static const int _dataPin = 2;
static const int _latchPin = 3;
static const int _clockPin = 4;
static const int _rxPin = 8; 
static const int _txPin = 9;
static const int _btnPin = 10;

int _speedValue = 0;
int _lastSpeedValue = -1;

speedUnit _speedUnit = milesPerHour;
speedUnit _lastSpeedUnit = kilometersPerHour;

int _buttonState;             // the current reading from the input pin
int _lastButtonState = LOW;   // the previous reading from the input pin

unsigned long _lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long _debounceDelay = 50;    // the debounce time; increase if the output flickers

SoftwareSerial ss(_rxPin, _txPin);
TinyGPSPlus gps;

void setup()
{
  Serial.begin(9600);
  ss.begin(GPSBaud);

  // Set up the outputs for the shift register
  pinMode(_mphLEDPin, OUTPUT);
  pinMode(_kphLEDPin, OUTPUT);
  pinMode(_latchPin, OUTPUT);
  pinMode(_clockPin, OUTPUT);
  pinMode(_dataPin, OUTPUT);
  pinMode(_btnPin, INPUT);
}

void loop()
{
  int reading = digitalRead(_btnPin);

  Serial.println(reading);
  
  if (reading != _lastButtonState)
  {
    _lastDebounceTime = millis();
  }

  if ((millis() - _lastDebounceTime) > _debounceDelay)
  {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != _buttonState)
    {
      _buttonState = reading;

      // only toggle the LED if the new button state is HIGH
      if (_buttonState == HIGH)
      {
        if (_speedUnit == milesPerHour)
        {
          Serial.println("Setting kph");
          _speedUnit = kilometersPerHour;
        }
        else
        {
          Serial.println("Setting mph");
          _speedUnit = milesPerHour;
        }
      }
    }
  }

  if (_speedUnit != _lastSpeedUnit)
  {
    if (_speedUnit == milesPerHour)
    {
      digitalWrite(_mphLEDPin, HIGH);
      digitalWrite(_kphLEDPin, LOW);  
    }
    else
    {
      digitalWrite(_mphLEDPin, LOW);
      digitalWrite(_kphLEDPin, HIGH);
    }

    _lastSpeedUnit = _speedUnit;
  }

  //Init
  _speedValue = 0;

  //Get speed
  smartDelay(50);

  if (gps.speed.isValid() && gps.speed.isUpdated())
  {
    if (_speedUnit == milesPerHour)
      _speedValue = (int)gps.speed.mph();
    else
      _speedValue = (int)gps.speed.kmph();
  }

  if (_speedValue != _lastSpeedValue)
  {
    byte hundreds = _speedValue / 100;
    byte tens = (_speedValue - (hundreds * 100)) / 10;
    byte units = _speedValue - (hundreds * 100) - (tens * 10);

    byte enable = 0;

    if (hundreds > 0)
    {
      enable = 3;
    }
    else if (tens > 0)
    {
      enable = 2;
    }

    byte first = (units << 4) | tens;
    byte last = (hundreds << 4) | enable;

    digitalWrite(_latchPin, LOW);
    shiftOut(_dataPin, _clockPin, MSBFIRST, first);
    shiftOut(_dataPin, _clockPin, MSBFIRST, last);
    digitalWrite(_latchPin, HIGH);

    _lastSpeedValue = _speedValue;
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
