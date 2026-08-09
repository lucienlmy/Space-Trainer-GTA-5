/*
* Space Trainer - load custom addon ped skins from SpaceStuff\CustomSkins
*/
#pragma once

#include <string>
#include <vector>

#include "..\Natives\types.h"

namespace CustomSkins
{
	enum class SkinType : int
	{
		ModelFile = 0,
		OutfitXml = 1
	};

	struct SkinEntry
	{
		std::string displayName;
		std::string modelName;
		Hash modelHash = 0;
		std::string filePath;
		std::string abilityName;
		SkinType type = SkinType::ModelFile;
	};

	void EnsureFolder();
	void Rescan();
	const std::vector<SkinEntry>& GetSkins();
	bool Activate(const SkinEntry& skin);
	void Menu();
}
