/*
* Space Trainer - shareable custom UI themes
* Folder: SpaceStuff\Themes\*.ini
*/
#include "CustomThemes.h"

#include "Menu.h"
#include "SpaceTheme.h"
#include "submenu_enum.h"
#include "submenu_switch.h"
#include "Language.h"
#include "Routine.h"

#include "..\Natives\natives2.h"
#include "..\Natives\types.h"
#include "..\Scripting\Game.h"
#include "..\Util\ExePath.h"
#include "..\Util\FileLogger.h"

#include <simpleini\SimpleIni.h>
#include <Windows.h>
#include <fstream>
#include <algorithm>
#include <cctype>

namespace CustomThemes
{
	static std::vector<ThemeFile> s_themes;
	static bool s_ready = false;
	static std::string s_activePath;

	static std::string Trim(std::string s)
	{
		while (!s.empty() && (unsigned char)s.back() <= ' ') s.pop_back();
		size_t i = 0;
		while (i < s.size() && (unsigned char)s[i] <= ' ') ++i;
		return s.substr(i);
	}

	static std::string SanitizeFileName(std::string name)
	{
		for (char& c : name)
		{
			if (c == '\\' || c == '/' || c == ':' || c == '*' || c == '?' || c == '"' || c == '<' || c == '>' || c == '|')
				c = '_';
		}
		return Trim(name);
	}

	static void WriteReadme(const std::string& dir)
	{
		const std::string path = dir + "\\README.txt";
		if (does_file_exist(path))
			return;
		std::ofstream f(path, std::ios::out | std::ios::trunc);
		if (!f.is_open()) return;
		f <<
			"Space Trainer - Custom Themes\r\n"
			"=============================\r\n"
			"Put .ini theme files in this folder to use them in the trainer.\r\n"
			"\r\n"
			"How to share:\r\n"
			"1) Open Settings -> Themes -> Custom Themes\r\n"
			"2) Adjust colours / SPACE UI as you like\r\n"
			"3) Save Current Theme As...\r\n"
			"4) Send the .ini file from this folder to friends\r\n"
			"5) They drop it here and press Reload / activate in menu\r\n"
			"\r\n"
			"File format: INI (UTF-8)\r\n"
			"[Theme]\r\n"
			"name=My Theme\r\n"
			"author=YourName\r\n"
			"\r\n"
			"[SpaceUI]\r\n"
			"accent=0          ; 0 Blue, 1 Purple, 2 Red, 3 Green, 4 White\r\n"
			"opacity=0.82\r\n"
			"scale=1.0\r\n"
			"blur=0.35\r\n"
			"animation=2      ; 0 Off, 1 Low, 2 Medium, 3 High\r\n"
			"space_ui=1\r\n"
			"\r\n"
			"[Colours]\r\n"
			"titlebox_R=59\r\n"
			"titlebox_G=130\r\n"
			"titlebox_B=246\r\n"
			"titlebox_A=255\r\n"
			"BG_R=10\r\n"
			"BG_G=14\r\n"
			"BG_B=20\r\n"
			"BG_A=210\r\n"
			"... (same keys as spaceConfig colours)\r\n";
	}

	void EnsureFolder()
	{
		if (s_ready) return;
		const std::string mainDir = GetPathffA(Pathff::Main, false);
		const std::string dir = GetPathffA(Pathff::Themes, false);
		CreateDirectoryA(mainDir.c_str(), nullptr);
		CreateDirectoryA(dir.c_str(), nullptr);
		WriteReadme(dir);

		// Example shareable theme
		const std::string example = dir + "\\SPACE_Blue.ini";
		if (!does_file_exist(example))
		{
			CSimpleIniA ini;
			ini.SetUnicode(true);
			ini.SetValue("Theme", "name", "SPACE Blue");
			ini.SetValue("Theme", "author", "xdigr");
			ini.SetLongValue("SpaceUI", "accent", 0);
			ini.SetDoubleValue("SpaceUI", "opacity", 0.82);
			ini.SetDoubleValue("SpaceUI", "scale", 1.0);
			ini.SetDoubleValue("SpaceUI", "blur", 0.35);
			ini.SetLongValue("SpaceUI", "animation", 2);
			ini.SetBoolValue("SpaceUI", "space_ui", true);
			ini.SetLongValue("Colours", "titlebox_R", 59);
			ini.SetLongValue("Colours", "titlebox_G", 130);
			ini.SetLongValue("Colours", "titlebox_B", 246);
			ini.SetLongValue("Colours", "titlebox_A", 255);
			ini.SetLongValue("Colours", "BG_R", 10);
			ini.SetLongValue("Colours", "BG_G", 14);
			ini.SetLongValue("Colours", "BG_B", 20);
			ini.SetLongValue("Colours", "BG_A", 210);
			ini.SetLongValue("Colours", "titletext_R", 255);
			ini.SetLongValue("Colours", "titletext_G", 255);
			ini.SetLongValue("Colours", "titletext_B", 255);
			ini.SetLongValue("Colours", "titletext_A", 255);
			ini.SetLongValue("Colours", "optiontext_R", 255);
			ini.SetLongValue("Colours", "optiontext_G", 255);
			ini.SetLongValue("Colours", "optiontext_B", 255);
			ini.SetLongValue("Colours", "optiontext_A", 255);
			ini.SetLongValue("Colours", "selectedtext_R", 255);
			ini.SetLongValue("Colours", "selectedtext_G", 255);
			ini.SetLongValue("Colours", "selectedtext_B", 255);
			ini.SetLongValue("Colours", "selectedtext_A", 255);
			ini.SetLongValue("Colours", "optionbreaks_R", 139);
			ini.SetLongValue("Colours", "optionbreaks_G", 149);
			ini.SetLongValue("Colours", "optionbreaks_B", 165);
			ini.SetLongValue("Colours", "optionbreaks_A", 200);
			ini.SetLongValue("Colours", "optioncount_R", 139);
			ini.SetLongValue("Colours", "optioncount_G", 149);
			ini.SetLongValue("Colours", "optioncount_B", 165);
			ini.SetLongValue("Colours", "optioncount_A", 220);
			ini.SetLongValue("Colours", "selectionhi_R", 30);
			ini.SetLongValue("Colours", "selectionhi_G", 45);
			ini.SetLongValue("Colours", "selectionhi_B", 65);
			ini.SetLongValue("Colours", "selectionhi_A", 180);
			ini.SaveFile(example.c_str());
		}

		s_ready = true;
	}

	static void ReadColour(CSimpleIniA& ini, const char* key, RGBA& out)
	{
		out.R = (int)ini.GetLongValue("Colours", (std::string(key) + "_R").c_str(), out.R);
		out.G = (int)ini.GetLongValue("Colours", (std::string(key) + "_G").c_str(), out.G);
		out.B = (int)ini.GetLongValue("Colours", (std::string(key) + "_B").c_str(), out.B);
		out.A = (int)ini.GetLongValue("Colours", (std::string(key) + "_A").c_str(), out.A);
	}

	static void WriteColour(CSimpleIniA& ini, const char* key, const RGBA& c)
	{
		ini.SetLongValue("Colours", (std::string(key) + "_R").c_str(), c.R);
		ini.SetLongValue("Colours", (std::string(key) + "_G").c_str(), c.G);
		ini.SetLongValue("Colours", (std::string(key) + "_B").c_str(), c.B);
		ini.SetLongValue("Colours", (std::string(key) + "_A").c_str(), c.A);
	}

	void Rescan()
	{
		EnsureFolder();
		s_themes.clear();
		const std::string dir = GetPathffA(Pathff::Themes, false);
		std::vector<std::string> files;
		get_all_filenames_with_extension(dir, ".ini", files, true);
		get_all_filenames_with_extension(dir, ".spacetheme", files, true);
		std::sort(files.begin(), files.end());
		files.erase(std::unique(files.begin(), files.end()), files.end());

		for (const auto& fname : files)
		{
			std::string lower = fname;
			for (char& c : lower) c = (char)std::tolower((unsigned char)c);
			if (lower == "readme.txt") continue;

			const std::string path = dir + "\\" + fname;
			CSimpleIniA ini;
			ini.SetUnicode(true);
			if (ini.LoadFile(path.c_str()) != SI_OK)
				continue;

			ThemeFile t;
			t.filePath = path;
			t.name = ini.GetValue("Theme", "name", "");
			t.author = ini.GetValue("Theme", "author", "");
			if (t.name.empty())
			{
				auto dot = fname.find_last_of('.');
				t.name = (dot == std::string::npos) ? fname : fname.substr(0, dot);
			}
			s_themes.push_back(std::move(t));
		}
		addlog(ige::LogType::LOG_INFO, "CustomThemes: " + std::to_string(s_themes.size()) + " themes in " + dir);
	}

	const std::vector<ThemeFile>& GetThemes()
	{
		if (s_themes.empty())
			Rescan();
		return s_themes;
	}

	bool SaveCurrentTheme(const std::string& displayName, const std::string& author)
	{
		EnsureFolder();
		std::string safe = SanitizeFileName(displayName);
		if (safe.empty())
			safe = "MyTheme";

		const std::string path = GetPathffA(Pathff::Themes, true) + safe + ".ini";
		CSimpleIniA ini;
		ini.SetUnicode(true);
		ini.SetValue("Theme", "name", displayName.c_str());
		ini.SetValue("Theme", "author", author.empty() ? "Unknown" : author.c_str());
		ini.SetValue("Theme", "version", "1");

		ini.SetLongValue("SpaceUI", "accent", (long)SpaceTheme::accentPreset);
		ini.SetDoubleValue("SpaceUI", "opacity", SpaceTheme::uiOpacity);
		ini.SetDoubleValue("SpaceUI", "scale", SpaceTheme::uiScale);
		ini.SetDoubleValue("SpaceUI", "blur", SpaceTheme::uiBlur);
		ini.SetLongValue("SpaceUI", "animation", (long)SpaceTheme::animQuality);
		ini.SetBoolValue("SpaceUI", "space_ui", SpaceTheme::spaceUiEnabled);

		WriteColour(ini, "titlebox", titlebox);
		WriteColour(ini, "BG", BG);
		WriteColour(ini, "titletext", titletext);
		WriteColour(ini, "optiontext", optiontext);
		WriteColour(ini, "selectedtext", selectedtext);
		WriteColour(ini, "optionbreaks", optionbreaks);
		WriteColour(ini, "optioncount", optioncount);
		WriteColour(ini, "selectionhi", selectionhi);
		WriteColour(ini, "pedtrackers", _globalPedTrackers_Col);

		ini.SetLongValue("Fonts", "title", font_title);
		ini.SetLongValue("Fonts", "options", font_options);
		ini.SetLongValue("Fonts", "selection", font_selection);
		ini.SetLongValue("Fonts", "breaks", font_breaks);
		ini.SetLongValue("Fonts", "hud", font_hud);
		ini.SetLongValue("Fonts", "speedo", font_speedo);

		ini.SetBoolValue("Flags", "gradients", Menu::gradients);
		ini.SetBoolValue("Flags", "rainbow", rainbowBoxes);
		ini.SetBoolValue("Flags", "thin_line", Menu::thinLineOverScrect);

		if (ini.SaveFile(path.c_str()) < 0)
			return false;

		s_activePath = path;
		Rescan();
		return true;
	}

	bool LoadThemeFile(const std::string& filePath)
	{
		CSimpleIniA ini;
		ini.SetUnicode(true);
		if (ini.LoadFile(filePath.c_str()) != SI_OK)
			return false;

		SpaceTheme::accentPreset = (SpaceTheme::AccentPreset)ini.GetLongValue("SpaceUI", "accent", (long)SpaceTheme::accentPreset);
		SpaceTheme::uiOpacity = (float)ini.GetDoubleValue("SpaceUI", "opacity", SpaceTheme::uiOpacity);
		SpaceTheme::uiScale = (float)ini.GetDoubleValue("SpaceUI", "scale", SpaceTheme::uiScale);
		SpaceTheme::uiBlur = (float)ini.GetDoubleValue("SpaceUI", "blur", SpaceTheme::uiBlur);
		SpaceTheme::animQuality = (SpaceTheme::AnimQuality)ini.GetLongValue("SpaceUI", "animation", (long)SpaceTheme::animQuality);
		SpaceTheme::spaceUiEnabled = ini.GetBoolValue("SpaceUI", "space_ui", SpaceTheme::spaceUiEnabled);

		ReadColour(ini, "titlebox", titlebox);
		ReadColour(ini, "BG", BG);
		ReadColour(ini, "titletext", titletext);
		ReadColour(ini, "optiontext", optiontext);
		ReadColour(ini, "selectedtext", selectedtext);
		ReadColour(ini, "optionbreaks", optionbreaks);
		ReadColour(ini, "optioncount", optioncount);
		ReadColour(ini, "selectionhi", selectionhi);
		ReadColour(ini, "pedtrackers", _globalPedTrackers_Col);

		font_title = (INT8)ini.GetLongValue("Fonts", "title", font_title);
		font_options = (INT8)ini.GetLongValue("Fonts", "options", font_options);
		font_selection = (INT8)ini.GetLongValue("Fonts", "selection", font_selection);
		font_breaks = (INT8)ini.GetLongValue("Fonts", "breaks", font_breaks);
		font_hud = (INT8)ini.GetLongValue("Fonts", "hud", font_hud);
		font_speedo = (INT8)ini.GetLongValue("Fonts", "speedo", font_speedo);

		Menu::gradients = ini.GetBoolValue("Flags", "gradients", Menu::gradients);
		rainbowBoxes = ini.GetBoolValue("Flags", "rainbow", rainbowBoxes);
		Menu::thinLineOverScrect = ini.GetBoolValue("Flags", "thin_line", Menu::thinLineOverScrect);

		if (SpaceTheme::spaceUiEnabled)
		{
			// Keep colours from file; only clamp UI metrics.
			if (SpaceTheme::uiOpacity < 0.4f) SpaceTheme::uiOpacity = 0.4f;
			if (SpaceTheme::uiOpacity > 1.0f) SpaceTheme::uiOpacity = 1.0f;
			if (SpaceTheme::uiScale < 0.8f) SpaceTheme::uiScale = 0.8f;
			if (SpaceTheme::uiScale > 1.2f) SpaceTheme::uiScale = 1.2f;
		}

		s_activePath = filePath;
		return true;
	}

	bool DeleteThemeFile(const std::string& filePath)
	{
		if (filePath.empty() || !does_file_exist(filePath))
			return false;
		const bool ok = DeleteFileA(filePath.c_str()) != 0;
		if (ok && s_activePath == filePath)
			s_activePath.clear();
		Rescan();
		return ok;
	}

	void Menu()
	{
		EnsureFolder();
		AddTitle("Custom Themes");

		bool reload = false;
		bool save = false;
		AddOption("Reload Themes Folder", reload);
		AddOption("Save Current Theme As...", save);
		if (reload)
		{
			Rescan();
			Game::Print::PrintBottomLeft("Themes reloaded (" + std::to_string(s_themes.size()) + ").");
		}
		if (save)
		{
			std::string name = Game::InputBox("", 40U, "Theme name", "My Theme");
			if (!name.empty())
			{
				std::string author = Game::InputBox("", 30U, "Author name", "Unknown");
				if (SaveCurrentTheme(name, author.empty() ? "Unknown" : author))
					Game::Print::PrintBottomLeft("~g~Saved to SpaceStuff\\Themes");
				else
					Game::Print::PrintBottomLeft("~r~Failed to save theme.");
			}
		}

		AddBreak("---Folder---");
		AddOption("SpaceStuff\\Themes", null);
		AddOption("Drop .ini files here to share/use", null);

		const auto& themes = GetThemes();
		if (themes.empty())
		{
			AddBreak("---Empty---");
			AddOption("No custom themes yet", null);
			return;
		}

		AddBreak("---Custom Themes---");
		for (const auto& t : themes)
		{
			bool pressed = false;
			bool del = false;
			std::string label = t.name;
			if (!t.author.empty())
				label += " [" + t.author + "]";
			const bool active = (!s_activePath.empty() && s_activePath == t.filePath);
			AddTickol(label, active, pressed, pressed, TICKOL::BOXTICK, TICKOL::BOXBLANK);
			if (pressed)
			{
				if (LoadThemeFile(t.filePath))
					Game::Print::PrintBottomLeft("~g~Theme applied:~s~ " + t.name);
				else
					Game::Print::PrintBottomLeft("~r~Failed to load theme.");
			}

			// Hold tip: delete via separate option when selected
			if (active || Menu::printingop == *Menu::currentopATM)
			{
				// no-op; deletion via dedicated button below when selected
			}
			(void)del;
		}

		bool deleteActive = false;
		AddOption("~r~Delete Selected Theme File", deleteActive);
		if (deleteActive)
		{
			if (s_activePath.empty())
				Game::Print::PrintBottomLeft("~y~Activate a theme first.");
			else if (DeleteThemeFile(s_activePath))
				Game::Print::PrintBottomLeft("Theme file deleted.");
			else
				Game::Print::PrintBottomLeft("~r~Could not delete theme.");
		}
	}
}

REGISTER_SUBMENU(CUSTOM_THEMES, CustomThemes::Menu)
