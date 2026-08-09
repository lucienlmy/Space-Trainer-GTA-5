/*
* Space Trainer - graphics quality presets for weak / strong PCs
*/
#include "GraphicsQuality.h"

#include "..\Menu\Menu.h"
#include "..\Menu\submenu_enum.h"
#include "..\Menu\submenu_switch.h"
#include "..\Natives\natives2.h"
#include "..\Scripting\Game.h"

namespace GraphicsQuality
{
	int currentPreset = Off;

	void Apply(int preset)
	{
		currentPreset = preset;
		switch (preset)
		{
		case Weak:
			Game::Print::PrintBottomLeft("~b~Graphics:~s~ Weak PC");
			break;
		case Medium:
			Game::Print::PrintBottomLeft("~b~Graphics:~s~ Medium");
			break;
		case High:
			Game::Print::PrintBottomLeft("~b~Graphics:~s~ High");
			break;
		case VeryHigh:
			Game::Print::PrintBottomLeft("~b~Graphics:~s~ Very High");
			break;
		default:
			Game::Print::PrintBottomLeft("~b~Graphics:~s~ Off (game default)");
			break;
		}
	}

	void Tick()
	{
		if (currentPreset == Off)
			return;

		float lod = 1.0f;
		float pedDens = 1.0f;
		float vehDens = 1.0f;
		float parked = 1.0f;
		float shadowScale = 1.0f;
		float flashFade = 40.0f;
		bool entityTracker = true;

		switch (currentPreset)
		{
		case Weak:
			lod = 0.45f;
			pedDens = 0.25f;
			vehDens = 0.20f;
			parked = 0.10f;
			shadowScale = 0.15f;
			flashFade = 8.0f;
			entityTracker = false;
			break;
		case Medium:
			lod = 0.70f;
			pedDens = 0.55f;
			vehDens = 0.50f;
			parked = 0.35f;
			shadowScale = 0.45f;
			flashFade = 18.0f;
			entityTracker = true;
			break;
		case High:
			lod = 1.0f;
			pedDens = 0.85f;
			vehDens = 0.85f;
			parked = 0.75f;
			shadowScale = 0.80f;
			flashFade = 35.0f;
			entityTracker = true;
			break;
		case VeryHigh:
			lod = 1.35f;
			pedDens = 1.0f;
			vehDens = 1.0f;
			parked = 1.0f;
			shadowScale = 1.0f;
			flashFade = 50.0f;
			entityTracker = true;
			break;
		default:
			return;
		}

		OVERRIDE_LODSCALE_THIS_FRAME(lod);
		SET_PED_DENSITY_MULTIPLIER_THIS_FRAME(pedDens);
		SET_SCENARIO_PED_DENSITY_MULTIPLIER_THIS_FRAME(pedDens, pedDens);
		SET_VEHICLE_DENSITY_MULTIPLIER_THIS_FRAME(vehDens);
		SET_RANDOM_VEHICLE_DENSITY_MULTIPLIER_THIS_FRAME(vehDens);
		SET_PARKED_VEHICLE_DENSITY_MULTIPLIER_THIS_FRAME(parked);
		CASCADE_SHADOWS_SET_CASCADE_BOUNDS_SCALE(shadowScale);
		CASCADE_SHADOWS_ENABLE_ENTITY_TRACKER(entityTracker);
		SET_FLASH_LIGHT_FADE_DISTANCE(flashFade);
		SET_DISTANCE_BLUR_STRENGTH_OVERRIDE(currentPreset == Weak ? 0.0f : -1.0f);

		if (currentPreset == Weak)
			DISABLE_OCCLUSION_THIS_FRAME();
	}

	void Menu()
	{
		AddTitle("PC Graphics");

		bool off = false, weak = false, med = false, high = false, vhigh = false;
		AddTickol("Off (Default)", currentPreset == Off, off, off, TICKOL::BOXTICK, TICKOL::BOXBLANK);
		AddTickol("Weak PC", currentPreset == Weak, weak, weak, TICKOL::BOXTICK, TICKOL::BOXBLANK);
		AddTickol("Medium", currentPreset == Medium, med, med, TICKOL::BOXTICK, TICKOL::BOXBLANK);
		AddTickol("High", currentPreset == High, high, high, TICKOL::BOXTICK, TICKOL::BOXBLANK);
		AddTickol("Very High", currentPreset == VeryHigh, vhigh, vhigh, TICKOL::BOXTICK, TICKOL::BOXBLANK);

		if (off) Apply(Off);
		if (weak) Apply(Weak);
		if (med) Apply(Medium);
		if (high) Apply(High);
		if (vhigh) Apply(VeryHigh);

		AddBreak("---Info---");
		AddOption("Lowers LOD, traffic, shadows for FPS", null);
	}
}

REGISTER_SUBMENU(GRAPHICSQUALITY, GraphicsQuality::Menu)
