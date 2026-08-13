#include "MenuTips.h"

#include "Menu.h"
#include "Language.h"
#include "submenu_enum.h"
#include "..\Natives\natives2.h"
#include "..\Scripting\Game.h"
#include "..\Scripting\Model.h"
#include "..\Memory\GTAmemory.h"
#include "..\Util\GTAmath.h"

#include <unordered_map>
#include <unordered_set>
#include <cctype>

namespace MenuTips
{
	std::string g_hoveredTip;

	static std::string Lower(std::string s)
	{
		for (char& c : s)
			c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
		return s;
	}

	void Clear()
	{
		g_hoveredTip.clear();
	}

	void SetHoveredTipRaw(const std::string& tipAlreadyTranslated)
	{
		if (Menu::printingop == *Menu::currentopATM)
			g_hoveredTip = tipAlreadyTranslated;
	}

	void SetHoveredTip(const std::string& tipEnglish)
	{
		SetHoveredTipRaw(Language::TranslateToSelected(tipEnglish));
	}

	void ApplyBuiltinTipForOption(const std::string& optionEnglish)
	{
		static const std::unordered_map<std::string, std::string> tips
		{
			{ "Players", "Online/local players list and actions." },
			{ "Player Options", "Godmode, powers, movement, wanted level." },
			{ "Vehicle Options", "Current vehicle options and spawner." },
			{ "Teleport Options", "Teleport to map places and coordinates." },
			{ "Weapon Options", "Weapons, ammo and special guns." },
			{ "Weather Options", "Change weather and world atmosphere." },
			{ "Time Options", "Change clock time and freeze time." },
			{ "Bodyguard Options", "Spawn and manage bodyguards." },
			{ "Object Spooner", "Place, edit and save world entities." },
			{ "Misc Options", "Extra world and fun options." },
			{ "Creators", "Object spooner: place, edit and save props." },
			{ "Bodyguards", "Hire and manage elite bodyguards." },
			{ "Animations", "Play animations and scenarios." },
			{ "Spawner", "Spawn vehicles, peds, animals and props." },
			{ "World", "Control the environment." },
			{ "Weather", "Change and freeze weather." },
			{ "Time", "Set, freeze and speed up time." },
			{ "NPC", "Spawn and control NPCs." },
			{ "Functions", "Freecam, disasters, professions and tools." },
			{ "Appearance", "SPACE TRAINER look, themes and UI scale." },
			{ "Statistics", "Live player and world statistics." },
			{ "Misc", "Extra tools, FPS, skins and utilities." },
			{ "Player", "Player abilities, health and movement." },
			{ "Weapons", "Weapons, ammo and combat modifiers." },
			{ "Teleport", "Travel the map instantly." },
			{ "Vehicles", "Current vehicle tools and godmode." },
			{ "Character Stats", "View and edit SP character statistics." },
			{ "SPACE TRAINER", "About SPACE TRAINER and links." },
			{ "SPACE", "About SPACE TRAINER and links." },
			{ "Object Spooner", "Place, edit and save world entities." },
			{ "Settings", "Language, binds and config." },
			{ "Creator", "Creator info, version and Telegram." },
			{ "Creator: xdigr", "Space trainer creator." },
			{ "Telegram: t.me/xdigr", "Open creator Telegram channel." },
			{ "Telegram", "Open creator Telegram channel." },
			{ "Creator Profile", "Open creator profile." },
			{ "Website", "Open creator website." },
			{ "GitHub", "Open GitHub page." },
			{ "Language", "Switch menu language (English / Russian)." },
			{ "Themes", "Load saved colour themes." },
			{ "Menu Colours", "Customize menu colours." },
			{ "Menu Fonts", "Customize menu fonts." },
			{ "Menu Position", "Move the menu on screen." },
			{ "Mouse Support", "Enable mouse navigation in the menu." },
			{ "Spawn Settings", "Auto-sit, upgrades and spawn flags." },
			{ "Favourites", "Your favourite vehicle models." },
			{ "Saved Vehicles", "Load vehicles saved as XML." },
			{ "Superman Punch", "Punch/kick knocks people and cars away." },
			{ "Invincibility", "Player cannot take damage." },
			{ "Never Wanted", "Police wanted level stays at zero." },
			{ "Give All Weapons", "Gives every weapon with ammo." },
			{ "Vehicle Spawner", "Open vehicle categories and spawn cars." },
			{ "Delete Vehicle", "Deletes the current vehicle." },
			{ "Repair Vehicle", "Fully repairs the current vehicle." },
			{ "Auto-Repair", "Keeps repairing the vehicle automatically." },
			{ "Seatbelt", "Prevents being ejected from the vehicle." },
			{ "Explosive Melee", "Melee hits cause explosions." },
			{ "Infinite Ammo In Clip", "Magazine never empties." },
			{ "Teleport Into Closest Vehicle", "Warps you into nearest free vehicle." },
			{ "Waypoint", "Teleport to your map waypoint." },
			{ "English", "Use English menu text." },
			{ "Russian", "Use Russian menu text." },
			{ "Reload Language Files", "Reload .json language files from disk." },
			{ "Titlebox Glare", "Optional glare effect on classic title box." },
			{ "Live Pulse", "Subtle animated accent pulse." },
		};

		auto it = tips.find(optionEnglish);
		if (it != tips.end())
			SetHoveredTip(it->second);
	}

	bool IsVehicleSpawnUnsafe(const GTAmodel::Model& model)
	{
		if (!model.IsValid() || !model.IsInCdImage() || !model.IsVehicle())
			return true;

		// Extra blacklist for stubs / console-era models that may still appear in lists.
		static const std::unordered_set<std::string> riskyNames
		{
			"astron", "astron2", "cyclone2", "ignus2", "entity3", "virtue",
			"panthere", "corsita", "greenwood", "baller7", "baller8", "issi8",
			"aleutian", "dorado", "impaler5", "impaler6", "cavalcade3", "polterminus",
			"terminus", "vivanite", "yosemite1500", "driftyosemite", "pipistrello",
			"coquette5", "envisage", "eurosx32", "niobe", "paragon3", "suzume",
			"banshee3", "buffalo5", "coureur", "dominator9", "fr36", "hardy",
			"driftnebula", "driftfuto", "driftjester", "driftsentinel", "driftcypher",
		};

		const std::string name = Lower(model.VehicleModelName());
		if (riskyNames.count(name))
			return true;

		if (!g_isEnhanced && GTAmemory::GetGameVersion() >= eGameVersion::VER_1_0_3095_0)
		{
			if (IS_VEHICLE_GEN9_EXCLUSIVE_MODEL(model.hash))
				return true;
		}
		return false;
	}

	std::string GetVehicleSpawnTip(const GTAmodel::Model& model)
	{
		if (IsVehicleSpawnUnsafe(model))
			return Language::TranslateToSelected("Crash risk: missing/unsupported model. Prefer another car.");
		return Language::TranslateToSelected("Safe to spawn.");
	}

	void DrawHoveredTipFooter()
	{
		if (Menu::currentsub == SUB::CLOSED || g_hoveredTip.empty())
			return;

		float temp = (float)Menu::totalop;
		if (temp > GTA_MAXOP) temp = GTA_MAXOP;
		const float scr_rect_Y = ((temp + 1.0f) * 0.035f) + 0.1570f;
		const float textY = scr_rect_Y - 0.0115f + menuPos.y;

		std::string tip = g_hoveredTip;
		if (tip.size() > 46)
			tip = tip.substr(0, 43) + "...";

		Game::Print::SetupDraw(0, Vector2(0.0f, 0.24f), false, false, false, optioncount);
		Game::Print::drawstring(tip, 0.058f + menuPos.x, textY);
	}
}
