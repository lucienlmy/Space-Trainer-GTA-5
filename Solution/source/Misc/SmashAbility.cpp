#include "SmashAbility.h"

#include "..\macros.h"

#include "..\Natives\natives2.h"
#include "..\Util\GTAmath.h"
#include "..\Scripting\enums.h"
#include "..\Scripting\GTAentity.h"
#include "..\Scripting\GTAped.h"
#include "..\Scripting\GTAvehicle.h"
#include "..\Scripting\World.h"
#include "..\Scripting\Game.h"

#include "..\Menu\Routine.h"

namespace SmashAbility
{
	static DWORD s_lastPunchTick = 0;

	static Vector3 FlatDir(const Vector3& from, const Vector3& to, const Vector3& fallback)
	{
		Vector3 dir = to - from;
		dir.z = 0.0f;
		const float len = dir.Length();
		if (len > 0.001f)
			return dir * (1.0f / len);
		return fallback;
	}

	static void KnockPed(GTAped ped, const Vector3& myPos, const Vector3& fwd)
	{
		if (!ped.Exists() || ped.Handle() == PLAYER_PED_ID())
			return;
		if (IS_PED_A_PLAYER(ped.Handle()))
			return;

		ped.RequestControlOnce();
		SET_PED_CAN_RAGDOLL(ped.Handle(), TRUE);
		SET_PED_TO_RAGDOLL(ped.Handle(), 3500, 3500, 0, false, false, false);

		Vector3 dir = FlatDir(myPos, ped.GetPosition(), fwd);
		ped.ApplyForce(Vector3(dir.x * 42.0f, dir.y * 42.0f, 22.0f), ForceType::MaxForceRot2);
		APPLY_FORCE_TO_ENTITY_CENTER_OF_MASS(ped.Handle(), 1, dir.x * 18.0f, dir.y * 18.0f, 8.0f, true, false, true, false);
	}

	static void KnockVehicle(GTAvehicle veh, const Vector3& myPos, const Vector3& fwd)
	{
		if (!veh.Exists() || veh.Handle() == g_myVeh)
			return;

		veh.RequestControlOnce();
		Vector3 dir = FlatDir(myPos, veh.GetPosition(), fwd);
		veh.ApplyForce(Vector3(dir.x * 55.0f, dir.y * 55.0f, 18.0f), ForceType::MaxForceRot2);
		APPLY_FORCE_TO_ENTITY_CENTER_OF_MASS(veh.Handle(), 1, dir.x * 28.0f, dir.y * 28.0f, 10.0f, true, false, true, false);
	}

	static bool IsCloseInFront(const Vector3& myPos, const Vector3& fwd, const Vector3& targetPos, float maxDist)
	{
		Vector3 to = targetPos - myPos;
		to.z = 0.0f;
		const float dist = to.Length();
		if (dist > maxDist)
			return false;
		if (dist < 0.05f)
			return true;
		to = to * (1.0f / dist);
		// Must be clearly in front (contact punch), not side/behind AoE.
		return (to.x * fwd.x + to.y * fwd.y) >= 0.55f;
	}

	void SmashAbility::TurnOn()
	{
		GenericLoopedMode::TurnOn();
		PrintSmashInstructions();
	}

	void SmashAbility::Tick()
	{
		if (!bEnabled)
			return;

		GTAped myPed = PLAYER_PED_ID();
		if (!myPed.Exists() || myPed.IsInVehicle() || IS_PAUSE_MENU_ACTIVE())
			return;

		// Only while a melee strike is actually being performed (not just aiming/combat stance).
		if (!IS_PED_PERFORMING_MELEE_ACTION(myPed.Handle()))
			return;

		const DWORD now = GetTickCount();
		if (now - s_lastPunchTick < 180)
			return;
		s_lastPunchTick = now;

		DoMeleeKnockback();
	}

	void SmashAbility::DoMeleeKnockback()
	{
		GTAped myPed = PLAYER_PED_ID();
		if (!myPed.Exists())
			return;

		const Vector3 myPos = myPed.GetPosition();
		const Vector3 fwd = myPed.ForwardVector();
		const float contactRange = 2.35f;

		Ped meleeTarget = GET_MELEE_TARGET_FOR_PED(myPed.Handle());
		bool hitSomeone = false;

		if (DOES_ENTITY_EXIST(meleeTarget) && IS_ENTITY_A_PED(meleeTarget))
		{
			GTAped targetPed(meleeTarget);
			if (IsCloseInFront(myPos, fwd, targetPed.GetPosition(), contactRange))
			{
				KnockPed(targetPed, myPos, fwd);
				hitSomeone = true;
			}
		}

		// Only scan nearby if we don't already have a melee target, and keep range tight.
		if (!hitSomeone)
		{
			std::vector<GTAped> peds;
			World::GetNearbyPeds(peds, myPed, contactRange + 0.4f, 16);
			for (GTAped& ped : peds)
			{
				if (ped.Handle() == myPed.Handle())
					continue;
				if (!IsCloseInFront(myPos, fwd, ped.GetPosition(), contactRange))
					continue;
				KnockPed(ped, myPos, fwd);
				hitSomeone = true;
				break; // one ped per punch
			}
		}

		std::vector<GTAvehicle> vehs;
		World::GetNearbyVehicles(vehs, myPed, contactRange + 0.8f);
		for (GTAvehicle& veh : vehs)
		{
			if (!IsCloseInFront(myPos, fwd, veh.GetPosition(), contactRange + 0.6f))
				continue;
			KnockVehicle(veh, myPos, fwd);
			break; // one vehicle per punch
		}
	}

	void SmashAbility::DoLevitate()
	{
		DoMeleeKnockback();
	}

	void SmashAbility::DoSmash()
	{
		DoMeleeKnockback();
	}

	bool SmashAbility::IsLevitatePressed()
	{
		return false;
	}
	bool SmashAbility::IsLevitateReleased()
	{
		return false;
	}

	void SmashAbility::PrintSmashInstructions()
	{
		Game::Print::PrintBottomLeft("Superman Punch: hit people/cars at close range.");
	}

	SmashAbility g_smashAbility;

	void ToggleOnOff()
	{
		g_smashAbility.Toggle();
	}
}
