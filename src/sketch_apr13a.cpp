// TODO DisplayLayoutManager -> Title, Content, Footer, Tooltip (PopUp). Zu nutzen für fixe Hintergründe (templates)
// TODO aus DisplayLayoutManager -> SaveScreen / Restore Screen
// TODO InteractionManager -> welche Buttons stehen an dieser Stelle zur Verfügung? Welche Aktionen lösen diese aus? -> wird dies genutzt, befindet sich der Script in einem Loop (Fahren muss angehalten sein), damit nur diese Werte abgefangen werden können.
// TODO TextInput (aus InteractionManager) -> string einlesen, mit HOCH und RUNTER um Buchstaben zu wählen oder in der 2. Version sogar einem eigenblendetem Alphabet (A-Z1-0 welches durchgescrollt werden kann), muss auch den Screen vor Anzeige eines interaktiven Elements speichern. Sobald Wert ausgelesen und  zurückgegeben, kann der ursprüngliche Screen wiederhergestellt werden
// TODO ValueInput (aus InteractionManager) -> siehe TextInput
// TODO DynamicValue Class -> nur innert eines gleichen loops verwendbar. Muss vorher instantiert werden. Merkt sich die CursorPosition, akzeptiert einen (Initial)Wert zur Ausgabe. Speichert den Wert und sobald ein neuer kommt, wird erst anhand Position und altem Wert, der alte überschrieben und der neue geschrieben
// TODO ASCI Symbole zu finden unter https://forums.adafruit.com/viewtopic.php?f=8&t=51999

// --- Config ---
#include "config/Screen.h"
#include "config/PinNumbers.h" // Hardware Arduino Pin Numbers
#include "config/BatVoltageLimits.h" // Battery Voltage Limits -> move to EEPROM
#include "config/ThrottleSensorValues.h" // Limits of Throttle Sensor
#include "config/Wheel.h" // Wheel Mesurement
#include "config/Engine.h" // Torque etc.
// ---

// --- Includes ---
#include <Globals.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_GrayOLED.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <gfxfont.h>
#include <Adafruit_SSD1306.h>
#include <splash.h>

#include <ODriveArduino.h>
#include <ODriveEnums.h>

#include <HardwareSerial.h>
#include <ButtonClass.h>
#include "ThrottleSensorClass.h"
#include "TermHelperClass.h"
#include "ViewBattery.h"

#include "SettingsManagerClass.h"
#include "ScrollClass.h"

// ---

// --- Define ENUMS ---
enum class Mode { Driving, Statistics, ConfirmAge_Instructions, ConfirmAge_Trial1, ConfirmAge_Trial2, ConfirmAge_Trial3, ConfirmAge_FailedRetry, ConfirmAge_FailedExit, ConfirmAge_Success, ManageSettings };
enum class Driver { Kid, Teen, Adult };

// -- Initialize Variables
// Menu
Mode currentMode = Mode::Driving;
Mode lastMode;
Driver currentDriver = Driver::Kid;
Driver lastDriver;

unsigned long ConfirmAgeStartTime = 0;
int ConfirmAgeAnswer = 0;
// ---

// -- Initialize Objects
Button ButtonLeft = Button(PinButtonLeft);
Button ButtonRight = Button(PinButtonRight);
Button ButtonMiddle = Button(PinButtonMiddle);
Button ButtonUp = Button(PinButtonUp);
Button ButtonDown = Button(PinButtonDown);
ThrottleSensorClass ThrottleSensor = ThrottleSensorClass(ThrottleMinValue, ThrottleStopValue, ThrottleStopTolleranceRange, ThrottleMaxValue);
SettingsManagerClass SettingsManager = SettingsManagerClass(); // original pos for global usage

//TermHelper TermHelper = TermHelper();
// ---

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
ViewBattery BatteryView(display);
HardwareSerial& odrive_serial = Serial1;
// ODrive object
ODriveArduino odrive(odrive_serial);


// Printing with stream operator helper functions
template<class T> inline Print& operator <<(Print &obj,     T arg) { obj.print(arg);    return obj; }
template<>        inline Print& operator <<(Print &obj, float arg) { obj.print(arg, 4); return obj; }

int SerialRead(char *Buff,int Size );

void ReadTrottleSensor();
void UpdateTrottle();
void UpdateBusVoltage();
void ManagePowerSwitch();

void CheckMode();
void Driving();
void ConfirmAge_Instructions();
void ConfirmAge_Trial1();
void ConfirmAge_Trial2();
void ConfirmAge_Trial3();

void showBattery();
void SensorValScreen();

//double getSpeedKmh();

void serial_flush() ;

/*
void IconDrawer(int id, uint16_t xPos, uint16_t yPos) {
  // width and height 18px

  // Throttle
  display.drawRoundRect(xPos+2, yPos+12,14,6,3,SSD1306_WHITE);
  display.fillRect(xPos,)
}*/


void SensorValScreen(){
    String sDisplay;
    sDisplay += "\nSpeedPerc.:=";
    sDisplay += ThrottleSensor.getSpeedPromille();
    sDisplay += "\nTorqVal:=";
    sDisplay += Torque;
    sDisplay += "\nBattV:=";
    sDisplay += BusVoltage;
    display.clearDisplay();
    display.setCursor(0, 0);     // Start at top-left corner
    display.print(sDisplay.c_str());
    display.display();
}

void setup() {
  int motornum = 0;
  int requested_state; 
  pinMode(PinLed, OUTPUT);
  pinMode(PinHoldPower,OUTPUT);
  pinMode(PinButtonOn,INPUT);
  pinMode(PinButtonUp,INPUT_PULLUP);
  pinMode(PinButtonDown,INPUT_PULLUP);
  pinMode(PinButtonRight,INPUT_PULLUP);
  pinMode(PinButtonLeft,INPUT_PULLUP);
  pinMode(PinButtonMiddle,INPUT_PULLUP);

  Serial.begin(9600);
  
  digitalWrite(PinHoldPower,HIGH);
  delay(200);
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  display.display();
  display.clearDisplay();
  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font
  display.print("High\nin the Roller Chair\nTurbo Controller");
  display.display();
  delay(1500);

  //odrive_serial.begin(57600);
  //Serial1.setTimeout(4);
  //requested_state = AXIS_STATE_CLOSED_LOOP_CONTROL;
  //odrive.runState(motornum, requested_state, false /*don't wait*/); // comment as long drive is not connected!
  //display.clearDisplay();
  //display.display();
}

void loop() {
  //Serial.println("Loop Begin");
  static uint8_t toggle=0;
  static unsigned long timeOld = 0;
  unsigned long timeNew = millis();

  if((timeOld+100) < timeNew){ //Run with 10Hz // TODO move this value to a global variable, so every loop can use this value... e.g. the Battery-Object which start to BLINK every 1sec if voltage is near to critical value
    //Serial.println("Loop: Running Code");
    timeOld = timeNew;

    // read inputs / sensors ---
    ButtonUp.refresh();
    ButtonDown.refresh();
    ButtonLeft.refresh();
    ButtonRight.refresh();
    ButtonMiddle.refresh();
    ReadTrottleSensor();
    // ---
    
    //UpdateBusVoltage(); //TOFIX currently stops script of running.


    ManagePowerSwitch();  
    CheckMode();
    //SensorValScreen();

    digitalWrite(PinLed,toggle);
    toggle ^= 1;
  } else {
    //Serial.println("Loop: Skip Code");
  }
  //Serial.println("Loop End");
}

int SerialRead(String * Buff,int Size ){

 *Buff = Serial1.readString();
 return Buff->length();
}

void serial_flush() {
  while (Serial1.available()) Serial1.read();
}

void ReadTrottleSensor() { // TODO maybe a job for ThrottleSensor?
  int _TrottleSensorVal;
  _TrottleSensorVal = analogRead(PinTrottleSensor);
  ThrottleSensor.setValue(_TrottleSensorVal);
}

void UpdateTrottle(){ // TODO now we can't change the max torque or the current speed.... should be passed as parameter
  Torque =0;
  if(ThrottleSensor.isForward() == true){
    Torque  = ThrottleSensor.getSpeedPromille()/(double)1000*MaxTorque;
  }else if (ThrottleSensor.isForward() == false){
    Torque = (ThrottleSensor.getSpeedPromille()/(double)1000)-1*MaxTorque;
  }
  String command = "r axis0.controller.input_torque = ";
  command += Torque;
  command += "\n";
  Serial1.write(command.c_str());
}

void UpdateBusVoltage(){
  String Buffer;
  serial_flush();
  Serial1.write("r vbus_voltage\n");
  Serial1.flush();
  BusVoltage = Serial1.parseFloat();
}

void ManagePowerSwitch(){
  static uint32_t counter = 0;
  if(digitalRead(PinButtonOn)==1){
    counter++;
  }else{
    counter = 0;
  }

  if(counter > 30){
    digitalWrite(PinHoldPower,0);
  }
}

void Driving() {
  //Serial.println("Driving: Begin");

  // Initial Background
  if(lastMode != Mode::Driving) {
    Serial.println("Driving: First Call");
    // TODO Reset Screen & Draw Backgroud
    display.clearDisplay();
    //BatteryView.setStatus(0);
    //BatteryView.showBattery();
    lastMode = Mode::Driving;
    if(lastDriver != currentDriver) {
      // RE READ User SETTINGS
      //MaxTorque = ??? kalkulieren aus max tempo des aktuellen users, raddurchmesser etc
      lastDriver = currentDriver;
    } else {
      // Still same driver? So nothing to do
    }
  } else {
    // We're still in the Driving-Mode
    //Serial.println("Driving: continue");
  }

      
  // ************** code needed for throttle gauge ******************* //
  /*double _oldThrottleVal = ReadTrottleSensor();
  double _currentThrottleVal;
  double speedInPercent;

  double needleLengh = 40; // aka Radius aka Hypothenuse
  int16_t x1 = 64-4;
  int16_t y1 = 60;
  int16_t x2 = 64+4;
  int16_t y2 = 60;
  int16_t x3, y3;
  double x3tmp, y3tmp;
*/
    if(ButtonMiddle.isLongPress()) {
      Serial.println("ButtonMiddle was pressed - set Mode to ConfirmAge");
      //lastMode = Mode::Driving;
      currentMode = Mode::ConfirmAge_Instructions;
      delay(500);
    }

    
  /*
  do{ 
    
    // TODO detect change of speed
    /*_currentThrottleVal = ReadTrottleSensor();
    if(_currentThrottleVal == _oldThrottleVal) {
    } else {
      Serial.println("Speed changed");
      if(_currentThrottleVal>ThrottleStop) {
        // forwärts fahren
      } else {
        if(_currentThrottleVal<ThrottleStop) {
          // rückwärts fahren
        } else {
          // stillstand
        }
        
      }
      
      // TODO -> lagere ThrottleSensor aus. Soll zurück geben ob VOR- oder RÜCKwärts und wievie % vom Schub

      int _currentThrottleVal = <= ThrottleMax) {
        int = = _currentThrottleVal / ThrottleMax;
      }

      // undraw old triangle
      display.fillTriangle(x1, y1, x2, y2, x3, y3, SSD1306_BLACK);
      
      double angle = 90 * speedInPercent;
      double bogenmass = (angle / 180)  * M_PI;

      x3 = 64 - (cos(bogenmass) * needleLengh);
      y3 = 60 - (sin(bogenmass) * needleLengh);

      // draw new triangle
      display.fillTriangle(x1, y1, x2, y2, x3, y3, SSD1306_WHITE);
      display.display();

      _oldThrottleVal = _currentThrottleVal;
    }*/
    
}

void ConfirmAge_Instructions() {
  if(currentDriver == Driver::Teen) {
    // maybe teens want to re-enter this mode, so we use this to set back to "kid"
    currentDriver = Driver::Kid;
    exit;
  }
  if(lastMode != Mode::ConfirmAge_Instructions) { // init screen
    lastMode = Mode::ConfirmAge_Instructions;
    Serial.println("ConfirmAge: Initial call, show instruction then wait keypress");
    exit;
  } else {
    if(ButtonMiddle.onPress()) { // continue on middlepress
      Serial.println("Continue to Trial1");
      currentMode = Mode::ConfirmAge_Trial1;
      exit;
    } else {
      if(ButtonUp.isLongPress()) { // jump to configuration manager with long-up-press
        Serial.println("Continue to ManagSettings");
        currentMode = Mode::ManageSettings;
        exit;
      }
    }
  }
}

void ConfirmAge_Trial1() {
  if(lastMode != Mode::ConfirmAge_Trial1) { // init screen
    lastMode = Mode::ConfirmAge_Trial1;
    // Clear Screen
    Serial.println("Trial 1: Build fix BG");
    ConfirmAgeAnswer = 0;
    if(TermHelper::generateTerm()) {
      //Serial.print("Term was generated");
    } else {
      //Serial.print("Could not generate Term");
    }
    
    // Show 1. Term
    Serial.print("Show term on screen: ");
    Serial.println(TermHelper::getTerm());
    Serial.print("Solution: ");
    Serial.println(TermHelper::getSolution());
    //TermHelper::generateTerm();
    exit;
  } else {
    if(ButtonUp.onPress()) {
      ConfirmAgeAnswer ++;
      Serial.println(ConfirmAgeAnswer);
    }
    if(ButtonUp.isLongPress()) {
      ConfirmAgeAnswer += 2;
      Serial.println(ConfirmAgeAnswer);
    }
    if(ButtonDown.onPress()) {
      ConfirmAgeAnswer --;
      Serial.println(ConfirmAgeAnswer);
    }
    if(ButtonDown.isLongPress()) {
      ConfirmAgeAnswer -= 2;
      Serial.println(ConfirmAgeAnswer);
    }
    if(ButtonMiddle.onPress()) {
      Serial.println("User confirmed input");
      if(ConfirmAgeAnswer == TermHelper::getSolution()) {
        Serial.println("Answer correct, change to Teen");
        // Show Success-Message
        delay(1000);
        currentDriver = Driver::Teen;
        currentMode = Mode::Driving;
      } else {
        Serial.println("Wrong Answer");
        currentMode = Mode::ConfirmAge_Trial2;
        exit;
      }
    }
  }
}

void ConfirmAge_Trial2() {
  if(lastMode != Mode::ConfirmAge_Trial2) { // init screen
    lastMode = Mode::ConfirmAge_Trial2;
    // Clear Screen
    Serial.println("Trial 2: Build fix BG");
    ConfirmAgeAnswer = 0;
    if(TermHelper::generateTerm()) {
      //Serial.print("Term was generated");
    } else {
      //Serial.print("Could not generate Term");
    }
    
    // Show 2. Term
    Serial.print("Show term on screen: ");
    Serial.println(TermHelper::getTerm());
    Serial.print("Solution: ");
    Serial.println(TermHelper::getSolution());
    //TermHelper::generateTerm();
    exit;
  } else {
    if(ButtonUp.onPress()) {
      ConfirmAgeAnswer ++;
      Serial.println(ConfirmAgeAnswer);
    }
    if(ButtonUp.isLongPress()) {
      ConfirmAgeAnswer += 2;
      Serial.println(ConfirmAgeAnswer);
    }
    if(ButtonDown.onPress()) {
      ConfirmAgeAnswer --;
      Serial.println(ConfirmAgeAnswer);
    }
    if(ButtonDown.isLongPress()) {
      ConfirmAgeAnswer -= 2;
      Serial.println(ConfirmAgeAnswer);
    }
    if(ButtonMiddle.onPress()) {
      Serial.println("User confirmed input");
      if(ConfirmAgeAnswer == TermHelper::getSolution()) {
        Serial.println("Answer correct, change to Teen");
        // Show Success-Message
        delay(1000);
        currentDriver = Driver::Teen;
        currentMode = Mode::Driving;
      } else {
        Serial.println("Wrong Answer");
        currentMode = Mode::ConfirmAge_Trial3;
        exit;
      }
    }
  }
}


void ConfirmAge_Trial3() {
  if(lastMode != Mode::ConfirmAge_Trial3) { // init screen
    lastMode = Mode::ConfirmAge_Trial3;
    // Clear Screen
    Serial.println("Trial 3: Build fix BG");
    ConfirmAgeAnswer = 0;
    if(TermHelper::generateTerm()) {
      //Serial.print("Term was generated");
    } else {
      //Serial.print("Could not generate Term");
    }
    
    // Show 3. Term
    Serial.print("Show term on screen: ");
    Serial.println(TermHelper::getTerm());
    Serial.print("Solution: ");
    Serial.println(TermHelper::getSolution());
    //TermHelper::generateTerm();
    exit;
  } else {
    if(ButtonUp.onPress()) {
      ConfirmAgeAnswer ++;
      Serial.println(ConfirmAgeAnswer);
    }
    if(ButtonUp.isLongPress()) {
      ConfirmAgeAnswer += 2;
      Serial.println(ConfirmAgeAnswer);
    }
    if(ButtonDown.onPress()) {
      ConfirmAgeAnswer --;
      Serial.println(ConfirmAgeAnswer);
    }
    if(ButtonDown.isLongPress()) {
      ConfirmAgeAnswer -= 2;
      Serial.println(ConfirmAgeAnswer);
    }
    if(ButtonMiddle.onPress()) {
      Serial.println("User confirmed input");
      if(ConfirmAgeAnswer == TermHelper::getSolution()) {
        Serial.println("Answer correct, change to Teen");
        // Show Success-Message
        delay(1000);
        currentDriver = Driver::Teen;
        currentMode = Mode::Driving;
      } else {
        Serial.println("Wrong Answer");
        currentMode = Mode::Driving;
        exit;
      }
    }
  }
}


void ManageSettings() {
  if(lastMode != Mode::ManageSettings) {
    // init call
    // draw background

    display.clearDisplay();
    display.drawFastHLine(1,9,SCREEN_WIDTH-2,SSD1306_WHITE);
    display.setCursor(1,0);
    display.print("Einstellung:");
    display.setCursor(84,0);
    display.write(0x11);
    display.setCursor(100,0);
    display.print("/3");
    display.write(0x10);
    display.display();

    lastMode = Mode::ManageSettings;
    delay(1000);
  } else {
    if(SettingsManager.isChanged() == true) {
      // reset and rebuild dynamic values
      display.fillRect(91,0,8,7, SSD1306_BLACK);
      display.fillRect(0,21,128,43, SSD1306_BLACK);
      
      // ID Setting (Top Right)
      display.setCursor(94,0);
      display.print(SettingsManager.getCurrentSettingId()+1);
      
      // ID Middle
      display.setCursor(28,21);
      display.println(SettingsManager.getCurrentName()); // max 16 zeichen
      
      
      // Description
      display.setCursor(28,31);
      display.println(SettingsManager.getCurrentDescription());
      

      // Current Value and Suffix and Status
      display.setCursor(40,50);
      display.setTextSize(2);
      if(SettingsManager.getCurrentValueFromMemory() == SettingsManager.getCurrentValueUsed()) {
        display.write(0x02);
      } else {
        display.write(0x01);
      }
      display.setTextSize(1);
      display.print(" ");
      display.print(SettingsManager.getCurrentValueUsed());
      display.print(" ");
      display.print(SettingsManager.getCurrentUnit());

      display.display();
      SettingsManager.resetChangedFlag();
      
    }

    
    Serial.print("SettingsManager at Setting: ");
    Serial.println(SettingsManager.getCurrentName());
    Serial.print("Current Value at: ");
    Serial.println(SettingsManager.getCurrentValueUsed());
    if(ButtonRight.onPress()) {
      SettingsManager.nextSetting();
      //Serial.println("Pressed up");
      exit;
    }
    if(ButtonLeft.onPress()) {
      SettingsManager.previousSetting();
      //Serial.println("Pressed down");
      exit;
    }
    if(ButtonUp.onPress()) {
      SettingsManager.increaseCurrentValue();
      //Serial.println("Pressed up");
      exit;
    }
    if(ButtonUp.isLongPress()) {
      SettingsManager.increaseCurrentValue(2);
      //Serial.println("Pressed up");
      exit;
    }
    if(ButtonDown.onPress()) {
      SettingsManager.decreaseCurrentValue();
      //Serial.println("Pressed down");
      exit;
    }
    if(ButtonDown.isLongPress()) {
      SettingsManager.decreaseCurrentValue(2);
      //Serial.println("Pressed down");
      exit;
    }
    if(ButtonMiddle.onRelease()) {
      SettingsManager.saveCurrentValueToMemory();
      //Serial.println("should persist");
    }
    if(ButtonMiddle.isLongPress()) {
      currentMode=Mode::Driving;
    }
  }

/*
  //display.drawCircle(14,30,9, SSD1306_WHITE);

  //display.drawBitmap(8, 21, Tempo_bits, 18, 18, SSD1306_WHITE, SSD1306_BLACK);
  // display.drawBitmap(8, 21, Tempo_bits, 18, 18, SSD1306_WHITE); // load icon from icons-folder
  display.display();

  
  display.setCursor(40,47);
  display.write(0x1E);
  display.setCursor(40,53);
  display.write(0x1F);
  
*/
}


void CheckMode(){
  //Serial.println("CheckMode Begin");
  switch(currentMode) {
    case Mode::Driving:
      Driving();
      break;
    case Mode::ConfirmAge_Instructions:
      ConfirmAge_Instructions();
      break;
    case Mode::ConfirmAge_Trial1:
      ConfirmAge_Trial1();
      break;
    case Mode::ConfirmAge_Trial2:
      ConfirmAge_Trial2();
      break;
    case Mode::ConfirmAge_Trial3:
      ConfirmAge_Trial3();
      break;
    case Mode::ConfirmAge_FailedRetry:
      //
      break;
    case Mode::ConfirmAge_FailedExit:
      //
      break;
    case Mode::ConfirmAge_Success:
      //
      break;
    case Mode::ManageSettings:
      ManageSettings();
      break;
    case Mode::Statistics:
      //
      break;
  };
  //Serial.println("Check
}