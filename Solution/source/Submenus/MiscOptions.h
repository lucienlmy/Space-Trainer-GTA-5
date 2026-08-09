#pragma once

#include "..\macros.h"

#include "..\Menu\Menu.h"
#include "..\Menu\Routine.h"

#include "..\Natives\natives2.h"
#include "..\Util\StringManip.h"
#include "..\Util\keyboard.h"
#include "..\Util\GTAmath.h"
#include "..\Util\FileLogger.h"
#include "..\Scripting\Game.h"
#include "..\Scripting\World.h"
#include "..\Scripting\Camera.h"
#include "..\Scripting\TimecycleModification.h"
#include "..\Scripting\GTAentity.h"
#include "..\Scripting\GTAped.h"
#include "..\Scripting\GTAvehicle.h"
#include "..\Scripting\GameplayCamera.h"

#include "..\Misc\Gta2Cam.h"
#include "..\Misc\ManualRespawn.h"
#include "..\Misc\MeteorShower.h"
#include "..\Misc\JumpAroundMode.h"
#include "..\Misc\FpsCounter.h"

#include "..\Submenus\Spooner\EntityManagement.h"

#include <string>
#include <map>
#include <vector>
#include <algorithm>

typedef signed char INT8;
typedef unsigned char UINT8;
typedef unsigned short UINT16;

namespace sub
{
	void MiscOps();
	void Timecycles();
	void ClearAreaSub();
	void DrawToolsMenu();
	void RadioMenu();

	namespace WaterHack
	{
		void Tick();
		void Sub_WaterHack();
	}

	namespace TVChannelStuff
	{
		extern bool loopBasicTV;
		extern std::string currentTvChannelLabel;
		extern std::map<std::string, std::string> tvPlaylists;

		void DrawTvWhereItsSupposedToBe();
		void TVMenu();
	}

	namespace HudOptions
	{
		extern bool revealMinimap;
		void HudOptionsMenu();
	}

	namespace GameCamOptions
	{
		extern float shakeAmplitude;
		extern INT8 shakeID;

		void GameCamOptionsMenu();
	}
}