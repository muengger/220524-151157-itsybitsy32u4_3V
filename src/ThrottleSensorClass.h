class ThrottleSensorClass {
  public:
    ThrottleSensorClass(int _minValue, int _stopValue, int _stopTollerance, int _maxValue);
    ~ThrottleSensorClass();

    signed int getSpeedPromille();
    bool isForward();

    void setValue(int _currentValue);

  private:
    int LowestValue;
    int StopValue;
    int StopTollerance;
    int LastBackwardValue; // Calculated from StopValue & StopTollerance
    int LastForwardValue; // Calculated from StopValue & StopTollerance
    int HighestValue;
    
    signed int CurrentSpeedPromille;
    bool isForwardFlag;

    int lastInputValue;
};