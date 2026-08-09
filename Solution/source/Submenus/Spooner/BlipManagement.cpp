#include "BlipManagement.h"

#include "..\..\macros.h"

//#include "..\..\Menu\Menu.h"
//#include "..\..\Menu\Routine.h"

//#include "..\..\Natives\natives2.h"
#include "..\..\Scripting\GTAblip.h"
#include "..\..\Scripting\World.h"
#include "..\..\Util\GTAmath.h"

#include "Databases.h"

#include <utility>
#include <vector>
#include <string>

namespace sub::Spooner
{
	namespace BlipManagement
	{
		void ClearDb()
		{
			Databases::RefBlipDb.clear();
		}
		void ClearAllRefCoordBlips()
		{
			for (auto& b : Databases::RefBlipDb)
			{
				b.first.Remove();
			}
			Databases::RefBlipDb.clear();
		}

		void AddBlipToRefCoordDb(const std::pair<GTAblip, std::string>& pB)
		{
			for (auto& b : Databases::RefBlipDb)
			{
				if (b.second.compare(pB.second) == 0)
				{
					b = pB;
					return;
				}
			}
			Databases::RefBlipDb.push_back(pB);
		}
		std::pair<GTAblip, std::string> CreateRefCoordBlip(const Vector3& position, const std::string& name, bool addToDb)
		{
			auto blip = World::CreateBlip(position);
			blip.SetFlashing(false);
			blip.SetScale(-1.0f);
			blip.SetFriendly(true);
			blip.SetIcon(BlipIcon::Castle);
			blip.SetColour(BlipColour::Green);
			blip.SetBlipName(name);
			const auto& blipAndNamePair = std::make_pair(blip, name);
			if (addToDb)
				AddBlipToRefCoordDb(blipAndNamePair);
			return blipAndNamePair;
		}
	}

}



