/*
* Space Trainer - hub menus (World / Spawner / Appearance)
*/
#include "..\Menu\Menu.h"
#include "..\Menu\submenu_enum.h"
#include "..\Menu\submenu_switch.h"
#include "..\Menu\SpaceTheme.h"
#include "..\Menu\CustomThemes.h"
#include "..\Menu\Language.h"
#include "..\Menu\MenuConfig.h"
#include "..\Misc\FpsCounter.h"
#include "..\Menu\Routine.h"
#include "..\Natives\natives2.h"
#include "..\Scripting\Game.h"

#include <vector>
#include <string>
#include <cstdio>

namespace sub
{
	void SpawnerHub()
	{
		AddTitle("Spawner");
		AddOption("Vehicles", null, nullFunc, SUB::SPAWNVEHICLE);
		AddOption("Peds / Models", null, nullFunc, SUB::MODELCHANGER);
		AddOption("Animals", null, nullFunc, SUB::MODELCHANGER_ANIMAL);
		AddOption("Objects", null, nullFunc, SUB::SPOONER_SPAWN_PROP);
		AddOption("Creators", null, nullFunc, SUB::SPOONER_MAIN);
		AddOption("Object Gun", null, nullFunc, SUB::OBJECTSPAWNER_OBJS);
		AddOption("Companions / Clones", null, nullFunc, SUB::CLONECOMPANIONSUB);
		AddOption("Custom Skins & Heroes", null, nullFunc, SUB::CUSTOMSKINS);
		AddOption("Bodyguards", null, nullFunc, SUB::BODYGUARDMAINMENU);
	}

	void WorldHub()
	{
		AddTitle("World");
		AddToggle("Decreased Ped Population", pedPopulation);
		AddToggle("Decreased Vehicle Population", vehiclePopulation);
		AddToggle("Clear Weapon Pickups", clearWeaponPickups);
		AddToggle("EMP / Blackout", blackoutMode);
		AddToggle("Simple Blackout", simpleBlackoutMode);
		AddToggle("Massacre Mode", massacreMode);
		AddToggle("Restricted Area Access", restrictedAreasAccess);
		AddOption("Clear Area", null, nullFunc, SUB::CLEARAREA);
		AddOption("Vision / Timecycle", null, nullFunc, SUB::TIMECYCLES);
		AddOption("PC Graphics", null, nullFunc, SUB::GRAPHICSQUALITY);
		AddOption("Map Mods (Old)", null, nullFunc, SUB::MAPMODS);
		AddOption("HUD Options", null, nullFunc, SUB::HUDOPTIONS);
		AddOption("Game Camera Options", null, nullFunc, SUB::GAMECAMOPTIONS);
		AddOption("Space Extras", null, nullFunc, SUB::SPACEEXTRAS);
		AddOption("Creators", null, nullFunc, SUB::SPOONER_MAIN);
	}

	void NpcHub()
	{
		AddTitle("NPC");
		AddOption("Spawn Ped / Model", null, nullFunc, SUB::MODELCHANGER);
		AddOption("Animals", null, nullFunc, SUB::MODELCHANGER_ANIMAL);
		AddOption("Companions / Clones", null, nullFunc, SUB::CLONECOMPANIONSUB);
		AddOption("Bodyguards", null, nullFunc, SUB::BODYGUARDMAINMENU);
		AddBreak("---Animations---");
		AddOption("Animations", null, nullFunc, SUB::ANIMATIONSUB);
		AddOption("All Ped Animations", null, nullFunc, SUB::ANIMATIONSUB_ALLPEDANIMS);
		AddOption("Scenarios", null, nullFunc, SUB::AnimationTaskScenarios);
		AddOption("Movement Styles", null, nullFunc, SUB::MOVEMENTGROUP);
		AddOption("Facial Moods", null, nullFunc, SUB::FACIALMOOD);
		AddOption("Speech Player", null, nullFunc, SUB::SPEECHPLAYER);
		AddToggle("Decreased Ped Population", pedPopulation);
		AddOption("Clear Area (Peds)", null, nullFunc, SUB::CLEARAREA);
	}

	void FunctionsHub()
	{
		AddTitle("Functions");

		bool freecamOff = false;
		AddToggle("FreeCam (No-Clip)", noClip, null, freecamOff);
		AddToggle("Super Run", superRun);
		AddToggle("Super Jump", superJump);
		AddToggle("God Mode", playerInvincibility);
		AddToggle("No Ragdoll", playerNoRagdoll);
		AddToggle("Never Wanted", neverWanted);
		AddToggle("Show Coordinates", bDisplayXyzhCoords);
		AddToggle("Show FPS", FPSCounter::bDisplayFps);
		AddToggle("Infinite Ammo", bitInfiniteAmmo);
		AddToggle("Explosive Melee", explosiveMelee);
		AddToggle("Rapid Fire", rapidFire);
		AddToggle("Drive On Water", driveOnWater);

		AddBreak("---Modules---");
		AddOption("Misc Options", null, nullFunc, SUB::MISCOPS);
		AddOption("Space Extras", null, nullFunc, SUB::SPACEEXTRAS);
		AddOption("Hero Abilities", null, nullFunc, SUB::HEROABILITIES);
		AddOption("Custom Skins & Heroes", null, nullFunc, SUB::CUSTOMSKINS);
		AddOption("Creators / Spooner", null, nullFunc, SUB::SPOONER_MAIN);
		AddOption("Cutscene Player", null, nullFunc, SUB::CUTSCENEPLAYER);
		AddOption("TV Player", null, nullFunc, SUB::TVCHANNELSTUFF_TV);
		AddOption("Radio", null, nullFunc, SUB::RADIOSUB);
		AddOption("Animal Riding (SP)", null, nullFunc, SUB::ANIMALRIDING);
	}

	void AppearanceHub()
	{
		AddTitle("Appearance");

		bool themePlus = false, themeMinus = false;
		std::vector<std::string> themes{
			"SPACE BLUE", "SPACE PURPLE", "SPACE RED",
			"SPACE GREEN", "SPACE WHITE", "SPACE CYAN"
		};
		AddTexter("Theme", (int)SpaceTheme::accentPreset, themes, null, themePlus, themeMinus);
		if (themePlus) SpaceTheme::CycleAccent(1);
		if (themeMinus) SpaceTheme::CycleAccent(-1);

		bool opPlus = false, opMinus = false;
		int opacityPct = (int)(SpaceTheme::uiOpacity * 100.0f + 0.5f);
		AddNumber("Transparency %", opacityPct, 0, null, opPlus, opMinus);
		if (opPlus && opacityPct < 100) { opacityPct += 5; SpaceTheme::uiOpacity = opacityPct / 100.0f; SpaceTheme::ApplyDefaultsToLegacyColors(); }
		if (opMinus && opacityPct > 35) { opacityPct -= 5; SpaceTheme::uiOpacity = opacityPct / 100.0f; SpaceTheme::ApplyDefaultsToLegacyColors(); }

		bool scPlus = false, scMinus = false;
		int scalePct = (int)(SpaceTheme::uiScale * 100.0f + 0.5f);
		AddNumber("Menu Scale %", scalePct, 0, null, scPlus, scMinus);
		if (scPlus && scalePct < 120) { scalePct += 5; SpaceTheme::uiScale = scalePct / 100.0f; }
		if (scMinus && scalePct > 80) { scalePct -= 5; SpaceTheme::uiScale = scalePct / 100.0f; }

		bool txPlus = false, txMinus = false;
		int textPct = (int)(SpaceTheme::textScale * 100.0f + 0.5f);
		AddNumber("Text Scale %", textPct, 0, null, txPlus, txMinus);
		if (txPlus && textPct < 130) { textPct += 5; SpaceTheme::textScale = textPct / 100.0f; }
		if (txMinus && textPct > 70) { textPct -= 5; SpaceTheme::textScale = textPct / 100.0f; }

		bool blPlus = false, blMinus = false;
		int blurPct = (int)(SpaceTheme::uiBlur * 100.0f + 0.5f);
		AddNumber("Blur Intensity %", blurPct, 0, null, blPlus, blMinus);
		if (blPlus && blurPct < 100) { blurPct += 5; SpaceTheme::uiBlur = blurPct / 100.0f; }
		if (blMinus && blurPct > 0) { blurPct -= 5; SpaceTheme::uiBlur = blurPct / 100.0f; }

		bool anPlus = false, anMinus = false;
		std::vector<std::string> anims{ "Off", "Low", "Medium", "High" };
		AddTexter("Animations", (int)SpaceTheme::animQuality, anims, null, anPlus, anMinus);
		if (anPlus && (int)SpaceTheme::animQuality < 3) SpaceTheme::animQuality = (SpaceTheme::AnimQuality)((int)SpaceTheme::animQuality + 1);
		if (anMinus && (int)SpaceTheme::animQuality > 0) SpaceTheme::animQuality = (SpaceTheme::AnimQuality)((int)SpaceTheme::animQuality - 1);

		AddToggle("Live Pulse", SpaceTheme::livePulse);
		AddToggle("Show Tips", SpaceTheme::showTips);
		AddToggle("Row Dividers", SpaceTheme::showRowDividers);
		AddToggle("Show FPS", FPSCounter::bDisplayFps);

		AddOption("Custom Themes Folder", null, nullFunc, SUB::CUSTOM_THEMES);
		AddOption("Menu Position", null, nullFunc, SUB::SETTINGS_MENUPOS);
		AddOption("Menu Colours (Advanced)", null, nullFunc, SUB::SETTINGS_COLOURS);

		bool reset = false;
		bool saveTheme = false;
		AddOption("Save As Custom Theme...", saveTheme);
		AddOption("Reset Appearance", reset);
		if (saveTheme)
		{
			std::string name = Game::InputBox("", 40U, "Theme name", "My Theme");
			if (!name.empty())
			{
				std::string author = Game::InputBox("", 30U, "Author", "xdigr");
				if (CustomThemes::SaveCurrentTheme(name, author.empty() ? "xdigr" : author))
					Game::Print::PrintBottomLeft("~g~Saved to SpaceStuff\\Themes");
				else
					Game::Print::PrintBottomLeft("~r~Save failed.");
			}
		}
		if (reset)
		{
			SpaceTheme::SetNamedTheme(SpaceTheme::AccentPreset::Cyan);
			SpaceTheme::uiOpacity = 0.78f;
			SpaceTheme::uiScale = 1.0f;
			SpaceTheme::textScale = 1.0f;
			SpaceTheme::uiBlur = 0.22f;
			SpaceTheme::animQuality = SpaceTheme::AnimQuality::Medium;
			SpaceTheme::livePulse = true;
			SpaceTheme::showTips = true;
			SpaceTheme::showRowDividers = true;
			SpaceTheme::ApplyDefaultsToLegacyColors();
			Game::Print::PrintBottomLeft("Appearance reset.");
		}
	}

	void StatisticsHub()
	{
		AddTitle("Statistics");

		Ped me = PLAYER_PED_ID();
		Player pl = PLAYER_ID();
		Vector3 pos = GET_ENTITY_COORDS(me, true);
		Vector3 vel = GET_ENTITY_VELOCITY(me);
		float speed = SYSTEM::VMAG(vel.x, vel.y, vel.z) * 3.6f;
		int wanted = GET_PLAYER_WANTED_LEVEL(pl);
		int hp = GET_ENTITY_HEALTH(me);
		int armor = GET_PED_ARMOUR(me);
		int hour = GET_CLOCK_HOURS();
		int minute = GET_CLOCK_MINUTES();

		char buf[96];
		sprintf_s(buf, "Health: %d", hp);
		AddOption(buf, null);
		sprintf_s(buf, "Armor: %d", armor);
		AddOption(buf, null);
		sprintf_s(buf, "Wanted: %d", wanted);
		AddOption(buf, null);
		sprintf_s(buf, "Speed: %.0f km/h", speed);
		AddOption(buf, null);
		sprintf_s(buf, "XYZ: %.1f  %.1f  %.1f", pos.x, pos.y, pos.z);
		AddOption(buf, null);
		sprintf_s(buf, "Time: %02d:%02d", hour, minute);
		AddOption(buf, null);
		sprintf_s(buf, "FPS overlay: %s", FPSCounter::bDisplayFps ? "ON" : "OFF");
		AddOption(buf, null);

		bool openStats = false, toggleFps = false;
		AddOption("Character Stats Editor", openStats, nullFunc, SUB::SPSTATMANAGER);
		AddOption("Toggle FPS Overlay", toggleFps);
		if (toggleFps) FPSCounter::bDisplayFps = !FPSCounter::bDisplayFps;
	}

	void SpaceUiSettings()
	{
		AppearanceHub();
	}
}

REGISTER_SUBMENU(SPAWNER_HUB, sub::SpawnerHub)
REGISTER_SUBMENU(WORLD_HUB, sub::WorldHub)
REGISTER_SUBMENU(NPC_HUB, sub::NpcHub)
REGISTER_SUBMENU(FUNCTIONS_HUB, sub::FunctionsHub)
REGISTER_SUBMENU(APPEARANCE_HUB, sub::AppearanceHub)
REGISTER_SUBMENU(STATISTICS_HUB, sub::StatisticsHub)
REGISTER_SUBMENU(SPACE_UISETTINGS, sub::SpaceUiSettings)
