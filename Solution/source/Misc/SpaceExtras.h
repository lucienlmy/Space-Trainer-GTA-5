/*
* Space Trainer exclusive extras
*/
#pragma once

namespace SpaceExtras
{
	extern bool slowMotion;
	extern bool softLanding;
	extern bool freezeNearby;
	extern bool attractPickups;
	extern bool vehicleHover;
	extern bool cleanAura;
	extern bool megaJump;
	extern bool infiniteStamina;
	extern bool quietSteps;
	extern bool forceField;
	extern bool autoFlipCar;
	extern bool nightVision;
	extern bool thermalVision;
	extern bool walkOnWater;
	extern bool tinyPlayer;
	extern bool explosiveMeleeQuick;
	extern bool oneHitKill;
	extern bool magnetPickups;

	void Tick();
	void Menu();
	void EnsureSpaceStuffFolders();
}
