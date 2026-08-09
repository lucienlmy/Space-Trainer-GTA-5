#include "FpsCounter.h"

#include "..\macros.h"

#include "..\Scripting\enums.h"
#include "..\Scripting\Game.h"
#include "..\Util\GTAmath.h"

#include <Windows.h> // GetTickCount
#include <string>

namespace FPSCounter
{
	FpsCounter::FpsCounter()
		: fpsValue(0), frameCounter(0), timer(GetTickCount())
	{
	}

	DWORD FpsCounter::Get()
	{
		this->Tick();
		return fpsValue;
	}

	void FpsCounter::Tick()
	{
		frameCounter++;

		if (GetTickCount() - timer > 500) //0.5sec
		{
			// store frame count to fpsValue
			fpsValue = frameCounter * 2;

			// reset
			frameCounter = 0;
			timer = GetTickCount();
		}
	}

	FpsCounter g_fpsCounter;

	bool bDisplayFps = false;
	int fpsCorner = 1; // Top-Right by default

	void DrawOverlay()
	{
		if (!bDisplayFps)
			return;

		const auto fps = g_fpsCounter.Get();
		const std::string text = std::to_string(fps) + " FPS";

		float x = 0.02f;
		float y = 0.02f;
		bool right = false;

		switch (fpsCorner)
		{
		case 0: // Top-Left
			x = 0.02f; y = 0.02f; right = false;
			break;
		case 1: // Top-Right
			x = 0.98f; y = 0.02f; right = true;
			break;
		case 2: // Bottom-Left
			x = 0.02f; y = 0.94f; right = false;
			break;
		case 3: // Bottom-Right
			x = 0.98f; y = 0.94f; right = true;
			break;
		default:
			x = 0.98f; y = 0.02f; right = true;
			break;
		}

		Game::Print::SetupDraw(0, Vector2(0.0f, 0.35f), false, right, true);
		Game::Print::drawstring(text, x, y);
	}
}
