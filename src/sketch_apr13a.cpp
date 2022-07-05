// TODO DisplayLayoutManager -> Title, Content, Footer, Tooltip (PopUp). Zu nutzen für fixe Hintergründe (templates)
// TODO aus DisplayLayoutManager -> SaveScreen / Restore Screen
// TODO InteractionManager -> welche Buttons stehen an dieser Stelle zur Verfügung? Welche Aktionen lösen diese aus? -> wird dies genutzt, befindet sich der Script in einem Loop (Fahren muss angehalten sein), damit nur diese Werte abgefangen werden können.
// TODO TextInput (aus InteractionManager) -> string einlesen, mit HOCH und RUNTER um Buchstaben zu wählen oder in der 2. Version sogar einem eigenblendetem Alphabet (A-Z1-0 welches durchgescrollt werden kann), muss auch den Screen vor Anzeige eines interaktiven Elements speichern. Sobald Wert ausgelesen und  zurückgegeben, kann der ursprüngliche Screen wiederhergestellt werden
// TODO ValueInput (aus InteractionManager) -> siehe TextInput
// TODO DynamicValue Class -> nur innert eines gleichen loops verwendbar. Muss vorher instantiert werden. Merkt sich die CursorPosition, akzeptiert einen (Initial)Wert zur Ausgabe. Speichert den Wert und sobald ein neuer kommt, wird erst anhand Position und altem Wert, der alte überschrieben und der neue geschrieben
// TODO ASCI Symbole zu finden unter https://forums.adafruit.com/viewtopic.php?f=8&t=51999

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

enum class Mode { Driving, ConfirmAge, ManageSettings };
// the setup function runs once when you press reset or power the board

//PINS
int PinTrottleSensor = 27; //D9,A9 //Poti für Geschwindigkeit
int PinODRVSerialRX = 0;
int PinODRVSerialTX = 1;
int PinLed = 13;
int PinButtonUp = 23; // Button OBEN
int PinButtonDown = 14; // Button UNTEN
int PinButtonRight = 15; // Button  RECHTS
int PinButtonLeft = 16; // Button LINKS
int PinButtonMiddle = 22; // Button Mitte
int PinButtonOn = 11; // Button ON ??? ganz unten???
int PinHoldPower = 10;
int PinI2CSCL = 3;
int PinI2CSDA = 2;

Button ButtonLeft = Button(PinButtonLeft);
Button ButtonMiddle = Button(PinButtonMiddle);

// Global ENUMs
Mode currentMode = Mode::Driving;
Mode lastMode = Mode::Driving;

// Global Variables
int TrottleSensorVal = 0; // gemessener Wert des Geschwindigkeitsreglers
float Torque = 0; // Drehmoment
float BusVoltage; // Spannung der Batterie

// int WheelDiameter = 160; // Raddurchmesser in mm. Anhand Umdrehung könnte so das Tempo berechnet werden. // TODO via Konfig änderbar
// float BatVoltageMax = 33 // Maximum Batteriespannung -> 100% geladen // TODO via Konfig änderbar
// float BatVoltageWarn = 25 // unter dieser Spannung wird gewarnt // TODO via Konfig änderbar
// float BatVoltageMin = 23 // Minimale Batteriespannung -> 0 % -> muss abstellen // TODO via Konfig änderbar

// int ThrottleMax = 955; // Maximaler Wert für Temposensor -> volles Tempo vorwärts // TODO via Konfi änderbar
// int ThrottleStop = 340; // Temposensorwert für Stillstand // TODO via Konfi änderbar
// int ThrottleMin = 0; // Maximaler Wert für Temposensor -> volles Tempor rückwärts // TODO via Konfi änderbar

// int MaxKMHForward = 20; // Anhand Durchmesser und Umdrehungen / Sekunde berechnete Maximalgeschwindigkeit vorwärts // TODO via Konfi änderbar
// int MaxKMHBackward = 5; // Anhand Durchmesser und Umdrehungen / Sekunde berechnete Maximalgeschwindigkeit rückwärts // TODO via Konfi änderbar


#define MAXSENSORVAL 955 // Maximalwert des Geschwindigkeitsreglers
#define MINSENSORVAL 0 // Minimalwert des Geschwindigkeitsreglers
#define MIDDLESENSORVAL 340 // Mittelwert des Geschwindigkeitsreglers. Alles darüber ist vorwärts fahren, alles darunter ist rückwärts fahren
#define HYSTSENSORVAL 10
#define MAXTORQUE 3 // maximaler Drehmoment

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

HardwareSerial& odrive_serial = Serial1;
// ODrive object
ODriveArduino odrive(odrive_serial);

// Printing with stream operator helper functions
template<class T> inline Print& operator <<(Print &obj,     T arg) { obj.print(arg);    return obj; }
template<>        inline Print& operator <<(Print &obj, float arg) { obj.print(arg, 4); return obj; }

int SerialRead(char *Buff,int Size );

int ReadTrottleSensor();
void UpdateTrottle();
void UpdateBusVoltage();
void ManagePowerSwitch();

void CheckMode();
void Driving();
void ConfirmAge();

void waitButtonPress();

void serial_flush() ;

/*
class Debug {

  void SerialOutput(String text) {
    //
  }

  void WaitButtonPress(Button selectedButton) {
    //
  }
};
*/

/* class CustomSetting { 
  public:
    String _Description;
    int _inputValue;
    double _factorBasic;
    
  public:
    void CustomSetting(int bindedVariable, String settingDescription, double factor) {
      _Description = settingDescription;
      _inputValue = bindedVariable;
    }

    void IncTrigger(int multiplicator) {
      _inputValue = _inputValue+(factor*multiplicator)
      // TODO: check, dass maximalwert nicht überschritten wird
    }

    void DecTrigger() {
      _inputValue = _inputValue - (factor*multiplicator)
      // TODO: check, dass minimalwert nicht unterschritten wird
    }
};*/


/*class ViewBattery {
  static uint16_t counter;
  static int lastStatus; // 0 to 100%
  static int criticalStatus; // values below this will provoque an alert!

  public:
    static int screenPosX;
    static int totalWidth;

  public:
    static void setStatus(int currentStatus) {
      if(currentStatus == lastStatus) {
        return;
      } else {
        display.fillRect(screenPosX+2,10,totalWidth-2,44,0); // fill battery with black
        int bar;
        int barCount;

        //barCount=(currentStatus+19)/20; // how many bars? 100%-81% = 5, 80%-61% = 4, 60% - 41% = 3, 40% - 21% = 2, 20% - 1% = 1, below 0% = 0;
        barCount = (currentStatus+10)/20; // how many bars? 100%-90% = 5, 89%-70% = 4, 69% - 50% = 3, 49% - 30% = 2, 29% - 10% = 1, below 10% = 0;
        for(bar=0;bar<=barCount;bar++) {
          display.fillRect(screenPosX+2, 53-(bar*9), totalWidth-2, 8, SSD1306_WHITE); // draw rectancles of 8px height, move 9px to top
        }
        lastStatus = currentStatus ;
      }
    };

    static void isEmpty() { // 
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

  private:
    static void showBattery() {
      drawBattery(false);
    };

    static void hideBattery() {
      drawBattery(true);
    };

    static void drawBattery(bool drawMode) {
      if(drawMode) {
        display.drawRoundRect(screenPosX,8,totalWidth,46,2,drawMode); // draw Battery shape
        display.fillRect(screenPosX,6,totalWidth/2,2,drawMode); // button at the top
      }
    }
};
int ViewBattery::screenPosX = 102;
int ViewBattery::totalWidth = 20;
*/


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

  odrive_serial.begin(57600);
  Serial1.setTimeout(4);
  requested_state = AXIS_STATE_CLOSED_LOOP_CONTROL;
  //odrive.runState(motornum, requested_state, false /*don't wait*/); // comment as long drive is not connected!
  display.clearDisplay();
  display.display();
}

void loop() {
  static uint8_t toggle=0;
  static unsigned long timeOld = 0;
  unsigned long timeNew = millis();
  if((timeOld+100) < timeNew){ //Run with 10Hz // TODO move this value to a global variable, so every loop can use this value... e.g. the Battery-Object which start to BLINK every 1sec if voltage is near to critical value
    timeOld = timeNew;
    CheckMode();
    digitalWrite(PinLed,toggle);
    toggle ^= 1;
  }
}

int SerialRead(String * Buff,int Size ){

 *Buff = Serial1.readString();
 return Buff->length();
}

void serial_flush() {
  while (Serial1.available()) Serial1.read();
}

int ReadTrottleSensor() {
  int _TrottleSensorVal;
  _TrottleSensorVal = analogRead(PinTrottleSensor);
  TrottleSensorVal = _TrottleSensorVal;
  return _TrottleSensorVal;
}

void UpdateTrottle(){ // TODO Umbauen. Soll einen % Wert vom Maximal möglichen Speed vor- oder rückwärts als Parameter erhalten
  float temp = TrottleSensorVal;
  Torque =0;
  if(TrottleSensorVal > MIDDLESENSORVAL + HYSTSENSORVAL){
    Torque = ((temp - (MIDDLESENSORVAL + HYSTSENSORVAL))*MAXTORQUE)/(MAXSENSORVAL -(MIDDLESENSORVAL + HYSTSENSORVAL));
  }else if (TrottleSensorVal < MIDDLESENSORVAL - HYSTSENSORVAL){
    Torque = ((temp/(MIDDLESENSORVAL - HYSTSENSORVAL))-1)*MAXTORQUE;
  }
  String command = "r axis0.controller.input_torque = ";
  command += Torque;
  command += "\n";
  Serial1.write(command.c_str());
}
// TODO neue Funktion für das Auslesen des Wertes. 


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

void waitButtonPress() {
  Serial.print("DEBUG\nWaiting left Button...");
  do {
    ButtonLeft.refresh();
    if(ButtonLeft.onPress()) {
      break;
    } else {
      delay(100);
    }
  }
  while(true);
  Serial.println("OK");
  delay(100);
}
   

void Driving() { // Driving
  double _oldThrottleVal = ReadTrottleSensor();
  double _currentThrottleVal;
  double speedInPercent;

  double needleLengh = 40; // aka Radius aka Hypothenuse
  int16_t x1 = 64-4;
  int16_t y1 = 60;
  int16_t x2 = 64+4;
  int16_t y2 = 60;
  int16_t x3, y3;
  double x3tmp, y3tmp;

  UpdateBusVoltage();
  ManagePowerSwitch();
  
  do{
    if(digitalRead(PinButtonMiddle) == LOW) {
      Serial.println("ButtonMiddle was pressed - set Mode to ModeTwo");
      lastMode = Mode::Driving;
      currentMode = Mode::ConfirmAge;
      break;
    }

    _currentThrottleVal = ReadTrottleSensor();
    if(_currentThrottleVal == _oldThrottleVal) {
    } else {
      Serial.println("Speed changed");
      if(_currentThrottleVal>MIDDLESENSORVAL) {
        // forwärts fahren
      } else {
        if(_currentThrottleVal<MIDDLESENSORVAL) {
          // rückwärts fahren
        } else {
          // stillstand
        }
        
      }
      
      // TODO -> lagere ThrottleSensor aus. Soll zurück geben ob VOR- oder RÜCKwärts und wievie % vom Schub

      if(_currentThrottleVal <= MAXSENSORVAL) {
        speedInPercent = _currentThrottleVal / MAXSENSORVAL;
      }

      // undraw old triangle
      display.fillTriangle(x1, y1, x2, y2, x3, y3, SSD1306_BLACK);
      
      double angle = 90 * speedInPercent;
      double bogenmass = (angle / 180)  * M_PI;

      /*x3 = 64-(cos(bogenmass) * needleLengh);
      y3 = 60-(sin(bogenmass) * needleLengh);*/

      x3tmp = (cos(bogenmass) * needleLengh);
      y3tmp = (sin(bogenmass) * needleLengh);

      x3 = 64 - x3tmp;
      y3 = 60 - y3tmp;

      // draw new triangle
      display.fillTriangle(x1, y1, x2, y2, x3, y3, SSD1306_WHITE);
      display.display();

      _oldThrottleVal = _currentThrottleVal;
    }

    UpdateTrottle();
  }
  while(true);
}

void ConfirmAge() { //Ask for Racer Confirmation
  Serial.println("Now in ModeTwo.");
  switch(lastMode) { // Sobald dieser Teil ausgeführt wird, geht nix mehr
    case Mode::Driving:
      break;
    case Mode::ConfirmAge:
      break;
    case Mode::ManageSettings:
      break;
    default:
      break;
  }

/*  display.clearDisplay();
  display.setCursor(0,0);
  display.write("Antwort eingeben");
  display.display();

  int trial;
  for(trial = 0; trial <= 3; trial++) {
    display.fillRect(0,20,SCREEN_WIDTH,8,0);
    display.setCursor(0,30);

    // Generiere Zufallsrechnung
    int termType = rand() % 3;
    int numberA = rand() % 20;
    int numberB = rand() % 20;
    String term = "";
    int solution, userInput;

    switch(termType) {
      case 0:
        Serial.print("Add:");
        Serial.print(numberA);
        term += numberA;
        Serial.print(" + ");
        term += " + ";
        Serial.print(numberB);
        term += numberB;
        term += " = ";
        solution = numberA+numberB;
        Serial.println(solution);
        break;
      case 1:
        Serial.println("Sub");
        if(numberA<numberB) {
          Serial.print(numberB);
          term += numberB;
          Serial.print(" - ");
          term += " - ";
          Serial.print(numberA);
          term += numberA;
          solution = numberB-numberA;
        } else {
          Serial.print(numberA);
          term += numberA;
          Serial.print(" - ");
          term += " - ";
          Serial.print(numberB);
          term += numberB;
          term += " = ";
          solution = numberA-numberB;
        }
        Serial.print(" = ");
        Serial.println(solution);
        break;
      case 3:
        Serial.println("Mult");
        Serial.print(numberA);
        term += numberA;
        Serial.print(" * ");
        term += " * ";
        Serial.print(numberB);
        term += numberB;
        term += " = ";
        solution = numberA*numberB;
        Serial.print(" = ");
        Serial.println(solution);
        break;
      default:
        break;
    }
    display.write(term);
    do { // Buttoneingaben abfangen, sobald fertig -> break;
      // userInput einlesen
    }
    while(true); // wiederholt Buttoneingaben sonst unendlich

    if(userInput == solution) {
        // userProfile auf Raser setzen
        // break; // Schleife abbrechen
    } else {

    }
  }
  // ModusOne setzen
  */

  do{
    if(digitalRead(PinButtonMiddle) == LOW) {
      Serial.println("ButtonMiddle was pressed - set Mode to ModeTree");
      lastMode = Mode::ConfirmAge;
      currentMode = Mode::ManageSettings;
      break;
    }
    delay(1000);
  }
  while(true);
}

void ManageSettings() {
 switch(lastMode) { // Sobald dieser Teil ausgeführt wird, geht nix mehr
    case Mode::Driving:
      break;
    case Mode::ConfirmAge:
      break;
    case Mode::ManageSettings:
      break;
    default:
      break;
  }

  do{
    Serial.println("Check Button Middle");
    if(digitalRead(PinButtonMiddle) == LOW) {
      Serial.println("ButtonMiddle was pressed - set Mode to Driving");
      lastMode = Mode::ManageSettings;
      currentMode = Mode::Driving;
      break;
    }
    delay(1000);
  }
  while(true);

  // Config Menu

}


void CheckMode(){
  switch(currentMode) {
    case Mode::Driving:
      Driving();
      break;
    case Mode::ConfirmAge:
      ConfirmAge();
      break;
    case Mode::ManageSettings:
      ManageSettings();
      break;
  };
}