#pragma once

#include "GenericLoopedMode.h"

#include "..\Scripting\Rope.h"
#include "..\Scripting\GTAentity.h"

#include <vector>

typedef unsigned __int8 UINT8;
typedef unsigned long Hash;

class Vector3;
class GTAprop;

namespace RopeGun
{
	class EntitiesAndRope
	{
	public:
		Rope rope;
		GTAentity e1;
		GTAentity e2;
		float initialDistance;

		EntitiesAndRope();
		EntitiesAndRope(const Rope& ropeP, const GTAentity& e1P, const GTAentity& e2P, float initialDistanceP);
		EntitiesAndRope(const EntitiesAndRope& obj);

		EntitiesAndRope& operator = (const EntitiesAndRope& right);
	};

	class RopeGun final : public GenericLoopedMode
	{
	private:
		const Hash whash;
		UINT8 shootCount;
		GTAentity _thing1, _thing2;
		std::vector<EntitiesAndRope> allRopes;
	public:
		RopeGun();

		void TurnOn() override;
		void TurnOff() override;

		void Tick() override;
		void DoRopeGunTick();
		void DoRopeEntitiesTick();

		void CreateRopeFor2(GTAentity& entity1, GTAentity& entity2);
		GTAprop CreateAFake(const Vector3& pos);

		void PrintGunInstructions();
		void PrintShootCountHelpText();

	};


	extern RopeGun g_ropeGun;

	void ToggleOnOff();
}


