#include "ScrollClass.h"
#include <Arduino.h>

#include <Adafruit_GFX.h>
#include <gfxfont.h>
#include <Adafruit_SSD1306.h>

ScrollClass::ScrollClass() {
}


ScrollClass::ScrollClass(Adafruit_SSD1306 _display, String _string, int _posX, int _posY, int _width) {
	Originstring = _string;
	PosX = _posX;
	PosY = _posY;
	AllowedChars = _width;
	StartMillis = millis();
	InitialDelay = 100;
	ScrollDelay = 100;
	ColumnOffset = 0;
	CharPosition = 0;
	scrollDisplay = _display;
	
	if(Originstring.length() <= AllowedChars) {
		isScrollableFlag = false;
		writeString(PosX, PosY, Originstring);
	} else {
		isScrollableFlag = true;
		writeString(PosX, PosY, Originstring.substring(0,AllowedChars));
	}


	// TODO initialisierung soll Text anzeigen, ev. abgeschnitten, an OriginalPotision
}

void ScrollClass::SetDelay(int _delay) {
	ScrollDelay = _delay;
}

void ScrollClass::SetInitialDelay(int _delay) {
	InitialDelay = _delay;
}

void ScrollClass::Refresh() {
	// TODO soll text direkt verschieben, falls es dies zu tun gibt
	String output;
	int StringLength = Originstring.length();
	int realX = PosX;

	output = Originstring;

	if(StringLength>AllowedChars) {
		
		if(StartMillis + ScrollDelay < millis()) {
			StartMillis = millis();

			scrollDisplay.fillRect(PosX-7, PosY, (AllowedChars*7)+(2*7), 8, SSD1306_BLACK);

			if(ColumnOffset < 6) {
				ColumnOffset++;
			} else {
				Serial.println("next Character");
				ColumnOffset = 1;
				CharPosition++;
				if(CharPosition < StringLength) {
					// nothing to do
				} else {
					CharPosition = 0;
				}

				Serial.print("New CharPos: ");
				Serial.println(CharPosition);	
			}
			if(StringLength - CharPosition > AllowedChars) { 
				// Der String ist gegen hinten noch abgeschnitten
				output = Originstring.substring(CharPosition, CharPosition+AllowedChars+1);

			} else {

				// Es ist der letzte Teil des Strings, der Loop hat bereits begonnen
				output = Originstring.substring(CharPosition);
				int secondStringWidth = AllowedChars - (StringLength - CharPosition);
				output += Originstring.substring(0,secondStringWidth);
			}
			realX = PosX-ColumnOffset;

			// cover left and right end of scrolling line
			writeString(realX, PosY, output);
			scrollDisplay.fillRect(PosX-7,PosY,7,8, SSD1306_BLACK); // at beginning
			scrollDisplay.fillRect(PosX+((AllowedChars-3)*7),PosY,7,8, SSD1306_BLACK);
			scrollDisplay.display();

		} else {
			// do nothing
		}
	} else {
		// do nothing, as already printed in constructor
	}	
}

void ScrollClass::writeString(int _posX, int _posY, String _text) {
	scrollDisplay.setCursor(_posX,_posY);
	scrollDisplay.print(_text);
}

bool ScrollClass::isScrollable() {
	return isScrollableFlag;
}