/*
* Space Trainer - Pro tools pack
* Smart vehicle cleanup, mod-safe mode, quick search, film cams,
* tuning shortcuts, custom gangs, realistic traffic.
*/
#pragma once

#include "..\Natives\types.h"

namespace SpaceProPack
{
	void TrackSpawnedVehicle(Vehicle veh);
	bool IsSafeMode();
	void Tick();
	void Menu();
	void QuickSearchMenu();
	void FilmCamMenu();
	void GangsMenu();
	void TrafficMenu();
	void VehicleCleanMenu();
}
