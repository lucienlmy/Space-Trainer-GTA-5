#pragma once

#include <vector>
#include <utility>
#include <string>

namespace TimecycleModification
{
	extern std::vector<std::pair<std::string, std::string>> vTimecycles; // pair<value, caption>

	bool PopulateTimecycleNames();

	bool IsModActive();
	int GetModId();
	void SetMod(const std::string& value);
	void SetMod(const std::string& value, float transition);
	void SetModStrength(float value);
	void ClearMod();
}