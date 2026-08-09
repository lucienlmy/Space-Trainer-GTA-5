/*
* Space Trainer - shareable custom UI themes
*/
#pragma once

#include <string>
#include <vector>

namespace CustomThemes
{
	struct ThemeFile
	{
		std::string name;     // display name
		std::string filePath; // full path
		std::string author;
	};

	void EnsureFolder();
	void Rescan();
	const std::vector<ThemeFile>& GetThemes();

	bool SaveCurrentTheme(const std::string& displayName, const std::string& author = "Unknown");
	bool LoadThemeFile(const std::string& filePath);
	bool DeleteThemeFile(const std::string& filePath);

	void Menu();
}
