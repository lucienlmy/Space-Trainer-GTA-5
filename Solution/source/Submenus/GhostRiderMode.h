#pragma once

#include <string>

#include "..\macros.h"
#include "..\Menu\Menu.h"

#include "..\Natives\natives2.h"
#include "..\Util\GTAmath.h"
#include "..\Util\ExePath.h"
#include "..\Misc\GenericLoopedMode.h"
#include "..\Scripting\GTAped.h"
#include "..\Scripting\GTAvehicle.h"
#include "..\Scripting\Model.h"
#include "..\Scripting\PTFX.h"
#include "..\Scripting\Game.h"

#include "..\Submenus\PedComponentChanger.h"
#include "..\Submenus\VehicleSpawner.h"

typedef char *PCHAR;

namespace sub
{
	namespace GhostRiderMode
	{
		void ToggleOnOff();
		void Tick();

		extern std::string outfitFileName;
		void ApplyGhostRiderOutfit();
		void SpawnGhostRiderRide();
		void Sub_GhostRiderMode();
	}
}