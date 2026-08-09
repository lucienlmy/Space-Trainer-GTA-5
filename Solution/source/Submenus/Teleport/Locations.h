#pragma once

#include <string>
#include <vector>
#include <string_view>

//typedef char *PCHAR;

namespace sub::TeleportLocations_catind
{
	class TeleLocation;
	class NamedTeleLocationList;

	namespace IplNames
	{
		extern const std::vector<std::string_view> vYacht_Smboat1;
		extern const std::vector<std::string_view> vYacht_Heist1;
		extern const std::vector<std::string_view> vYacht_Heist2;
		extern const std::vector<std::string_view> vAircraftCarrier1;
		extern const std::vector<std::string_view> vNorthYankton;
		extern const std::vector<std::string_view> vAllCustomisableApartments1;
		extern const std::vector<std::string_view> vAllOffices1;
		extern const std::vector<std::string_view> vAllOfficeGarages1;
	}

	namespace Locations
	{
		extern const std::vector<TeleLocation> vApartmentInteriors;

		extern const std::vector<NamedTeleLocationList> vAllCategories;
	}

}



