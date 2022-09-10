#include <Arduino.h>
#include "SettingsManagerClass.h"
#include "Globals.h"
#include "FlashAsEEPROM_SAMD.h"

Setting _settings[12];

// Constructor
SettingsManagerClass::SettingsManagerClass() {

	CurrentSetting = PersistedSetting::WheelDiameter; 

	// Define Changeable Settings
	
	Setting Diameter = Setting("Raddurchmesser", "Notwendig fur Temporbererchnung - ", 1, "mm");
	Diameter.setValueUsed(20); // set default value if nothing is persisted

	Setting MaxSpeedKid = Setting("Max. Tempo", "Kindergarten", 1, "km/h");
	MaxSpeedKid.setValueUsed(15);

	Setting MaxTorqueKid = Setting("Max. Drehmoment", "Kindergarten", 1, "M");
	MaxTorqueKid.setValueUsed(1.5);

	Setting MaxSpeedTeen = Setting("Max. Tempo", "Schulkind", 1, "km/h" );
	MaxSpeedTeen.setValueUsed(25);

	Setting MaxTorqueTeen = Setting("Max. Drehmoment", "Schulkind", 1, "M");
	MaxTorqueTeen.setValueUsed(3);

	Setting BatMaxVoltage = Setting("Max. Spannung", "Die Batterie ist zu 100% geladen - ", 1, "V");
	BatMaxVoltage.setValueUsed(42);

	Setting BatMinVoltage = Setting("Min. Spannung", "Ab dieser Spannung wird abgeschalten - ", 1, "V");
	BatMaxVoltage.setValueUsed(33);

	Setting BatWarnPercentage = Setting("Bat. Warnung", "Ist die Spannung nur noch so gering, wird gewarnt - ", 1, "%");
	BatMaxVoltage.setValueUsed(5);

	Setting ThrootleForw = Setting("Max. nach vorn", "Maximalwert des Gasreglers - ", 1, "");
	ThrootleForw.setValueUsed(955);

	Setting ThrootleBackw = Setting("Max. nach hinten", "Minimalwert des Gasreglers - ", 1, "");
	ThrootleBackw.setValueUsed(0);

	Setting ThrootleStop = Setting("Stillstand", "Wert wenn Gasregler im Ruhezustand - ", 1, "");
	ThrootleStop.setValueUsed(340);

	Setting ThrootleStopTollerance = Setting("Tolleranz", "+/- Bereich um den Stillstand - ", 1, "+/-");
	ThrootleStopTollerance.setValueUsed(10);

	// Index-IDs of following Array is taken from Enum class "PersistedSettings"
	_settings[0] = Diameter; // index should be replaced with (int)PersistedSetting::WheelDiameter
	_settings[1] = MaxSpeedKid;
	_settings[2] = MaxSpeedTeen;
	_settings[3] = MaxTorqueKid;
	_settings[4] = MaxTorqueTeen;
	_settings[5] = BatMinVoltage;
	_settings[6] = BatMaxVoltage;
	_settings[7] = BatWarnPercentage;
	_settings[8] = ThrootleForw;
	_settings[9] = ThrootleBackw;
	_settings[10] = ThrootleStop;
	_settings[11] = ThrootleStopTollerance;
	//Read all values from Memory

	for(int i = 0; i <= 10; i++) {
		PersistedSetting _set = EnumOfIndex(i);
		int valueMemory = getSettingValueFromMemory(_set);
		_settings[i].setValueUsed(valueMemory);
	}

	CurrentSetting = EnumOfIndex(0);
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
	if((int)CurrentSetting == 11) {
		CurrentSetting = SettingsManagerClass::EnumOfIndex(0);
		//Serial.println("Set Pos back to 0");
	} else {
		CurrentSetting = EnumOfIndex((int)CurrentSetting+1);
		//Serial.print("Inc Pos to ");
		//Serial.println(CurrentSetting);
	}
	isChangedFlag = true;
}

void SettingsManagerClass::previousSetting() {
	if((int)CurrentSetting == 0) {
		CurrentSetting = EnumOfIndex(11);
		//Serial.println("Set Pos back to 0");
	} else {
		CurrentSetting = EnumOfIndex((int)CurrentSetting-1);
		//Serial.print("Dec Pos to ");
		//Serial.println(CurrentSetting);
	}
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
