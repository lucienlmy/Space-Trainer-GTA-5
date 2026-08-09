#pragma once

#include "..\Scripting\GTAentity.h"

#include <string>
#include <vector>

#include "..\macros.h"

#include "..\Menu\Menu.h"
#include "..\Menu\Routine.h"

#include "..\Natives\natives2.h"

#include "..\Submenus\WeaponOptions.h"
#include "..\Util\keyboard.h"

#include "..\Scripting\Game.h"

#include "..\Util\ExePath.h"

#include <windows.h>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <json\single_include\nlohmann\json.hpp>

namespace sub
{
	namespace PtfxSubs
	{
		extern int ITEMS_PER_PAGE;
		struct PtfxS
		{
			std::string name;
			std::string asset;
			std::string fx;
		};

		inline bool operator==(const PtfxS& a, const PtfxS& b) 
		{
			return a.name == b.name && a.asset == b.asset && a.fx == b.fx;
		}

		extern const std::vector<PtfxS> PTFX;

		typedef struct 
		{ 
			GTAentity entity; 
			std::string asset;
			std::string fx; 
		} PtfxlopS;

		extern std::vector<PtfxlopS> fxLoops;
		void AddEntityToPtfxLops(const PtfxS& effect, const GTAentity& entity);
		void AddOptionEntity(const PtfxS& effect, GTAentity entity);
		void AddOptionLoopOnEntity(const PtfxS& effect, GTAentity entity);
		void PTFXFavourites();
		void PTFXSub();

	}

}




