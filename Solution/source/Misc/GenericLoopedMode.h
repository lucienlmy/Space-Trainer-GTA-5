#pragma once

// nts: This is so useless with the way it's used
class GenericLoopedMode abstract
{
public:
	static void TickAll();

protected:
	bool bEnabled;
public:
	GenericLoopedMode();

	bool& Enabled();

	virtual void Toggle();
	virtual void TurnOn();
	virtual void TurnOff();

	virtual void Tick() = 0;
};




