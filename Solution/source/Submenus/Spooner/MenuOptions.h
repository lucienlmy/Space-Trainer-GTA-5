#pragma once

#include <string>
#include <functional>

typedef int INT;
typedef signed char INT8;
typedef unsigned char UINT8;
typedef unsigned long DWORD, Hash;
typedef int Ped;

namespace GTAmodel {
	class Model;
}

namespace sub::Spooner
{
	namespace MenuOptions
	{
		void AddOption_AddProp(const std::string& text, const GTAmodel::Model& model);
		void AddOptionAddPed(const std::string& text, const GTAmodel::Model& model);
		void AddOption_AddVehicle(const std::string& text, const GTAmodel::Model& model);

		void AddOption_RelationshipTextScroller();
		void AddOption_AddPedWithCallback(const std::string& text, const GTAmodel::Model& model,
			const std::function<void(Ped, const std::string&)>& callback);
	}

}



