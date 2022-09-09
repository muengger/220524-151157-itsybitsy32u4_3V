class ScrollClass {
	public:
		ScrollClass(String _string, int _posX, int posY, int _width);
		void SetDelay(int _delay);
		void SetInitialDelay(int _delay);
		void Refresh();
		
		// Variablen
		String Originstring; // also includes space / connector 
		int PosX;
		int PosY;
		int Width;
		int InitialDelay;
		int ScrollDelay;

		// Hilfsvariablen
		int ColumnOffset;
		int CharPosition;
		unsigned long StartMillis;
};