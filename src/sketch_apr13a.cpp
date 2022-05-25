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

// the setup function runs once when you press reset or power the board

//PINS
int PinTrottleSensor = 27; //D9,A9
int PinODRVSerialRX = 0;
int PinODRVSerialTX = 1;
int PinLed = 13;
int PinButtonUp = 23;
int PinButtonDown = 14;
int PinButtonRight = 15;
int PinButtonLeft = 16;
int PinButtonMidle = 22;
int PinButtonOn = 11;
int PinHoldPower = 10;
int PinI2CSCL = 3;
int PinI2CSDA = 2;


int TrottleSensorVal = 0;
float Torque = 0;
float BusVoltage;

#define MAXSENSORVAL 955
#define MINSENSORVAL 0
#define MIDDLESENSORVAL 340
#define HYSTSENSORVAL 10
#define MAXTORQUE 3




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
void UpdateTrottle();
void UpdateBusVoltage();
void ManagePowerSwitch();
void ManageDisplay();
void serial_flush() ;

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
  pinMode(PinButtonMidle,INPUT_PULLUP);

  digitalWrite(PinHoldPower,HIGH);
  delay(200);
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  display.display();
  display.clearDisplay();
  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font
  display.print("High\nim the Roller Chair\nTurbo Controller");
  display.display();
  delay(2000);

  odrive_serial.begin(57600);
  Serial1.setTimeout(4);
  requested_state = AXIS_STATE_CLOSED_LOOP_CONTROL;
  odrive.run_state(motornum, requested_state, false /*don't wait*/);
  
}

// the loop function runs over and over again forever
void loop() {
  static uint8_t toggle=0;
  static unsigned long timeOld = 0;
  unsigned long timeNew = millis();
  if((timeOld+100) < timeNew){ //Run with 10Hz
    timeOld = timeNew;
    
    UpdateBusVoltage();
    ManagePowerSwitch();
    ManageDisplay();

    UpdateTrottle();
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

void UpdateTrottle(){
  TrottleSensorVal = analogRead(PinTrottleSensor);
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

void ManageDisplay(){
  String sDisplay;
  sDisplay += "PotiVal:=";
  sDisplay += TrottleSensorVal;
  sDisplay += "\nTorqVal:=";
  sDisplay += Torque;
  sDisplay += "\nBattV:=";
  sDisplay += BusVoltage;
  display.clearDisplay();
  display.setCursor(0, 0);     // Start at top-left corner
  display.print(sDisplay.c_str());
  display.display();
}