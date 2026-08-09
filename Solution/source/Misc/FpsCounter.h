#pragma once

typedef unsigned long DWORD;

namespace FPSCounter
{
	class FpsCounter final
	{
	protected:
		DWORD fpsValue;
		DWORD frameCounter;
		DWORD timer;

	public:
		FpsCounter();

		DWORD Get();

		void Tick();
	};
	extern FpsCounter g_fpsCounter;

	extern bool bDisplayFps;
	extern int fpsCorner; // 0 TL, 1 TR, 2 BL, 3 BR

	void DrawOverlay();
}
