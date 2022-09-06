#include "ViewBattery.h"

ViewBattery::ViewBattery(Adafruit_SSD1306 Display){
  //todo über constukto setzen
  screenPosX = 90;
  totalWidth = 32;
  lastStatus = 0;
  blinkCounter = 0;
  display = Display;
}
ViewBattery::~ViewBattery(){
}

void ViewBattery::setStatus(int currentStatus) {
  if(currentStatus == lastStatus) {
    if(currentStatus == 0) {
      Serial.print("Bat empty. Couter at ");
      Serial.println(blinkCounter);
      if(blinkCounter==100) { // falls 200 erreicht, ausblenden
        Serial.println("HIDE");
        hideBattery();
        blinkCounter = 0; // zähler zurücksetzen
      } else {
        if(blinkCounter==50) { // falls 100 erreicht, einblenden
          Serial.println("SHOW");
          showBattery();
        } 
        Serial.println("do nothing");
        blinkCounter++; // und weiter hochzählen
      }
    }
  } else {
    if(lastStatus == 0) {
      showBattery();
    }
    display.fillRoundRect(screenPosX+1,9,totalWidth-2,45,totalWidth/10,SSD1306_BLACK);;
    int bar;
    int barCount;

    //barCount=(currentStatus+19)/20; // how many bars? 100%-81% = 5, 80%-61% = 4, 60% - 41% = 3, 40% - 21% = 2, 20% - 1% = 1, below 0% = 0;
    barCount = (currentStatus+10)/20; // how many bars? 100%-90% = 5, 89%-70% = 4, 69% - 50% = 3, 49% - 30% = 2, 29% - 10% = 1, below 10% = 0;

    for(bar=0;bar<barCount;bar++) {
      display.fillRect(screenPosX+3, 46-(bar*9), totalWidth-6, 8, SSD1306_WHITE); // draw rectancles of 8px height, move 9px to top
    }

    // TODO positioniere % angaben mittig
    /*String statusPercent = currentStatus;
    int16_t x1, y1;
    uint16_t w, h;
    display.getTextBounds(currentStatus, 0, 0, &x1, &y1, &w, &h);*/
    display.fillRect(screenPosX,SCREEN_HEIGHT-7,totalWidth,8,SSD1306_BLACK);
    display.setCursor(screenPosX+(totalWidth/2)-7,SCREEN_HEIGHT-7);
    display.print(currentStatus);
    display.print("%");
    // END TODO

    lastStatus = currentStatus ;
    display.display();
  }
}

void ViewBattery::isEmpty() { // 
  if(lastStatus != 0) {
    setStatus(0);
  }
  if(counter==200) { // falls 200 erreicht, ausblenden
    hideBattery();
    counter = 0; // zähler zurücksetzen
  } else {
    if(counter==100) { // falls 100 erreicht, einblenden
      showBattery();
    } 
    counter++; // und weiter hochzählen
  }
}


void ViewBattery::showBattery() {
  drawBattery(true);
}


void ViewBattery::hideBattery() {
  drawBattery(false);
}

void ViewBattery::drawBattery(bool drawMode) {
  display.drawRoundRect(screenPosX,8,totalWidth,48,totalWidth/10,drawMode); // draw Battery shape
  display.fillRect(screenPosX+(totalWidth/4),6,totalWidth/2,2,drawMode); // button at the top
  display.display();
}


