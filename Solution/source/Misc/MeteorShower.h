#pragma once

#include "GenericLoopedMode.h"

#include "..\Scripting\Game.h" //GameSound

#include <vector>

typedef unsigned long DWORD;

namespace GTAmodel {
	class Model;
}
class GTAprop;

namespace MeteorShower
{
	class MeteorShower final : public GenericLoopedMode
	{
	private:
		DWORD timer1;
		DWORD timer2;
		std::vector<GTAprop> rockArray;
		std::vector<GTAmodel::Model> rockModels;
		static const std::vector<std::string> rockModelNames;
		static Game::Sound::GameSound crateLandSound;
	public:
		MeteorShower();

		void TurnOn() override;
		void TurnOff() override;

		void Tick() override;
		inline void DoMeteorShowerTick();

	};
	
	
	extern MeteorShower g_meteorShower;

	void ToggleOnOff();
}


