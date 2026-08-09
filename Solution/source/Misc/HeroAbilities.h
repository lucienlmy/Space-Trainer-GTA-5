/*
* Space Trainer - hero power presets (Superman, Batman, etc.)
*/
#pragma once

#include <string>

namespace HeroAbilities
{
	enum class Id : int
	{
		None = 0,
		Superman,
		Batman,
		Flash,
		Hulk,
		IronMan,
		SpiderMan,
		Wolverine,
		Aquaman
	};

	extern Id activeId;

	const char* DisplayName(Id id);
	Id ParseAbilityName(const std::string& name);
	void Apply(Id id);
	void Clear(bool silent = false);
	void Tick();
	void Menu();
}
