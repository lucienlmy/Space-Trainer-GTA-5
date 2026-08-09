#pragma once

#include "GenericLoopedMode.h"

namespace VehicleCruise
{
	class VehicleCruise final : public GenericLoopedMode
	{
	private:
		float cruiseSpeed;
	public:
		VehicleCruise();

		void TurnOn() override;
		void TurnOff() override;

		void BeginCruise();
		void EndCruise();

		void Tick() override;
		void DoCruiseTick();

		void PrintCruiseText(bool working);

	};


	extern VehicleCruise g_vehicleCruise;

	void ToggleOnOff();
}



