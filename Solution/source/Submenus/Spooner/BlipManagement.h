#pragma once

#include <utility>
#include <string>

class GTAblip;
class Vector3;

namespace sub::Spooner
{
	namespace BlipManagement
	{
		void ClearDb();
		void ClearAllRefCoordBlips();

		void AddBlipToRefCoordDb(const std::pair<GTAblip, std::string>& pB);
		std::pair<GTAblip, std::string> CreateRefCoordBlip(const Vector3& position, const std::string& name, bool addToDb);
	}

}



