#include "submenu_switch.h"
#include "..\macros.h"
#include "Menu.h"

std::unordered_map<int, SubmenuHandler>& SubMenuRegistry::GetMap()
{
	static std::unordered_map<int, SubmenuHandler> map;
	return map;
}

void SubMenuRegistry::Register(int sub, SubmenuHandler handler)
{
	GetMap()[sub] = handler;
}

void SubMenuRegistry::Dispatch(int sub)
{
	auto& map = GetMap();
	auto it = map.find(sub);
	if (it != map.end())
	{
		it->second();
	}
}

bool SubMenuRegistry::IsRegistered(int sub)
{
	auto& map = GetMap();
	return map.find(sub) != map.end();
}

void Menu::submenu_switch()
{
	SubMenuRegistry::Dispatch(currentsub);
}
