#include <Arduino.h>
#include "SettingsManagerClass.h"
#include "Globals.h"
#include "FlashAsEEPROM_SAMD.h"

Setting _settings[3];

// Constructor
SettingsManagerClass::SettingsManagerClass() {

	// Define Changeable Settings
	CurrentSetting = PersistedSetting::WheelDiameter; 
	Setting Diameter = Setting("Raddurchmesser", "Notw. f. Kalk.", 1, "mm");
	Diameter.setValueUsed(20); // set default value if nothing is persisted
	Setting MaxSpeedKid = Setting("Max. Tempo", "- Kind -", 1, "km/h");
	MaxSpeedKid.setValueUsed(15);
	Setting MaxSpeedTeen = Setting("Max. Tempo", "- Teenie -", 1, "km/h" );
	MaxSpeedTeen.setValueUsed(25);
	


	// Index-IDs of following Array is taken from Enum class "PersistedSettings"
	_settings[0] = Diameter; // 0 should be replaced with (int)PersistedSetting::WheelDiameter
	_settings[1] = MaxSpeedKid; // 1 should be replaced with (int)PersistedSetting::MaxSpeedKid
	_settings[2] = MaxSpeedTeen; // 2 should be replaced with PersistedSetting::MaxSpeedTeen
	//Read all values from Memory
	//int i = 0;
	for(int i = 0; i <= 2; i++) {
		PersistedSetting _set = EnumOfIndex(i);
		int valueMemory = getSettingValueFromMemory(_set);
		_settings[i].setValueUsed(valueMemory);
	}
}

// Destructor
SettingsManagerClass::~SettingsManagerClass() {
	// do nothing
}

// Get Name of current Setting
String SettingsManagerClass::getCurrentName() {
	return _settings[(int)CurrentSetting].getName();
}

// Get Description of current Setting
String SettingsManagerClass::getCurrentDescription() {
	return _settings[(int)CurrentSetting].getDescription();
	
}

// Call getStep() of Setting to get the size of Step for each in- or decrease
int SettingsManagerClass::getCurrentStep() {
	return _settings[(int)CurrentSetting].getStep();
}

// Call getUnit() of Setting to know the Suffix
String SettingsManagerClass::getCurrentUnit() {
	return _settings[(int)CurrentSetting].getUnit();
}

// Call getValue of Setting to 
int SettingsManagerClass::getCurrentValueUsed() {
	return _settings[(int)CurrentSetting].getValueUsed();
}

int SettingsManagerClass::getSettingValueFromMemory(PersistedSetting _selectedSetting) {
	int address = (int)_selectedSetting;
	int value = EEPROM.read(address);
	return value;
}

void SettingsManagerClass::setSettingValueToMemory(PersistedSetting _currentSetting, int _currentValue) {
	EEPROM.update((int)_currentSetting, _currentValue);
	EEPROM.commit();
}

int SettingsManagerClass::getCurrentValueFromMemory() {
	return getSettingValueFromMemory(CurrentSetting);
}

void SettingsManagerClass::setCurrentValueToMemory(int _currentValue) {
	setSettingValueToMemory(CurrentSetting, _currentValue);
}

int getValueUsedOfSetting(PersistedSetting _selectedSetting) {
	return _settings[(int)_selectedSetting].getValueUsed();
}

void SettingsManagerClass::nextSetting() {
	if((int)CurrentSetting == 2) {
		CurrentSetting = SettingsManagerClass::EnumOfIndex(0);
		//Serial.println("Set Pos back to 0");
	} else {
		CurrentSetting = EnumOfIndex((int)CurrentSetting+1);
		//Serial.print("Inc Pos to ");
		//Serial.println(CurrentSetting);
	}
	isChangedFlag = true;
	delay(1000);
}

void SettingsManagerClass::previousSetting() {
	if((int)CurrentSetting == 0) {
		CurrentSetting = EnumOfIndex(2);
		//Serial.println("Set Pos back to 0");
	} else {
		CurrentSetting = EnumOfIndex((int)CurrentSetting-1);
		//Serial.print("Dec Pos to ");
		//Serial.println(CurrentSetting);
	}
	//delay(1000);
	isChangedFlag = true;
}

bool SettingsManagerClass::isChanged() {
	return isChangedFlag;
}

void SettingsManagerClass::resetChangedFlag() {
	isChangedFlag = false;
}

int SettingsManagerClass::getCurrentSettingId() {
	return (int)CurrentSetting;
}

void SettingsManagerClass::saveCurrentValueToMemory() {
	setCurrentValueToMemory(_settings[(int)CurrentSetting].getValueUsed());
	isChangedFlag = true;
	delay(500);
}

void SettingsManagerClass::increaseCurrentValue(int factor) {
	int _currentValue = _settings[(int)CurrentSetting].getValueUsed();
	int _step = _settings[(int)CurrentSetting].getStep()*factor;
	int _newValue = _currentValue + _step;
	_settings[(int)CurrentSetting].setValueUsed(_newValue);
	Serial.print("Increased by Manager");
	isChangedFlag = true;
}

void SettingsManagerClass::increaseCurrentValue() {
	increaseCurrentValue(1);
}

void SettingsManagerClass::decreaseCurrentValue(int factor) {
	int _currentValue = _settings[(int)CurrentSetting].getValueUsed();
	int _step = _settings[(int)CurrentSetting].getStep()*factor;
	int _newValue = _currentValue - _step;
	_settings[(int)CurrentSetting].setValueUsed(_newValue);
	Serial.print("Decreased by Manager");
	isChangedFlag = true;
}

void SettingsManagerClass::decreaseCurrentValue() {
	decreaseCurrentValue(1);
}

PersistedSetting SettingsManagerClass::EnumOfIndex(int i) {
	PersistedSetting choosenSetting;
	choosenSetting = static_cast<PersistedSetting>(i);
	return choosenSetting;
}
