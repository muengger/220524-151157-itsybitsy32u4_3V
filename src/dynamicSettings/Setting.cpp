#include "Setting.h"

Setting::Setting() {
	// do nothing
};

Setting::Setting(String _name, String _description, int _step, String _setUnit) {
	Name = _name;
	Description = _description;
	Step = _step;
	Unit = _setUnit;
}

Setting::~Setting() {
	// do nothing
}

String Setting::getName() {
	return Name;
}

String Setting::getDescription() {
	return Description;
}

int Setting::getStep() {
	return Step;
}

String Setting::getUnit() {
	return Unit;
}

void Setting::setValueUsed(int _valueUsed) {
	ValueUsed = _valueUsed;
}

int Setting::getValueUsed() {
	return ValueUsed;
}