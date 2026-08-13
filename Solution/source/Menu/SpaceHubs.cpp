/*
* Space Trainer - hub menus
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
#include "..\Scripting\Camera.h"
#include "..\Scripting\World.h"
#include "..\Submenus\Bodyguards\BodyguardManagement.h"
#include "..\Misc\Gta2Cam.h"
#include "..\macros.h"

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
		AddOption("Objects / Creators", null, nullFunc, SUB::SPOONER_MAIN);
		AddOption("Object Gun", null, nullFunc, SUB::OBJECTSPAWNER_OBJS);
		AddOption("Companions / Clones", null, nullFunc, SUB::CLONECOMPANIONSUB);
		AddOption("Custom Skins", null, nullFunc, SUB::CUSTOMSKINS);
	}

	void WorldHub()
	{
		AddTitle("World");
		AddToggle("Less Peds", pedPopulation);
		AddToggle("Less Vehicles", vehiclePopulation);
		AddToggle("Blackout", simpleBlackoutMode);
		AddToggle("Restricted Areas", restrictedAreasAccess);
		AddOption("Clear Area", null, nullFunc, SUB::CLEARAREA);
		AddOption("Disasters", null, nullFunc, SUB::DISASTERS);
		AddOption("Zombie Apocalypse", null, nullFunc, SUB::ZOMBIE_APOCALYPSE);
		AddOption("Film Cameras", null, nullFunc, SUB::SPACE_FILMCAM);
		AddOption("Weather FX / Vision", null, nullFunc, SUB::TIMECYCLES);
		AddOption("HUD Options", null, nullFunc, SUB::HUDOPTIONS);
		AddOption("Cameras", null, nullFunc, SUB::GAMECAMOPTIONS);
		AddOption("PC Graphics", null, nullFunc, SUB::GRAPHICSQUALITY);
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
		AddOption("Scenarios", null, nullFunc, SUB::AnimationTaskScenarios);
		AddOption("Movement Styles", null, nullFunc, SUB::MOVEMENTGROUP);
		AddOption("Facial Moods", null, nullFunc, SUB::FACIALMOOD);
		AddOption("Speech Player", null, nullFunc, SUB::SPEECHPLAYER);
	}

	void FunctionsHub()
	{
		AddTitle("Functions");

		bool freecamOn = false, freecamOff = false;
		AddToggle("FreeCam", noClip, freecamOn, freecamOff);
		if (freecamOn)
		{
			// Stop other script cams so FreeCam owns the view
			if (GTA2Cam::g_gta2Cam.Enabled())
				GTA2Cam::g_gta2Cam.TurnOff();
			World::DestroyAllCameras();
			Camera::RenderScriptCams(false);
			noClipToggle = true;
			Game::Print::PrintBottomCentre("FreeCam ON. WASD + mouse. F3 toggles.");
		}
		if (freecamOff)
		{
			if (noClipToggle)
			{
				SetNoclipOff1();
				SetNoclipOff2();
			}
			noClipToggle = false;
		}
		AddToggle("Super Run", superRun);
		AddToggle("Super Jump", superJump);
		AddToggle("God Mode", playerInvincibility);
		AddToggle("Never Wanted", neverWanted);
		AddToggle("Infinite Ammo", bitInfiniteAmmo);

		AddBreak("---Modes---");
		AddOption("Chaos Modes", null, nullFunc, SUB::CHAOSMODES);
		AddOption("Contract Hits", null, nullFunc, SUB::HITMAN_CONTRACTS);
		AddOption("Disasters", null, nullFunc, SUB::DISASTERS);
		AddOption("Zombie Apocalypse", null, nullFunc, SUB::ZOMBIE_APOCALYPSE);
		AddOption("Professions", null, nullFunc, SUB::PROFESSIONS);
		AddOption("Tools Pack", null, nullFunc, SUB::SPACE_PROPACK);

		AddBreak("---More---");
		AddOption("Misc Options", null, nullFunc, SUB::MISCOPS);
		AddOption("Space Extras", null, nullFunc, SUB::SPACEEXTRAS);
		AddOption("Hero Abilities", null, nullFunc, SUB::HEROABILITIES);
		AddOption("Radio", null, nullFunc, SUB::RADIOSUB);
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

		AddToggle("Live Pulse", SpaceTheme::livePulse);
		AddToggle("Show Tips", SpaceTheme::showTips);
		AddToggle("Show FPS", FPSCounter::bDisplayFps);

		bool animPlus = false, animMinus = false;
		std::vector<std::string> animQ{ "Off", "Low", "Medium", "High" };
		AddTexter("UI Animation", (int)SpaceTheme::animQuality, animQ, null, animPlus, animMinus);
		if (animPlus && (int)SpaceTheme::animQuality < 3)
			SpaceTheme::animQuality = (SpaceTheme::AnimQuality)((int)SpaceTheme::animQuality + 1);
		if (animMinus && (int)SpaceTheme::animQuality > 0)
			SpaceTheme::animQuality = (SpaceTheme::AnimQuality)((int)SpaceTheme::animQuality - 1);

		AddOption("Custom Themes Folder", null, nullFunc, SUB::CUSTOM_THEMES);
		AddOption("Menu Position", null, nullFunc, SUB::SETTINGS_MENUPOS);

		bool saveTheme = false, reset = false;
		AddOption("Save As Custom Theme...", saveTheme);
		AddOption("Reset Appearance", reset);
		if (saveTheme)
		{
			std::string name = Game::InputBox("", 40U, "Theme name", "My Theme");
			if (!name.empty())
			{
				if (CustomThemes::SaveCurrentTheme(name, "xdigr"))
					Game::Print::PrintBottomLeft("~g~Saved to SpaceStuff\\Themes");
				else
					Game::Print::PrintBottomLeft("~r~Save failed.");
			}
		}
		if (reset)
		{
			SpaceTheme::accentPreset = SpaceTheme::AccentPreset::Cyan;
			SpaceTheme::uiOpacity = 0.82f;
			SpaceTheme::uiScale = 1.0f;
			SpaceTheme::textScale = 1.0f;
			SpaceTheme::uiBlur = 0.28f;
			SpaceTheme::animQuality = SpaceTheme::AnimQuality::High;
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

		char buf[96];
		sprintf_s(buf, "Health: %d", GET_ENTITY_HEALTH(me));
		AddOption(buf, null);
		sprintf_s(buf, "Armor: %d", GET_PED_ARMOUR(me));
		AddOption(buf, null);
		sprintf_s(buf, "Wanted: %d", GET_PLAYER_WANTED_LEVEL(pl));
		AddOption(buf, null);
		sprintf_s(buf, "Speed: %.0f km/h", speed);
		AddOption(buf, null);
		sprintf_s(buf, "XYZ: %.1f  %.1f  %.1f", pos.x, pos.y, pos.z);
		AddOption(buf, null);
		sprintf_s(buf, "Bodyguards: %u / 7", (unsigned)sub::BodyguardMenu::BodyguardDb.size());
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
