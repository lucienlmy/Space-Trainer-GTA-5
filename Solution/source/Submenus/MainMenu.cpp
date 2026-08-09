#include "MainMenu.h"
#include "..\Menu\SpaceMenuData.h"
#include "..\Menu\MenuTips.h"

namespace sub
{
	void MainMenu()
	{
		static bool firstLoad = true;
		if (firstLoad)
		{
			firstLoad = false;
			addlog(ige::LogType::LOG_TRACE, "First Load of MainMenu Submenu");
		}

		AddTitle("MAIN");

		for (const auto& cat : SpaceMenuData::MainCategories())
		{
			bool pressed = false;
			AddOption(cat.title, pressed, nullFunc, cat.submenu);
			MenuTips::SetHoveredTip(cat.description);
		}

		g_Ped2 = PLAYER_ID();
		g_Ped1 = PLAYER_PED_ID();
		g_PlayerName = GET_PLAYER_NAME(g_Ped2);
		g_Ped3 = GET_PLAYER_GROUP(g_Ped2);
	}
}


#include "..\Menu\submenu_switch.h"
#include "..\Menu\submenu_enum.h"
REGISTER_SUBMENU(MAINMENU, sub::MainMenu)
