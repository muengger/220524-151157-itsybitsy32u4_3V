#include <Arduino.h>
#include "SettingsManager.h"
//#include "dynamicSettings/Setting.h"
#define maxPosition 2

//Setting List<Setting>;
Setting _settings[maxPosition];

SettingsManager::SettingsManager() {
	Serial.println("SettingsManager loaded");
	CurrentPosition = 0;
	Setting Diameter = Setting("Raddurchmesser", "Benötigt für Kalk.", 1, "mm"); // TODO -> save settings to a global SettingsManager
	Setting MaxSpeedKid = Setting("Max. Tempo", "- Kind -", 1, "km/h"); // TODO -> save settings to a global SettingsManager
	Setting MaxSpeedTeen = Setting("Max. Tempo", "- Teenie -", 1, "km/h"); // TODO -> save settings to a global SettingsManager
	Diameter.Name = "Set0Direct";
	MaxSpeedKid.Name = "Set1Direct";
	_settings[0] = Diameter;
	_settings[1] = MaxSpeedKid;
	//_settings[2] = MaxSpeedTeen;
}

/*SettingsManager::~SettingsManager() {
	// do nothing
}*/

/*void SettingsManager::add(Setting& _setting) {
	//_settings.
	return true;
}*/

int SettingsManager::count() {
	
	return sizeof _settings;
}
String SettingsManager::getName() {
	return _settings[CurrentPosition].getName();
}
		
/*		void includeSetting(Setting _setting); // add a setting to the setting-manager
		Setting readCurrentSetting(); // reads current setting
*/		
// navigate trought settings

void SettingsManager::nextSetting() {
	if(CurrentPosition==maxPosition) {
		CurrentPosition = 0;
	} else {
		CurrentPosition++;
	}
	Serial.print("Inc Pos to ");
	Serial.println(CurrentPosition);
};

void SettingsManager::previousSetting() {
	if(CurrentPosition == 0) {
		CurrentPosition = maxPosition;
	} else {
		CurrentPosition--;
	}
	Serial.print("Dec Pos to ");
	Serial.println(CurrentPosition);
};
