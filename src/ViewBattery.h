#include <Adafruit_SSD1306.h>
#include "config/Screen.h"

class ViewBattery {
  public:
  	
    ViewBattery(Adafruit_SSD1306 display);
    ~ViewBattery();

    int screenPosX;
    int totalWidth;

    void setStatus(int currentStatus);
    void isEmpty();
    void showBattery();
  
  private:
    void hideBattery();
	  void drawBattery(bool drawMode);
    uint16_t counter;
	  int lastStatus; // 0 to 100%
	  int criticalStatus; // values below this will provoque an alert!
	  int blinkCounter;
	  Adafruit_SSD1306 display;
};

