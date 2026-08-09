#pragma once

#include <string>
#include <vector>
#include <string_view>

//typedef char *PCHAR;
constexpr std::string_view operator ""_sv(const char* str, std::size_t length)  //Maybe not the best location for this but every file that requiers this includes this header file so why not 
{
	return str;
}

namespace sub::TeleportLocations_catind
{
	class TeleLocation
	{
	public:
		std::string name;
		float x;
		float y;
		float z;
		std::vector<std::string_view> iplsToLoad;
		std::vector<std::string_view> iplsToRemove;
		std::vector<std::string_view> interiorProps;
		bool bOnTheLine;
		bool bOffTheLine;
		bool bInterior;
	public:
		TeleLocation(bool pbOnTheLine, float pX, float pY, float pZ, const std::string& pName)
			: name(pName), x(pX), y(pY), z(pZ), bOnTheLine(pbOnTheLine), bOffTheLine(false), bInterior(false)
		{
		}
		TeleLocation(const std::string& pName, float pX, float pY, float pZ, bool pbOnTheLine = false, bool pbOffTheLine = false, bool pbInterior = false)
			: name(pName), x(pX), y(pY), z(pZ), bOnTheLine(pbOnTheLine), bOffTheLine(pbOffTheLine), bInterior(pbInterior)
		{
		}
		TeleLocation(const std::string& pName, float pX, float pY, float pZ, std::vector<std::string_view> pIplsToLoad, std::vector<std::string_view> pIplsToRemove, bool pbOnTheLine = false, bool pbOffTheLine = false, bool pbInterior = false)
			: name(pName), x(pX), y(pY), z(pZ), iplsToLoad(pIplsToLoad), iplsToRemove(pIplsToRemove), bOnTheLine(pbOnTheLine), bOffTheLine(pbOffTheLine), bInterior(pbInterior)
		{
		}
		TeleLocation(const std::string& pName, float pX, float pY, float pZ, std::vector<std::string_view> pIplsToLoad, std::vector<std::string_view> pIplsToRemove, std::vector<std::string_view> pInteriorProps, bool pbOnTheLine = false, bool pbOffTheLine = false, bool pbInterior = false)
			: name(pName), x(pX), y(pY), z(pZ), iplsToLoad(pIplsToLoad), iplsToRemove(pIplsToRemove), interiorProps(pInteriorProps), bOnTheLine(pbOnTheLine), bOffTheLine(pbOffTheLine), bInterior(pbInterior)
		{
		}
	};

	class NamedTeleLocationList
	{
	public:
		std::string categoryName;
		const std::vector<TeleLocation>* locList_ptr;
		const std::vector<NamedTeleLocationList>* nextNamedLocListList;
	};

}



