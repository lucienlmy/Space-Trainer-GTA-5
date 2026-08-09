#pragma once

namespace JumpAroundMode
{
	extern bool bEnabled;

	inline void DrawDiscoLights();

	void Tick();

	void BgMusic(bool enable);
	void StartJumping(bool enable);

}



