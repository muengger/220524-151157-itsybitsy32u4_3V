enum buttonStatus { ON_PRESS, WHILE_PRESSED, ON_RELEASE, WHILE_UNPRESSED};

class Button {
  public:
    Button(int usePin);
    buttonStatus getStatus();
    void refresh();
    bool isLongPress();
    bool onPress();
    bool onRelease();
    unsigned long getDuration();
    
  private:
    void buttonNotPressed();
    void buttonIsPressed();
    int buttonPin;
    buttonStatus currentStatus;
    int ButtonLongPressMillis = 1500;
    unsigned long ButtonOnPressMillis;
    unsigned long pressTime;
};