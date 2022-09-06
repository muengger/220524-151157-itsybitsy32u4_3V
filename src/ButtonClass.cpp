#include "ButtonClass.h"
#include <Arduino.h>

Button::Button(int usePin) {
    buttonPin = usePin;
    pressTime = 0;
  }

buttonStatus Button::getStatus() {
    return currentStatus;
  }

unsigned long Button::getDuration() {
    return pressTime;
  }

 void Button::refresh() {
    bool pinStatus = digitalRead(buttonPin);
    if(pinStatus == 0) { // pressed
      buttonIsPressed();
    } else {
      buttonNotPressed();
    }
  }
void Button::buttonIsPressed() {
    if(ButtonOnPressMillis == 0) { // not pressed before
      currentStatus = buttonStatus::ON_PRESS;
      //Serial.println("ButtonIsPressed: ON PRESS");
      ButtonOnPressMillis = millis();
    } else { // already pressed before
      currentStatus = buttonStatus::WHILE_PRESSED;
      //Serial.println("ButtonIsPressed: WHILE PRESSED ------------------");
    }
    pressTime = millis() - ButtonOnPressMillis;
  }

void Button::buttonNotPressed() {
    if(currentStatus == buttonStatus::ON_PRESS || currentStatus == buttonStatus::WHILE_PRESSED) { // was pressed before
      currentStatus = buttonStatus::ON_RELEASE;
    } else { // not pressed before
      currentStatus = buttonStatus::WHILE_UNPRESSED;
    }
    ButtonOnPressMillis = 0;
    pressTime = 0;
  }

bool Button::isLongPress() {
    if(currentStatus == buttonStatus::WHILE_PRESSED) {
      if(pressTime > ButtonLongPressMillis) {
        return true;
      } else {
          // button not yet pressed LongPressMillis
        return false;
      }
    } else {
      // button is not pressed
      return false;
    }
  }

bool Button::onPress() {
    if(currentStatus == buttonStatus::ON_PRESS) {
      return true;
    } else {
      return false;
    }
  }

bool Button::onRelease() {
    if(currentStatus == buttonStatus::ON_RELEASE) {
      return true;
    } else {
      return false;
    }
  }

