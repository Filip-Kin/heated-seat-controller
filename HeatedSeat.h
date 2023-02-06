#include <Arduino.h>

#define AUTO_HIGH 22
#define AUTO_LOW 28
#define AUTO_OFF 32
#define MAX_TEMPERATURE 40

class HeatedSeat {
 public:
  HeatedSeat(byte onSwitch, byte autoSwitch, byte relayHigh, byte relayLow, byte tempPin) {
    _onSwitch = onSwitch;
    _autoSwitch = autoSwitch;
    _relayHigh = relayHigh;
    _relayLow = relayLow;
    _tempPin = tempPin;
  }

  float getTemperature() {
    Vo = analogRead(_tempPin);
    R2 = R1 * (1023.0 / (float)Vo - 1.0);
    logR2 = log(R2);
    T = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2));
    T = T - 273.15;
    return T;
  }

  void readTemperature() { 
    // Shift temperature array
    float sum = 0;
    for (int i = 0; i < 4; i++) {
      _tempArray[i] = _tempArray[i + 1];
      sum += _tempArray[i];
    }

    // Get new reading
    _tempArray[4] = getTemperature();
    sum += _tempArray[4];

    // Assign average reading to variable
    _temp = sum / 5.0;
  }

  void runLoop() {
    if (_temp > MAX_TEMPERATURE) {
      // If temperature is past safety cutoff we want to shut it off
      _setting = 0;

    } else if (digitalRead(_autoSwitch) == HIGH) {
      // If switch is set to auto

      Serial.print("Auto " + String(_temp));
      
      if ((_temp < AUTO_HIGH && _setting != 2) || (_temp < AUTO_LOW && _setting == 2)) {
        _setting = 2;
      } else if ((_temp < AUTO_LOW && _setting != 1) || (_temp < AUTO_OFF && _setting == 1)) {
        _setting = 1;
      } else {
        _setting = 0;
      }

    } else if (digitalRead(_onSwitch) == HIGH) {
      // If switch is set to on (toggle through modes)

      if (!btnPressedLastLoop) {
        btnPressedLastLoop = true;
        // This condition and variable will keep the controller from switching through multiple settings if you hold the button down
  
        if (_setting <= 0) {  // If zero then set to high
          _setting = 2;
        } else {
          _setting--;  // Otherwise go down one step
        }
      }
    } else {
      btnPressedLastLoop = false;
    }

    Serial.println("Setting " + String(_setting));
    if (_setting != _lastSetting) {
        digitalWrite(_relayHigh, LOW);
        digitalWrite(_relayLow, LOW);
        delay(10);
        _lastSetting = _setting;
    }
    switch (_setting) {
      case 0:
        digitalWrite(_relayHigh, LOW);
        digitalWrite(_relayLow, LOW);
        break;
      case 1:
        digitalWrite(_relayHigh, LOW);
        digitalWrite(_relayLow, HIGH);
        break;
      case 2:
        digitalWrite(_relayHigh, HIGH);
        digitalWrite(_relayLow, HIGH);
        break;
    }
  }

 private:
  byte _onSwitch;
  byte _autoSwitch;
  byte _relayHigh;
  byte _relayLow;
  byte _tempPin;
  float _temp;
  float _tempArray[5] = {0,0,0,0,0};
  bool btnPressedLastLoop = false;
  bool autoLastLoop = false;
  int _setting = 0;
  int _lastSetting = 0;
  int Vo;
  float R1 = 10000;
  float logR2, R2, T;
  float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;
};
