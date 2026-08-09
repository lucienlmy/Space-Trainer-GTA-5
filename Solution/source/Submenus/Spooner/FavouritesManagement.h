#pragma once

#include <string>

typedef unsigned long DWORD, Hash;
namespace GTAmodel {
	class Model;
}

namespace sub::Spooner
{
	namespace FavouritesManagement
	{
		extern std::string xmlFavouriteProps;
		bool IsPropAFavourite(const std::string& modelName, Hash modelHash);
		bool AddPropToFavourites(const std::string& modelName, Hash modelHash);
		bool RemovePropFromFavourites(const std::string& modelName, Hash modelHash);

		extern bool(*IsVehicleAFavourite)(GTAmodel::Model vehModel);
		extern bool(*AddVehicleToFavourites)(GTAmodel::Model vehModel, const std::string& customName);
		extern bool(*RemoveVehicleFromFavourites)(GTAmodel::Model vehModel);
	}

}



