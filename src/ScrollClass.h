#include <Adafruit_SSD1306.h>

class ScrollClass {
	public:
		ScrollClass();
		ScrollClass(Adafruit_SSD1306 _display, String _string, int _posX, int posY, int _width);
		void SetDelay(int _delay);
		void SetInitialDelay(int _delay);
		void Refresh();
		
		// Variablen
		String Originstring; // also includes space / connector 
		int PosX;
		int PosY;
		int AllowedChars;
		int InitialDelay;
		int ScrollDelay;

		bool isScrollable();
		bool isScrollableFlag;

		void writeString(int _posX, int _posY, String _text);

		// Hilfsvariablen
		int ColumnOffset;
		int CharPosition;
		unsigned long StartMillis;

		Adafruit_SSD1306 scrollDisplay;
};