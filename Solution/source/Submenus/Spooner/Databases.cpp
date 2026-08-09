#include "Databases.h"

#include "..\..\macros.h"

//#include "..\..\Menu\Menu.h"
//#include "..\..\Menu\Routine.h"

//#include "..\..\Natives\natives2.h"
#include "..\..\Scripting\GTAblip.h"
#include "..\..\Scripting\GTAentity.h"

#include "SpoonerEntity.h"
#include "SpoonerMarker.h"

#include <vector>
#include <utility>
#include <string>

namespace sub::Spooner
{
	namespace Databases
	{
		std::vector<SpoonerEntity> EntityDb;
		std::vector<std::pair<GTAentity, Hash>> RelationshipDb; // Why did I make this?
		std::vector<std::pair<GTAblip, std::string>> RefBlipDb;
		std::vector<SpoonerMarker> MarkerDb;
	}

}



