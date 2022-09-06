#include "ThrottleSensor.h"

#include <SPI.h>
#include <Wire.h>
#include <HardwareSerial.h>

ThrottleSensor::ThrottleSensor(int _minValue, int _stopValue, int _stopTollerance, int _maxValue){
  LowestValue = _minValue;
  StopValue = _stopValue;
  StopTollerance = _stopTollerance;
  HighestValue = _maxValue;

  LastBackwardValue = StopValue-(StopTollerance/2);
  LastForwardValue = StopValue+(StopTollerance/2);

  lastInputValue = 0;
}

ThrottleSensor::~ThrottleSensor(){
}

void ThrottleSensor::setValue(int _currentInputValue) {
  if(_currentInputValue == lastInputValue) {
    // nothing to do
  } else {
    // calculateNewValues
    if(_currentInputValue <= LastBackwardValue) { // direction: backward
      if(_currentInputValue < LowestValue) _currentInputValue = LowestValue;
      CurrentSpeedPromille = (LastBackwardValue-LowestValue-(_currentInputValue-LowestValue)+1)*1000/(double)(LastBackwardValue-LowestValue+1);
      isForwardFlag = false;
    } else {
      if(_currentInputValue >= LastForwardValue) { // direction: forward
        if(_currentInputValue > HighestValue) _currentInputValue = HighestValue;
        CurrentSpeedPromille = (_currentInputValue-LastForwardValue+1)*1000/(double)(HighestValue-LastForwardValue+1);
        isForwardFlag = true;
      } else { // should be StopValue or within tollerance

      }
    }
    lastInputValue = _currentInputValue;
  }
}

int ThrottleSensor::getSpeedPromille() {
  return CurrentSpeedPromille;
}

bool ThrottleSensor::isForward() {
  return isForwardFlag;
}