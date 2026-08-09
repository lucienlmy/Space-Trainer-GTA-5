#pragma once

typedef unsigned __int64 DWORD64;


namespace sub::TeleportLocations_catind
{
	class NamedTeleLocationList;

	extern const NamedTeleLocationList* _selectedCategory;

	//extern Vector3 _customTeleLoc;

	namespace Submenus
	{
		void Sub_TeleportMain();
		void Sub_CustomCoords();
		void Sub_SelectedCategory();
		void Sub_BlipList();
		void Sub_SavedLocations();

	}

}



