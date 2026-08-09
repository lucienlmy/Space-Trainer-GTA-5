#include "Credits.h"
#include "MainMenu.h"

#include "..\macros.h"
#include "..\Menu\Menu.h"
#include "..\Menu\Language.h"
#include "..\Menu\submenu_switch.h"
#include "..\Menu\submenu_enum.h"

#include <windows.h>
#include <shellapi.h>

#pragma comment(lib, "shell32.lib")

namespace
{
	void OpenUrl(const char* url)
	{
		ShellExecuteA(nullptr, "open", url, nullptr, nullptr, SW_SHOWNORMAL);
	}
}

namespace sub
{
	void CreditsMenu()
	{
		AddTitle("SPACE");

		AddBreak("---SPACE Trainer---");
		AddOption(std::string("Version: ") + SPACE_CURRENT_VER_, null);

		AddBreak("---Created by---");
		AddOption("xdigr", null);

		bool openCreator = false, openTg = false, openWeb = false, openGh = false;
		AddOption("Creator Profile", openCreator, nullFunc, -1, true);
		AddOption("Telegram", openTg, nullFunc, -1, true);
		AddOption("Website", openWeb, nullFunc, -1, true);
		AddOption("GitHub", openGh, nullFunc, -1, true);

		if (openCreator) OpenUrl("https://t.me/xdigr");
		if (openTg) OpenUrl("https://t.me/xdigr");
		if (openWeb) OpenUrl("https://t.me/xdigr");
		if (openGh) OpenUrl("https://github.com/");
	}

	void SpaceAboutMenu()
	{
		CreditsMenu();
	}
}

REGISTER_SUBMENU(CREDITSSUB, sub::CreditsMenu)
REGISTER_SUBMENU(SPACE_ABOUT, sub::SpaceAboutMenu)
