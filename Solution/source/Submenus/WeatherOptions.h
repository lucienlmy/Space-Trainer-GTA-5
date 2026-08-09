#pragma once

#include "..\macros.h"

#include "..\Menu\Menu.h"
#include "..\Menu\Routine.h"

#include "..\Natives\natives2.h"
#include "..\Memory\GTAmemory.h"
#include "..\Scripting\World.h"
#include "..\Util\FileLogger.h"

#include <string>
#include <map>
#include <array>

namespace sub
{
	void WeatherMenu();

	namespace WeatherClouds
	{
		extern const std::array<std::string, 20> cloudNames;
		void CloudMenu();
	}
}