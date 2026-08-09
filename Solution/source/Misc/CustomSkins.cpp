/*
* Space Trainer - load custom addon ped skins from SpaceStuff\CustomSkins
*/
#include "CustomSkins.h"
#include "HeroAbilities.h"

#include "..\Menu\Menu.h"
#include "..\Menu\Routine.h"
#include "..\Menu\submenu_enum.h"
#include "..\Menu\submenu_switch.h"
#include "..\Natives\natives2.h"
#include "..\Scripting\Game.h"
#include "..\Scripting\Model.h"
#include "..\Scripting\GTAped.h"
#include "..\Submenus\PedModelChanger.h"
#include "..\Submenus\PedComponentChanger.h"
#include "..\Util\ExePath.h"
#include "..\Util\FileLogger.h"
#include "..\Util\StringManip.h"

#include <Windows.h>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

namespace CustomSkins
{
	static std::vector<SkinEntry> s_skins;
	static bool s_folderReady = false;

	static std::string Trim(std::string s)
	{
		while (!s.empty() && (s.back() == '\r' || s.back() == '\n' || s.back() == ' ' || s.back() == '\t'))
			s.pop_back();
		size_t i = 0;
		while (i < s.size() && (s[i] == ' ' || s[i] == '\t'))
			++i;
		return s.substr(i);
	}

	static std::string Lower(std::string s)
	{
		for (char& c : s)
			c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
		return s;
	}

	static std::string FileStem(const std::string& fileName)
	{
		const auto slash = fileName.find_last_of("\\/");
		std::string name = (slash == std::string::npos) ? fileName : fileName.substr(slash + 1);
		const auto dot = name.find_last_of('.');
		if (dot != std::string::npos)
			name = name.substr(0, dot);
		return name;
	}

	static void WriteTextFile(const std::string& path, const std::string& content)
	{
		if (does_file_exist(path))
			return;
		std::ofstream f(path, std::ios::out | std::ios::trunc);
		if (f.is_open())
			f << content;
	}

	void EnsureFolder()
	{
		if (s_folderReady)
			return;

		const std::string mainDir = GetPathffA(Pathff::Main, false);
		const std::string skinsDir = GetPathffA(Pathff::CustomSkins, false);
		CreateDirectoryA(mainDir.c_str(), nullptr);
		CreateDirectoryA(skinsDir.c_str(), nullptr);

		WriteTextFile(skinsDir + "\\README.txt",
			"Space Trainer - Custom Skins\r\n"
			"============================\r\n"
			"1) Install addon ped mods with OpenIV / your mod manager (the actual .ydd/.ytd model).\r\n"
			"2) Put a .skin / .txt / .ini file here with the model name used by that mod.\r\n"
			"3) Or put outfit .xml files (OutfitPedData) here.\r\n"
			"4) Open Space Trainer -> Custom Skins & Heroes and activate.\r\n"
			"\r\n"
			"Example .skin file:\r\n"
			"name=Superman\r\n"
			"model=Superman\r\n"
			"ability=superman\r\n"
			"\r\n"
			"ability values: none, superman, batman, flash, hulk, ironman, spiderman, wolverine, aquaman\r\n"
			"You can also put only one line with the model name.\r\n");

		WriteTextFile(skinsDir + "\\Superman.skin",
			"name=Superman\r\n"
			"model=Superman\r\n"
			"ability=superman\r\n");

		WriteTextFile(skinsDir + "\\Batman.skin",
			"name=Batman\r\n"
			"model=Batman\r\n"
			"ability=batman\r\n");

		WriteTextFile(skinsDir + "\\Flash.skin",
			"name=The Flash\r\n"
			"model=Flash\r\n"
			"ability=flash\r\n");

		WriteTextFile(skinsDir + "\\Hulk.skin",
			"name=Hulk\r\n"
			"model=Hulk\r\n"
			"ability=hulk\r\n");

		WriteTextFile(skinsDir + "\\IronMan.skin",
			"name=Iron Man\r\n"
			"model=IronMan\r\n"
			"ability=ironman\r\n");

		WriteTextFile(skinsDir + "\\SpiderMan.skin",
			"name=Spider-Man\r\n"
			"model=SpiderMan\r\n"
			"ability=spiderman\r\n");

		s_folderReady = true;
	}

	static bool ParseKeyValueFile(const std::string& path, SkinEntry& out)
	{
		std::ifstream f(path);
		if (!f.is_open())
			return false;

		out.displayName = FileStem(path);
		out.modelName.clear();
		out.abilityName.clear();
		out.filePath = path;
		out.type = SkinType::ModelFile;

		std::string line;
		bool any = false;
		while (std::getline(f, line))
		{
			line = Trim(line);
			if (line.empty() || line[0] == '#' || line[0] == ';')
				continue;

			const auto eq = line.find('=');
			if (eq == std::string::npos)
			{
				// Single model name line
				out.modelName = line;
				any = true;
				continue;
			}

			std::string key = Lower(Trim(line.substr(0, eq)));
			std::string val = Trim(line.substr(eq + 1));
			if (key == "name" || key == "title" || key == "display")
				out.displayName = val;
			else if (key == "model" || key == "ped" || key == "hashname")
				out.modelName = val;
			else if (key == "ability" || key == "power" || key == "hero")
				out.abilityName = val;
			any = true;
		}

		if (!any)
			return false;
		if (out.modelName.empty())
			out.modelName = out.displayName;

		out.modelHash = GET_HASH_KEY(out.modelName.c_str());
		return true;
	}

	static bool ParseOutfitXml(const std::string& path, SkinEntry& out)
	{
		std::ifstream f(path);
		if (!f.is_open())
			return false;
		std::stringstream buf;
		buf << f.rdbuf();
		const std::string content = buf.str();
		if (content.find("OutfitPedData") == std::string::npos && content.find("ModelHash") == std::string::npos)
			return false;

		out.displayName = FileStem(path);
		out.filePath = path;
		out.type = SkinType::OutfitXml;
		out.modelName.clear();
		out.modelHash = 0;
		out.abilityName.clear();

		// Optional ability comment: <!-- ability=superman -->
		const auto ab = content.find("ability=");
		if (ab != std::string::npos)
		{
			size_t start = ab + 8;
			size_t end = start;
			while (end < content.size() && !std::isspace(static_cast<unsigned char>(content[end])) && content[end] != '-' && content[end] != '>')
				++end;
			out.abilityName = Trim(content.substr(start, end - start));
		}
		return true;
	}

	void Rescan()
	{
		EnsureFolder();
		s_skins.clear();

		const std::string dir = GetPathffA(Pathff::CustomSkins, false);
		std::vector<std::string> files;
		get_all_filenames_with_extension(dir, ".skin", files, true);
		get_all_filenames_with_extension(dir, ".txt", files, true);
		get_all_filenames_with_extension(dir, ".ini", files, true);
		get_all_filenames_with_extension(dir, ".xml", files, true);

		std::sort(files.begin(), files.end());
		files.erase(std::unique(files.begin(), files.end()), files.end());

		for (const auto& fname : files)
		{
			if (Lower(fname) == "readme.txt")
				continue;

			const std::string path = dir + "\\" + fname;
			SkinEntry entry;
			const std::string lower = Lower(fname);

			bool ok = false;
			if (lower.size() >= 4 && lower.substr(lower.size() - 4) == ".xml")
				ok = ParseOutfitXml(path, entry);
			else
				ok = ParseKeyValueFile(path, entry);

			if (ok)
				s_skins.push_back(std::move(entry));
		}

		addlog(ige::LogType::LOG_INFO, "CustomSkins: loaded " + std::to_string(s_skins.size()) + " entries from " + dir);
	}

	const std::vector<SkinEntry>& GetSkins()
	{
		if (s_skins.empty())
			Rescan();
		return s_skins;
	}

	bool Activate(const SkinEntry& skin)
	{
		bool ok = false;

		if (skin.type == SkinType::OutfitXml)
		{
			const std::string& path = skin.filePath;
			ok = sub::ComponentChangerOutfit::Apply(PLAYER_PED_ID(), path, true, false, false, false, false, false);
			ok = sub::ComponentChangerOutfit::Apply(PLAYER_PED_ID(), path, false, true, true, true, true, true) || ok;
			if (ok)
				Game::Print::PrintBottomLeft("~g~Outfit applied:~s~ " + skin.displayName);
			else
				Game::Print::PrintBottomLeft("~r~Failed to apply outfit XML.");
		}
		else
		{
			GTAmodel::Model model(skin.modelHash ? skin.modelHash : GET_HASH_KEY(skin.modelName.c_str()));
			if (!model.IsInCdImage() || !model.IsValid())
			{
				Game::Print::PrintBottomLeft("~r~Model not found:~s~ " + skin.modelName + " (install addon ped mod first)");
				return false;
			}
			if (!model.IsPed())
			{
				Game::Print::PrintBottomLeft("~r~Not a ped model:~s~ " + skin.modelName);
				return false;
			}
			sub::ChangeModel(model);
			ok = true;
			Game::Print::PrintBottomLeft("~g~Skin:~s~ " + skin.displayName);
		}

		if (ok && !skin.abilityName.empty())
		{
			const auto ability = HeroAbilities::ParseAbilityName(skin.abilityName);
			if (ability != HeroAbilities::Id::None)
				HeroAbilities::Apply(ability);
		}
		return ok;
	}

	static void AddSkinByModelName()
	{
		std::string modelName = Game::InputBox("", 60U, "Addon ped model name", "");
		if (modelName.empty())
			return;

		std::string display = Game::InputBox(modelName, 60U, "Display name", modelName);
		if (display.empty())
			display = modelName;

		std::string ability = Game::InputBox("", 30U, "Ability (superman/batman/flash/... or empty)", "");

		EnsureFolder();
		std::string safe = display;
		for (char& c : safe)
		{
			if (c == '\\' || c == '/' || c == ':' || c == '*' || c == '?' || c == '"' || c == '<' || c == '>' || c == '|')
				c = '_';
		}

		const std::string path = GetPathffA(Pathff::CustomSkins, true) + safe + ".skin";
		std::ofstream f(path, std::ios::out | std::ios::trunc);
		if (!f.is_open())
		{
			Game::Print::PrintBottomLeft("~r~Could not write skin file.");
			return;
		}
		f << "name=" << display << "\r\n";
		f << "model=" << modelName << "\r\n";
		if (!ability.empty())
			f << "ability=" << ability << "\r\n";
		f.close();

		Rescan();
		Game::Print::PrintBottomLeft("~g~Saved:~s~ " + path);
	}

	void Menu()
	{
		EnsureFolder();
		AddTitle("Custom Skins");

		bool reload = false;
		bool addModel = false;
		bool openHeroes = false;
		AddOption("Hero Abilities", openHeroes, nullFunc, SUB::HEROABILITIES);
		AddOption("Reload Skins Folder", reload);
		AddOption("Add Skin By Model Name", addModel);

		if (reload)
		{
			Rescan();
			Game::Print::PrintBottomLeft("Skins reloaded (" + std::to_string(s_skins.size()) + ").");
		}
		if (addModel)
			AddSkinByModelName();

		AddBreak("---Folder---");
		AddOption("SpaceStuff\\CustomSkins", null);

		const auto& skins = GetSkins();
		if (skins.empty())
		{
			AddBreak("---Empty---");
			AddOption("Put .skin / .xml files in folder", null);
			return;
		}

		AddBreak("---Skins---");
		for (const auto& skin : skins)
		{
			bool pressed = false;
			std::string label = skin.displayName;
			if (skin.type == SkinType::OutfitXml)
				label += " [XML]";
			else if (!skin.modelName.empty())
				label += " [" + skin.modelName + "]";
			if (!skin.abilityName.empty())
				label += " *" + skin.abilityName;

			AddOption(label, pressed);
			if (pressed)
				Activate(skin);
		}
	}
}

REGISTER_SUBMENU(CUSTOMSKINS, CustomSkins::Menu)
