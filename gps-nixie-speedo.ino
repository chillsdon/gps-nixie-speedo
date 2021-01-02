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

const int DEBOUNCE_DELAY = 150;   // the debounce time; increase if the output flickers

// Variables will change:
int _lastSteadyState = LOW;       // the previous steady state from the input pin
int _lastFlickerableState = LOW;  // the previous flickerable state from the input pin
int _currentState;                // the current reading from the input pin

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long _lastDebounceTime = 0;  // the last time the output pin was toggled

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
  // read the state of the switch/button:
  _currentState = digitalRead(_btnPin);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch/button changed, due to noise or pressing:
  if (_currentState != _lastFlickerableState) {
    // reset the debouncing timer
    _lastDebounceTime = millis();
    // save the the last flickerable state
    _lastFlickerableState = _currentState;
  }

  if ((millis() - _lastDebounceTime) > DEBOUNCE_DELAY) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if(_lastSteadyState == HIGH && _currentState == LOW)
    {
      Serial.println("The button is pressed");
      
    }
    else if (_lastSteadyState == LOW && _currentState == HIGH)
    {
      Serial.println("The button is released");

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

    // save the the last steady state
    _lastSteadyState = _currentState;
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
