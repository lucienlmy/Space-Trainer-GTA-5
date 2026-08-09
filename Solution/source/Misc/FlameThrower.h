#pragma once

//#include "GenericLoopedMode.h"

#include <vector>

typedef unsigned long DWORD, Hash;
typedef char *PCHAR;

class GTAplayer;

namespace FlameThrower
{
	extern Hash _whash;
	
	bool IsPlayerAdded(GTAplayer player);
	void RemovePlayer(GTAplayer player);
	void AddPlayer(GTAplayer player);

	void RemoveSelf();
	void AddSelf();

	void Tick();

}






