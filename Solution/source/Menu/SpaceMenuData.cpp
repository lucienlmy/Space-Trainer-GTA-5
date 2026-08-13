/*
* Space Trainer - data-driven main hub categories
*/
#include "SpaceMenuData.h"

namespace SpaceMenuData
{
	const std::vector<Category>& MainCategories()
	{
		static const std::vector<Category> cats = {
			{ "player",      "Player",       "Player abilities, health and movement.", SUB::PLAYEROPS },
			{ "bodyguards",  "Bodyguards",   "Hire and manage elite bodyguards.", SUB::BODYGUARDMAINMENU },
			{ "weapon",      "Weapons",      "Weapons, ammo and combat modifiers.", SUB::WEAPONOPS },
			{ "vehicle",     "Vehicles",     "Current vehicle tools and godmode.", SUB::VEHICLEOPS },
			{ "spawner",     "Spawner",      "Spawn vehicles, peds, animals and props.", SUB::SPAWNER_HUB },
			{ "teleport",    "Teleport",     "Travel the map instantly.", SUB::TELEPORTOPS },
			{ "world",       "World",        "Control the environment.", SUB::WORLD_HUB },
			{ "weather",     "Weather",      "Change and freeze weather.", SUB::WEATHEROPS },
			{ "time",        "Time",         "Set, freeze and speed up time.", SUB::TIMEOPS },
			{ "npc",         "NPC",          "Spawn and control NPCs.", SUB::NPC_HUB },
			{ "animations",  "Animations",   "Player and NPC animations.", SUB::ANIMATIONSUB },
			{ "functions",   "Functions",    "Freecam, disasters, professions and tools.", SUB::FUNCTIONS_HUB },
			{ "appearance",  "Appearance",   "SPACE TRAINER look, themes and UI scale.", SUB::APPEARANCE_HUB },
			{ "stats",       "Statistics",   "Live player and world statistics.", SUB::STATISTICS_HUB },
			{ "settings",    "Settings",     "Language, binds and config.", SUB::SETTINGS },
			{ "about",       "SPACE TRAINER","About SPACE TRAINER and links.", SUB::SPACE_ABOUT },
		};
		return cats;
	}
}
