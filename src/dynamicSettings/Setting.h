#include <Arduino.h>

class Setting {
	public:
		Setting();
		Setting(String _name, String _description, int _step, String _setUnit);
		~Setting();
		
		/*void BindVariable(int _integer);*/

		String getName();
		
		/*
		String getDescription();
		int getStep();
		String getUnit();

		int getCurrentValue();
		//void setIcon(18x18Icon icon_bytes);*/

	//private:
		String Name;
		String Description;
		int Step;
		String Unit;
		
		/*
		int VarInteger;
		int EEPROMAddress;*/
};