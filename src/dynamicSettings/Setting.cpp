#include "Setting.h"

String Name="NameInit";

Setting::Setting() {
	//
};

Setting::Setting(String _name, String _description, int _step, String _setUnit) {
	Name = "nameOfAllSettings";
	Description = _description;
	Step = _step;
	Unit = _setUnit;
	Serial.print("Saved Setting with Name: ");
	Serial.println(Name);
}

Setting::~Setting() {
	// do something
}

/*int Setting::getCurrentValue() {
	return VarInteger;
}

void Setting::BindVariable(int _integer) {
	VarInteger = _integer;
}

*/
String Setting::getName() {
	return Name;
}
/*

String Setting::getDescription() {
	return Description;
}

int Setting::getStep() {
	return Step;
}

String Setting::getUnit() {
	return Unit;
}*/