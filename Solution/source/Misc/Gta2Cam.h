#pragma once

#include "GenericLoopedMode.h"

class GTAentity;
class Camera;

namespace GTA2Cam
{
	class Gta2Cam final : public GenericLoopedMode
	{
	private:
	public:
		const Camera& MainCam();

		void CreateMainCam(GTAentity myPed);

		void TurnOn() override;
		void TurnOff() override;

		void Tick() override;
		inline void DoGta2CamTick();

	};


	extern Gta2Cam g_gta2Cam;

	void ToggleOnOff();
}






