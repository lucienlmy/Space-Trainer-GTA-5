#pragma once

#include "GenericLoopedMode.h"

namespace VehicleFly
{
	class VehicleFly final : public GenericLoopedMode
	{
	private:
	public:
		void TurnOn() override;
		void GoUp(float const& control);
		void GoDown(float const& control);
		void GoForward(float const& control);
		void GoBackward(float const& control);
		void GoRight(float const& control);
		void GoLeft(float const& control);

		float Pressed_GoUp();
		float Pressed_GoDown();
		float Pressed_GoForward();
		float Pressed_GoBackward();
		float Pressed_GoRight();
		float Pressed_GoLeft();

		void DisableDrivingControls();

		void Tick() override;
		inline void DoVehicleFlyTick();

		void PrintFlyInstructions();
	};


	extern VehicleFly g_vehicleFly;

	void ToggleOnOff();
}


