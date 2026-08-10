#include "BodyguardSettings.h"
#include "BodyguardManagement.h"
#include "../../Menu/Menu.h"
#include "../../Natives/natives.h"
#include "../../Natives/natives2.h"
#include "../../Util/keyboard.h"
#include "../../Scripting/Game.h"
#include "BodyguardMenu.h"
#include "../../Scripting/Model.h"
#include "BodyguardSpawn.h"

#include <algorithm>
#include <cstdio>
#include <string>

namespace sub::BodyguardMenu
{
	BodyguardEntity* SelectedBodyguard = nullptr;

	static void PruneMissingBodyguards()
	{
		// Only drop entries whose handle is clearly gone (0 or not in world).
		BodyguardDb.erase(
			std::remove_if(BodyguardDb.begin(), BodyguardDb.end(),
				[](const BodyguardEntity& e)
				{
					const int h = e.Handle.GetHandle();
					return h == 0 || !DOES_ENTITY_EXIST(h);
				}),
			BodyguardDb.end());
	}

	void BodyguardList()
	{
		AddTitle("Bodyguard List");
		PruneMissingBodyguards();

		if (BodyguardDb.empty())
		{
			AddOption("No bodyguards spawned");
			AddOption("Hint: use Quick Spawn or Spawn Bodyguard", null);
			return;
		}

		char countBuf[48];
		sprintf_s(countBuf, "Active: %u / 7", (unsigned)BodyguardDb.size());
		AddBreak(countBuf);

		BodyguardEntity* pBodyguardToDelete = nullptr;

		for (UINT i = 0; i < BodyguardDb.size(); i++)
		{
			auto& bg = BodyguardDb[i];
			if (!bg.Handle.Exists())
				continue;

			bool bPressed = false;
			Ped ped = bg.Handle.GetHandle();
			int hp = ENTITY::GET_ENTITY_HEALTH(ped);
			std::string label = !bg.Name.empty() ? bg.Name : bg.HashName;
			label += " [" + std::to_string(hp) + " HP]";

			AddOption(label, bPressed, nullFunc, SUB::BODYGUARD_ENTITYOPS);
			if (bPressed)
				SelectedBodyguard = &bg;

			if (*Menu::currentopATM == Menu::printingop)
			{
				sub::BodyguardMenu::BodyguardManagement::ShowArrowAboveEntity(bg.Handle);

				bool bDeletePressed = false;
				if (Menu::bitController)
				{
					Menu::add_IB(INPUT_SCRIPT_RLEFT, "Delete Bodyguard");
					bDeletePressed = IS_DISABLED_CONTROL_JUST_PRESSED(2, INPUT_SCRIPT_RLEFT) != 0;
				}
				else
				{
					Menu::add_IB(VirtualKey::B, "Delete Bodyguard");
					bDeletePressed = IsKeyJustUp(VirtualKey::B);
				}
				if (bDeletePressed)
					pBodyguardToDelete = &bg;
			}
		}

		if (pBodyguardToDelete)
			sub::BodyguardMenu::BodyguardManagement::DeleteBodyguard(*pBodyguardToDelete);
	}

	void BodyguardOps_()
	{
		AddTitle("Bodyguard Settings");
	}
}

#include "..\..\Menu\submenu_switch.h"
#include "..\..\Menu\submenu_enum.h"
REGISTER_SUBMENU(BODYGUARD_LIST,        sub::BodyguardMenu::BodyguardList)
REGISTER_SUBMENU(BODYGUARD_SETTINGS,    sub::BodyguardMenu::BodyguardOps_)
