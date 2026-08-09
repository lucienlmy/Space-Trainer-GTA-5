#pragma once

#include "GenericLoopedMode.h"

namespace ManualRespawn
{
	class ManualRespawn final : public GenericLoopedMode
	{
	private:
		bool inRespawn;
	public:
		ManualRespawn();
		bool InRespawn();
		void TurnOff() override;
		bool IsSkipPressed();
		inline void ShowRespawnHelpText();
		void Tick() override;
		inline void DoManualRespawnTick();

	};
	

	extern ManualRespawn g_manualRespawn;

	void ToggleOnOff();
	void CheckSelfDealthModel();
}