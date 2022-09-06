#include "dynamicSettings/Setting.h"

class SettingsManager {
	public: 
		SettingsManager();
		//~SettingsManager();
		
		//void add(Setting _setting); // add a setting to the setting-manager
		int count();
		
		// navigate trought settings
		void nextSetting();
		void previousSetting();

		// read / write data
		String getName();

		Setting _settings[2];
	private:
		int CurrentPosition;
};