#pragma once

#include "GenericLoopedMode.h"

class GTAvehicle;

namespace VehicleTow
{
	class VehicleTow final : public GenericLoopedMode
	{
	private:
	public:
		void TurnOn() override;
		void TurnOff() override;

		// run on tick
		void Tick() override;
		inline void Tick2();
		inline void DoTowTick();
		inline void DoTowMaintainTick();

		// get vehicle behind first
		GTAvehicle GetVehicleBehindFirst();

		void ExtendTow(GTAvehicle& newVeh);
		void ShortenTow();
		void EndTows();

		// controls
		bool ExtendPressed();
		bool ShortenPressed();

		// help text
		void DisplayHelpText_Extend(GTAvehicle& vehBehind);

	};


	extern VehicleTow g_vehicleTow;

	void ToggleOnOff();
}


