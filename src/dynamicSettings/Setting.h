#include <Arduino.h>

class Setting {
	public:
		// Constructor
		Setting();
		Setting(String _name, String _description, int _step, String _setUnit);

		// Destructor
		~Setting();
		
		// Returns the Name of the Setting
		String getName();

		// Returns the Description of the Setting
		String getDescription();

		// Returns the Step to In- or Decrease
		int getStep();

		// Returns the Suffix for the Value
		String getUnit();
		
		// Sets a value to be used for this Setting
		void setValueUsed(int _valueUsed);

		// Gets the value used for this Setting
		int getValueUsed();
		
	private:
		String Name;
		String Description;
		int Step;
		String Unit;
		int ValueUsed;

};