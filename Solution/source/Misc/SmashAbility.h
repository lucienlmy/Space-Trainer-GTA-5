#pragma once

#include "GenericLoopedMode.h"

namespace SmashAbility
{
	class SmashAbility final : public GenericLoopedMode
	{
	private:

	public:
		void TurnOn() override;

		void Tick() override;

		void DoMeleeKnockback();
		void DoLevitate();
		void DoSmash();

		bool IsLevitatePressed();
		bool IsLevitateReleased();

		void PrintSmashInstructions();

	};


	extern SmashAbility g_smashAbility;

	void ToggleOnOff();
}



