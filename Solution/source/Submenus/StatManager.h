#pragma once

#include <string>

#include "..\macros.h"

#include "..\Menu\Menu.h"

#include "..\Natives\natives2.h"
#include "..\Scripting\Game.h"
#include "..\Util\FileLogger.h"

#include <string>
#include <vector>
#include <utility>

typedef unsigned __int8 UINT8;

namespace sub
{
	namespace SpStatManager
	{
		enum class StatDataType_t : UINT8
		{
			UNKNOWN,
			BOOL,
			INT,
			FLOAT
		};

		struct CharStat_t 
		{ 
			std::string name;
			std::string caption; 
			StatDataType_t type; 
			float min;
			float max; 
		};

		int StatGetInt(const std::string& name);
		bool StatGetBool(const std::string& name);
		float StatGetFloat(const std::string& name);
		std::string StatGetString(const std::string& name);
		void StatSetInt(const std::string& name, int value);
		void StatSetBool(const std::string& name, bool value);
		void StatSetFloat(const std::string& name, float value);
		void StatSetString(const std::string& name, const std::string& value);

		void AddOptionStats(const CharStat_t& stat);
		void SPStatsManagerMenu();
		void SPStatsInCharMenu();
		void InCharInListMenu();

	}

}



