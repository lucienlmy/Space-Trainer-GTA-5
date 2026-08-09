#pragma once

#include "..\macros.h"

#include "..\Menu\Menu.h"

#include "..\Natives\natives2.h"
#include "..\Scripting\GTAplayer.h"
#include "..\Scripting\Game.h"
#include "..\Util\ExePath.h"
#include "..\Util\FileLogger.h"

#include <Windows.h>
#include <string>
#include <vector>

typedef unsigned long DWORD;
typedef char *PCHAR;

namespace sub
{
	namespace CutscenePlayer
	{
		void PopulateCutsceneLabels();
		void EndCutscene();
		void CutsceneListMenu();
	}
}