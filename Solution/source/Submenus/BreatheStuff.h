#pragma once

#include "..\macros.h"

#include "..\Menu\Menu.h"
#include "..\Menu\Routine.h"

#include "..\Natives\natives2.h"
#include "..\Scripting\PTFX.h"
#include "..\Util\keyboard.h"
#include "..\Scripting\enums.h"
#include "..\Scripting\GTAped.h"
#include "..\Scripting\Model.h"
#include "..\Scripting\Game.h"

#include <string>
#include <vector>
#include <utility>

typedef unsigned __int8 UINT8;

namespace PTFX 
{
	class LoopedPTFX;
}

namespace sub
{
	namespace BreatheStuff
	{
		enum class BreathePtfxType : UINT8 { None, Blood, Fire, };
		extern const std::vector<std::pair<std::string, BreathePtfxType>> captionsBreatheStuff;
		extern PTFX::LoopedPTFX g_breatheStuffPTFX;
		extern BreathePtfxType playerBreatheStuff;

		void SetSelfBreathePTFX(const BreathePtfxType& type);
		void AddOption(const std::string& text, const BreathePtfxType& type);
		void BreatheStuffMenu();

	}

}




