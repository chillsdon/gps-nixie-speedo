#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>

#define PMTK_SET_NMEA_UPDATE_1HZ  "$PMTK220,1000*1F"
#define PMTK_SET_NMEA_UPDATE_5HZ  "$PMTK220,200*2C"
#define PMTK_SET_NMEA_UPDATE_10HZ "$PMTK220,100*2F"

// turn on only the second sentence (GPRMC)
#define PMTK_SET_NMEA_OUTPUT_RMCONLY "$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29"
// turn on GPRMC and GGA
#define PMTK_SET_NMEA_OUTPUT_RMCGGA "$PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28"
// turn on ALL THE DATA
#define PMTK_SET_NMEA_OUTPUT_ALLDATA "$PMTK314,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0*28"
// turn off output
#define PMTK_SET_NMEA_OUTPUT_OFF "$PMTK314,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28"

#define PMTK_Q_RELEASE "$PMTK605*31"

enum speedUnit {
  milesPerHour,
  kilometersPerHour
};

static const int GPSBaud = 9600;

static const float mphMultiplier = 1.15078;
static const float kphMultiplier = 1.852;

static const int _mphLEDPin = 0;
static const int _kphLEDPin = 1;
static const int _dataPin = 2;
static const int _latchPin = 3;
static const int _clockPin = 4;
static const int _rxPin = 8; 
static const int _txPin = 9;
static const int _btnPin = 10;

float _lastKnots = 0.0;
int _speedValue = 0;
int _lastSpeedValue = -1;
bool _unitsChanged = true;
bool _serialEnable = false;
bool _displayCounter = false;
int _counter = 0;
speedUnit _speedUnit;

SoftwareSerial ss(_txPin, _rxPin);
Adafruit_GPS GPS(&ss);

void setup()
{
  // Set up the outputs for the shift register
  pinMode(_mphLEDPin, OUTPUT);
  pinMode(_kphLEDPin, OUTPUT);
  pinMode(_latchPin, OUTPUT);
  pinMode(_clockPin, OUTPUT);
  pinMode(_dataPin, OUTPUT);
  pinMode(_btnPin, INPUT);

  if (_serialEnable)
  {
    Serial.begin(115200);
    delay(2000);
  }
  
  GPS.begin(GPSBaud);

  // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  // uncomment this line to turn on only the "minimum recommended" data for high update rates!
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
  // uncomment this line to turn on all the available data - for 9600 baud you'll want 1 Hz rate
  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_ALLDATA);

  // Set the update rate
  // 1 Hz update rate
  //GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
  // 5 Hz update rate- for 9600 baud you'll have to set the output to RMC or RMCGGA only (see above)
  //GPS.sendCommand(PMTK_SET_NMEA_UPDATE_5HZ);
  // 10 Hz update rate - for 9600 baud you'll have to set the output to RMC only (see above)
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_10HZ);
}

void loop()
{
  if (digitalRead(_btnPin) == LOW)
  {
    if (_speedUnit != milesPerHour)
    {
      _speedUnit = milesPerHour;
      _unitsChanged = true;
    }

    //_displayCounter = false;
  }
  else
  {
    if (_speedUnit != kilometersPerHour)
    {
      _speedUnit = kilometersPerHour;
      _unitsChanged = true;
    }

    //_displayCounter = true;
  }

  if (_unitsChanged)
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

    _unitsChanged = false;
  }

  //Init
  _speedValue = 0;

  if (_displayCounter)
  {
    _speedValue = _counter;

    _counter += 1;

    if (_counter > 999)
      _counter = 0;

    delay(250);
  }
  else
  {
    char c = GPS.read();

    if (GPS.newNMEAreceived())
    {
      if (!GPS.parse(GPS.lastNMEA()))
      {
        return;
      }
    }

    float knots = GPS.speed;
    
    bool isValid = knots > 0.0;
    bool isUpdated = true; //knots != _lastKnots;

    if (isValid && isUpdated)
    {
      if (_speedUnit == milesPerHour)
      {
        _speedValue = (int)(knots * mphMultiplier);
      }
      else
      {
        _speedValue = (int)(knots * kphMultiplier);
      }

      _lastKnots = knots;
    }
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
