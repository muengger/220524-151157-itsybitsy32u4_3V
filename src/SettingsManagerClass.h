#include "dynamicSettings/Setting.h"
#include "Globals.h"

// Responsible to read and save (persist) all settings
class SettingsManagerClass {
	public: 
		SettingsManagerClass();
		~SettingsManagerClass();

		// navigate trought settings
		void nextSetting();
		void previousSetting();
		int getCurrentSettingId();

		bool isChanged();
		void resetChangedFlag();

		// Read data
		// Read the Name of the Setting
		String getCurrentName();
		// Read the Description of the Setting
		String getCurrentDescription();
		// Read the size of a Step to take, when increasing or decreasing
		int getCurrentStep();
		// Read the Unit (Suffix) of the Settings Value
		String getCurrentUnit();
		// Read the currentValue displayed)
		int getCurrentValueUsed();
		// Save currentValue of currentSetting to memory
		void saveCurrentValueToMemory();

		// Reads setting value of a given Setting from Memory
		int getSettingValueFromMemory(PersistedSetting _currentSetting);
		// Stores value of a given setting to Memory
		void setSettingValueToMemory(PersistedSetting _currentSetting, int _currentValue);
		
		// Reads setting value of current Setting from Memory
		int getCurrentValueFromMemory();
		// Stores value of current setting to Memory
		void setCurrentValueToMemory(int _currentValue);

		int getValueUsedOfSetting(PersistedSetting _selectedSetting);
		int setValueUsedOfSetting(PersistedSetting _selectedSetting);

		// Change data displayed (currentValue)
		
		// Increases currentValue one Step
		void increaseCurrentValue(int factor);
		void increaseCurrentValue();
		
		// Decreases currentValue one Step
		void decreaseCurrentValue(int factor);
		void decreaseCurrentValue();

		Setting _settings[3];

	private:
		PersistedSetting CurrentSetting;
		bool isChangedFlag = false;

		PersistedSetting EnumOfIndex(int i);
};