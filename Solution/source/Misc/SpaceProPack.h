/*
* Space Trainer - Tools pack
* Smart vehicle cleanup, mod-safe mode, film cams,
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
	void FilmCamMenu();
	void GangsMenu();
	void TrafficMenu();
	void VehicleCleanMenu();
}
