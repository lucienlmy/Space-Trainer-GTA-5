#pragma once

#include "GenericLoopedMode.h"

typedef unsigned long Hash;

namespace MagnetGun
{
	class MagnetGun final : public GenericLoopedMode
	{
	private:
		bool bGunActive;
		Hash _whash;
		float distanceFromCam;
	public:
		MagnetGun();

		void TurnOn() override;

		void Tick() override;
		inline void DoMagnetGunTick();
		void DisplayMagnetGunHelp();

	};


	extern MagnetGun g_magnetGun;
	void ToggleOnOff();
}




