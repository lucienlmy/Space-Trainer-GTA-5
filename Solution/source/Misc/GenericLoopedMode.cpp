#include "GenericLoopedMode.h"

// Not used
void GenericLoopedMode::TickAll()
{

}


GenericLoopedMode::GenericLoopedMode()
	: bEnabled(false)
{
}

bool& GenericLoopedMode::Enabled()
{
	return bEnabled;
}

void GenericLoopedMode::Toggle()
{
	if (bEnabled)
	{
		TurnOff();
	}
	else
	{
		TurnOn();
	}
}
void GenericLoopedMode::TurnOn()
{
	bEnabled = true;
}
void GenericLoopedMode::TurnOff()
{
	bEnabled = false;
}




