/*
* Space Trainer - graphics quality presets for weak / strong PCs
*/
#pragma once

namespace GraphicsQuality
{
	enum Preset : int
	{
		Off = 0,
		Weak = 1,
		Medium = 2,
		High = 3,
		VeryHigh = 4
	};

	extern int currentPreset;

	void Apply(int preset);
	void Tick();
	void Menu();
}
