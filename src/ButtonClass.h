enum buttonStatus { ON_PRESS, WHILE_PRESSED, ON_RELEASE, WHILE_UNPRESSED};

class Button {
  int buttonPin;
  buttonStatus currentStatus;
  int pressCounter;
  int ButtonLongPressMinimalValue = 30;

  public:Button(int usePin) {
    this->buttonPin = usePin;
    this->pressCounter = 0;
  }

  public:buttonStatus getStatus() {
    return this->currentStatus;
  }

  private:int getDuration() {
    return this->pressCounter;
  }

  public:void refresh() {
    bool pinStatus = digitalRead(this->buttonPin);
    if(pinStatus == 0) { // pressed
      this->buttonIsPressed();
    } else {
      this->buttonNotPressed();
    }
  }
  private:void buttonIsPressed() {
    if(this->pressCounter == 0) { // not pressed before
      this->currentStatus = buttonStatus::ON_PRESS;
    } else { // already pressed before
      this->currentStatus = buttonStatus::WHILE_PRESSED;
    }  
    this->pressCounter++;
  }

  private:void buttonNotPressed() {
    if(this->currentStatus == buttonStatus::ON_PRESS || this->currentStatus == buttonStatus::WHILE_PRESSED) { // was pressed before
      this->currentStatus = buttonStatus::ON_RELEASE;
    } else { // not pressed before
      this->currentStatus = buttonStatus::WHILE_UNPRESSED;
    }
    this->pressCounter = 0;
  }

  public:bool isLongPress() {
    if(this->pressCounter > this->ButtonLongPressMinimalValue) {
      return true;
    } else {
      return false;
    }
  }

  public:bool onPress() {
    if(this->currentStatus == buttonStatus::ON_PRESS) {
      return true;
    } else {
      return false;
    }
  }

  public:bool onRelease() {
    if(this->currentStatus == buttonStatus::ON_RELEASE) {
      return true;
    } else {
      return false;
    }
  }

};