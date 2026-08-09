#pragma once

#include <vector>

#include "..\macros.h"

#include "..\Menu\Menu.h"
#include "..\Menu\Routine.h"

#include "..\Natives\natives2.h"
#include "..\Util\GTAmath.h"
#include "..\Scripting\DxHookIMG.h"
#include "..\Scripting\Game.h"
#include "..\Util\ExePath.h"

#include <string>
#include <time.h>
#include <stdio.h>

typedef unsigned __int8 UINT8;

class Vector2;

namespace sub
{
	void TimeMenu();

	namespace Clock
	{
		extern UINT8 loopClock;
		extern UINT8 analogueClockIndex;
		extern Vector2 analogueClockPosition;

		inline void DisplayClockDigital();

		struct ClockImage 
		{ 
			std::string name; 
			DxHookIMG::DxTexture faceId;
			DxHookIMG::DxTexture hourId;
			DxHookIMG::DxTexture minuteId; 
		};

		extern std::vector<ClockImage> clockImages;

		void LoadClockImages();
		inline void DisplayClockAnalogue();

		void DisplayClock();

		void ClockMenu();
	}

}




