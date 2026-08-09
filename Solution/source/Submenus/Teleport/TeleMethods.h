#pragma once

typedef int INT, Ped, Vehicle, Object, Entity;
typedef unsigned long DWORD, Hash;
typedef unsigned int UINT;
typedef unsigned char UINT8;
typedef unsigned short UINT16;
typedef char *PCHAR;

class Vector3;
class GTAentity;
class GTAped;

void TeleportNetPed(GTAentity ped, float X, float Y, float Z, bool bWait = true, bool bPtfx = true);
void TeleportNetPed(GTAentity ped, const Vector3& pos, bool bWait = true, bool bPtfx = true);
void TeleportToMissionBlip(GTAped ped);

namespace sub::TeleportLocations_catind
{
	class TeleLocation;

	namespace TeleMethods
	{
		void ToWaypoint(GTAped ped);
		void ToWaypoint241();
		void ToMissionBlip241();
		void ToForward241();
		void ToCoordinates241(const Vector3& coord);
		void ToTeleLocation241(const TeleLocation& loc);
	}

}



