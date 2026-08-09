#pragma once

#include <vector>
#include <utility>
#include <string>

typedef unsigned long DWORD, Hash;
class GTAentity;
class GTAblip;

namespace sub::Spooner
{
	class SpoonerEntity;
	class SpoonerMarker;

	namespace Databases
	{
		extern std::vector<SpoonerEntity> EntityDb;
		extern std::vector<std::pair<GTAentity, Hash>> RelationshipDb; // Why did I make this?
		extern std::vector<std::pair<GTAblip, std::string>> RefBlipDb;
		extern std::vector<SpoonerMarker> MarkerDb;
	}

}



