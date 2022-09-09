#include <Arduino.h>
#include "ScrollClass.h"

ScrollClass::ScrollClass(String _string, int _posX, int _posY, int _width) {
	Originstring = _string;
	PosX = _posX;
	PosY = _posY;
	Width = _width;
	StartMillis = millis();
	InitialDelay = 100;
	ScrollDelay = 50;
}
void ScrollClass::SetDelay(int _delay) {
	ScrollDelay = _delay;
}
void ScrollClass::SetInitialDelay(int _delay) {
	InitialDelay = _delay;
}
void ScrollClass::Refresh() {

}

// Variablen
String Originstring; // also includes space / connector 
int PosX;
int PosY;
int Width;

int ColumnOffset;
int CharPosition;
