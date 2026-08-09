#pragma once

#include "..\macros.h"

#include "..\Menu\Menu.h"

#include "..\Natives\natives2.h"
#include "..\Util\GTAmath.h"
#include "..\Util\ExePath.h"
#include "..\Scripting\GTAentity.h"
#include "..\Scripting\GTAprop.h"
#include "..\Scripting\GTAped.h"
#include "..\Scripting\Game.h"
#include "..\Scripting\Model.h"
#include "..\Scripting\World.h"

#include "..\Submenus\Teleport\TeleMethods.h"

#include <pugixml\src\pugixml.hpp>
#include <string>
#include <vector>

typedef int Object;
typedef unsigned long DWORD, Hash;

namespace sub
{
	namespace MapMods
	{
		void UnloadAllMapMods();
		void MapMods();
		void MapMods2();
	}
}