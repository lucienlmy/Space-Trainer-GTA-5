/*
* Space Trainer - Contract Hit missions (5 types)
*/
#include "HitmanContracts.h"

#include "..\Menu\Menu.h"
#include "..\Menu\Routine.h"
#include "..\Menu\submenu_enum.h"
#include "..\Menu\submenu_switch.h"
#include "..\Natives\natives2.h"
#include "..\Scripting\Game.h"
#include "..\Scripting\GTAped.h"
#include "..\Scripting\GTAvehicle.h"
#include "..\Scripting\Model.h"
#include "..\Scripting\World.h"
#include "..\Scripting\enums.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <string>
#include <vector>
#include <Windows.h>

namespace HitmanContracts
{
	enum class MissionType : int
	{
		None = 0,
		GangAssault,
		SoloHit,
		DriveByHit,
		BoatHit,
		SniperHit
	};

	struct Target
	{
		Ped ped = 0;
		Blip blip = 0;
		bool isPrimary = false;
		bool alerted = false;
	};

	static MissionType s_type = MissionType::None;
	static std::vector<Target> s_targets;
	static std::vector<Vehicle> s_vehicles;
	static Hash s_groupNeutral = 0;
	static Hash s_groupHostile = 0;
	static int s_reward = 0;
	static DWORD s_startedAt = 0;
	static DWORD s_lastHud = 0;
	static bool s_anyAlerted = false;

	static Hash NeutralGroup()
	{
		if (s_groupNeutral == 0)
			s_groupNeutral = World::AddRelationshipGroup("SPACE_HITMAN_NEUTRAL");
		Hash playerGroup = GET_HASH_KEY("PLAYER");
		// 3 = Neutral — will not attack on sight
		SET_RELATIONSHIP_BETWEEN_GROUPS(3, s_groupNeutral, playerGroup);
		SET_RELATIONSHIP_BETWEEN_GROUPS(3, playerGroup, s_groupNeutral);
		return s_groupNeutral;
	}

	static Hash HostileGroup()
	{
		if (s_groupHostile == 0)
			s_groupHostile = World::AddRelationshipGroup("SPACE_HITMAN_HOSTILE");
		Hash playerGroup = GET_HASH_KEY("PLAYER");
		SET_RELATIONSHIP_BETWEEN_GROUPS(5, s_groupHostile, playerGroup);
		SET_RELATIONSHIP_BETWEEN_GROUPS(5, playerGroup, s_groupHostile);
		return s_groupHostile;
	}

	static void ClearBlip(Blip& b)
	{
		if (b && DOES_BLIP_EXIST(b))
			REMOVE_BLIP(&b);
		b = 0;
	}

	static void CleanupEntities(bool deleteAll)
	{
		for (auto& t : s_targets)
		{
			ClearBlip(t.blip);
			if (deleteAll && DOES_ENTITY_EXIST(t.ped))
			{
				SET_ENTITY_AS_MISSION_ENTITY(t.ped, true, true);
				DELETE_PED(&t.ped);
			}
		}
		s_targets.clear();

		for (Vehicle v : s_vehicles)
		{
			if (deleteAll && DOES_ENTITY_EXIST(v))
			{
				SET_ENTITY_AS_MISSION_ENTITY(v, true, true);
				DELETE_VEHICLE(&v);
			}
		}
		s_vehicles.clear();
	}

	static void ResetMissionSilent()
	{
		CleanupEntities(true);
		s_type = MissionType::None;
		s_reward = 0;
		s_anyAlerted = false;
	}

	void Abort()
	{
		ResetMissionSilent();
		Game::Print::PrintBottomCentre("~r~Contract aborted.");
	}

	bool IsActive()
	{
		return s_type != MissionType::None;
	}

	static Blip MarkTarget(Ped ped, bool primary)
	{
		Blip b = ADD_BLIP_FOR_ENTITY(ped);
		SET_BLIP_SPRITE(b, primary ? 432 : 270);
		SET_BLIP_COLOUR(b, 1);
		SET_BLIP_SCALE(b, primary ? 0.95f : 0.7f);
		SET_BLIP_AS_FRIENDLY(b, false);
		SET_BLIP_FLASHES(b, primary);
		BEGIN_TEXT_COMMAND_SET_BLIP_NAME("STRING");
		ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(primary ? "Hit Target" : "Associate");
		END_TEXT_COMMAND_SET_BLIP_NAME(b);
		if (primary)
		{
			SET_BLIP_ROUTE(b, true);
			SET_BLIP_ROUTE_COLOUR(b, 1);
		}
		return b;
	}

	static Vector3 FindOutdoorSpawn(float preferredDist, float angleDeg, bool wantWater)
	{
		Ped me = PLAYER_PED_ID();
		Vector3 base = GET_ENTITY_COORDS(me, true);
		const float heading = GET_ENTITY_HEADING(me);

		for (int attempt = 0; attempt < 12; ++attempt)
		{
			const float dist = preferredDist + (attempt % 4) * 12.0f;
			const float ang = angleDeg + heading + attempt * 28.0f;
			const float rad = ang * 0.01745329251f;
			Vector3 guess(base.x + std::cos(rad) * dist, base.y + std::sin(rad) * dist, base.z);

			Vector3_t node{};
			float nodeHeading = 0.0f;
			if (GET_CLOSEST_VEHICLE_NODE_WITH_HEADING(guess.x, guess.y, guess.z, &node, &nodeHeading, 1, 3.0f, 0.0f))
			{
				Vector3 pos(node.x, node.y, node.z);
				float gz = pos.z;
				GET_GROUND_Z_FOR_3D_COORD(pos.x, pos.y, pos.z + 50.0f, &gz, FALSE, FALSE);
				pos.z = gz + 1.05f;

				if (wantWater)
				{
					float waterZ = 0.0f;
					if (GET_WATER_HEIGHT(pos.x, pos.y, pos.z + 30.0f, &waterZ))
					{
						pos.z = waterZ + 0.6f;
						return pos;
					}
					continue;
				}

				if (GET_INTERIOR_AT_COORDS(pos.x, pos.y, pos.z) == 0)
					return pos;
			}

			Vector3_t safe{};
			if (GET_SAFE_COORD_FOR_PED(guess.x, guess.y, guess.z, TRUE, &safe, 16))
			{
				Vector3 pos(safe.x, safe.y, safe.z + 1.0f);
				if (!wantWater && GET_INTERIOR_AT_COORDS(pos.x, pos.y, pos.z) == 0)
					return pos;
			}
		}

		Vector3 fallback = GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(me, 0.0f, preferredDist, 0.0f);
		float gz = fallback.z;
		GET_GROUND_Z_FOR_3D_COORD(fallback.x, fallback.y, fallback.z + 60.0f, &gz, FALSE, FALSE);
		fallback.z = gz + 1.05f;
		return fallback;
	}

	static Ped SpawnContractPed(Hash modelHash, const Vector3& pos, float heading, Hash weapon, bool primary)
	{
		GTAmodel::Model model(modelHash);
		if (!model.IsInCdImage() || !model.Load(6000))
			return 0;

		REQUEST_COLLISION_AT_COORD(pos.x, pos.y, pos.z);
		Ped ped = CREATE_PED(26, model.hash, pos.x, pos.y, pos.z, heading, FALSE, FALSE);
		model.Unload();
		if (!DOES_ENTITY_EXIST(ped) || ped == 0)
			return 0;

		SET_ENTITY_AS_MISSION_ENTITY(ped, true, true);
		SET_PED_DEFAULT_COMPONENT_VARIATION(ped);
		SET_ENTITY_LOAD_COLLISION_FLAG(ped, true, 1);
		SET_ENTITY_VELOCITY(ped, 0.0f, 0.0f, 0.0f);

		float gz = pos.z;
		GET_GROUND_Z_FOR_3D_COORD(pos.x, pos.y, pos.z + 50.0f, &gz, FALSE, FALSE);
		SET_ENTITY_COORDS_NO_OFFSET(ped, pos.x, pos.y, gz + 1.0f, false, false, true);

		SET_PED_RELATIONSHIP_GROUP_HASH(ped, NeutralGroup());
		SET_PED_AS_ENEMY(ped, false);
		SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(ped, true);
		SET_PED_KEEP_TASK(ped, true);
		SET_PED_COMBAT_ABILITY(ped, primary ? 2 : 1);
		SET_PED_COMBAT_ATTRIBUTES(ped, 46, true);
		SET_PED_COMBAT_ATTRIBUTES(ped, 5, false); // can leave vehicle when not yet hostile
		SET_PED_ACCURACY(ped, primary ? 50 : 30);
		SET_ENTITY_MAX_HEALTH(ped, primary ? 280 : 180);
		SET_ENTITY_HEALTH(ped, primary ? 280 : 180, 0);
		SET_PED_ARMOUR(ped, primary ? 40 : 0);
		SET_PED_CAN_RAGDOLL(ped, true);
		FREEZE_ENTITY_POSITION(ped, false);

		if (weapon)
			GIVE_WEAPON_TO_PED(ped, weapon, 200, false, false); // unequipped until alerted

		Target t;
		t.ped = ped;
		t.isPrimary = primary;
		t.alerted = false;
		t.blip = MarkTarget(ped, primary);
		s_targets.push_back(t);
		return ped;
	}

	static Vehicle SpawnVehicleAt(Hash modelHash, const Vector3& pos, float heading)
	{
		GTAmodel::Model model(modelHash);
		if (!model.IsInCdImage() || !model.Load(6000))
			return 0;
		REQUEST_COLLISION_AT_COORD(pos.x, pos.y, pos.z);
		Vehicle v = CREATE_VEHICLE(model.hash, pos.x, pos.y, pos.z, heading, FALSE, FALSE, FALSE);
		model.Unload();
		if (!DOES_ENTITY_EXIST(v) || v == 0)
			return 0;
		SET_ENTITY_AS_MISSION_ENTITY(v, true, true);
		SET_VEHICLE_ON_GROUND_PROPERLY(v, 5.0f);
		SET_VEHICLE_ENGINE_ON(v, true, true, false);
		SET_ENTITY_VELOCITY(v, 0.0f, 0.0f, 0.0f);
		s_vehicles.push_back(v);
		return v;
	}

	static void MakeHostile(Target& t)
	{
		if (!DOES_ENTITY_EXIST(t.ped) || t.alerted)
			return;
		t.alerted = true;
		s_anyAlerted = true;
		SET_PED_RELATIONSHIP_GROUP_HASH(t.ped, HostileGroup());
		SET_PED_AS_ENEMY(t.ped, true);
		SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(t.ped, false);
		SET_PED_COMBAT_ATTRIBUTES(t.ped, 5, true);
		Hash weap = 0;
		GET_CURRENT_PED_WEAPON(t.ped, &weap, true);
		if (!weap || weap == WEAPON_UNARMED)
		{
			GIVE_WEAPON_TO_PED(t.ped, WEAPON_PISTOL, 120, false, true);
			SET_CURRENT_PED_WEAPON(t.ped, WEAPON_PISTOL, true);
		}
		else
			SET_CURRENT_PED_WEAPON(t.ped, weap, true);
		TASK_COMBAT_PED(t.ped, PLAYER_PED_ID(), 0, 16);
	}

	static void AlertAll()
	{
		for (auto& t : s_targets)
			MakeHostile(t);
	}

	static void GiveIdleTask(Ped ped, MissionType type)
	{
		if (!DOES_ENTITY_EXIST(ped))
			return;
		switch (type)
		{
		case MissionType::GangAssault:
			TASK_START_SCENARIO_IN_PLACE(ped, "WORLD_HUMAN_SMOKING", 0, true);
			break;
		case MissionType::SoloHit:
			TASK_WANDER_STANDARD(ped, 10.0f, 10);
			break;
		case MissionType::SniperHit:
			TASK_STAND_STILL(ped, -1);
			break;
		default:
			TASK_STAND_STILL(ped, -1);
			break;
		}
	}

	static void GiveCashReward(int amount)
	{
		Ped me = PLAYER_PED_ID();
		Vector3 p = GET_ENTITY_COORDS(me, true);
		const int per = (std::max)(500, amount / 6);
		for (int i = 0; i < 6; ++i)
		{
			CREATE_AMBIENT_PICKUP(
				GET_HASH_KEY("PICKUP_MONEY_CASE"),
				p.x + (i % 3) * 0.5f,
				p.y + (i / 3) * 0.5f,
				p.z + 0.4f,
				0, per, 1, false, true);
		}
	}

	static void CompleteMission()
	{
		const int reward = s_reward;
		CleanupEntities(false);
		s_type = MissionType::None;
		s_reward = 0;
		s_anyAlerted = false;
		GiveCashReward(reward);
		char msg[96];
		sprintf_s(msg, "~g~Contract complete. Reward ~w~$%d", reward);
		Game::Print::PrintBottomLeft(msg);
	}

	static void FailMission(const char* reason)
	{
		CleanupEntities(true);
		s_type = MissionType::None;
		s_reward = 0;
		s_anyAlerted = false;
		Game::Print::PrintBottomCentre(std::string("~r~Contract failed: ") + reason);
	}

	static const char* TypeName(MissionType t)
	{
		switch (t)
		{
		case MissionType::GangAssault: return "Gang Assault";
		case MissionType::SoloHit: return "Solo Contract";
		case MissionType::DriveByHit: return "Mobile Hit";
		case MissionType::BoatHit: return "Boat Hit";
		case MissionType::SniperHit: return "Sniper Contract";
		default: return "None";
		}
	}

	static void StartGangAssault()
	{
		ResetMissionSilent();
		s_type = MissionType::GangAssault;
		s_reward = 25000;
		s_startedAt = GetTickCount();

		static const Hash models[] = {
			GET_HASH_KEY("g_m_y_ballaorig_01"),
			GET_HASH_KEY("g_m_y_ballaeast_01"),
			GET_HASH_KEY("g_m_y_ballasout_01"),
			GET_HASH_KEY("g_m_y_famca_01"),
			GET_HASH_KEY("g_m_y_famdnf_01")
		};

		Vector3 center = FindOutdoorSpawn(60.0f, 35.0f, false);
		SET_NEW_WAYPOINT(center.x, center.y);

		for (int i = 0; i < 6; ++i)
		{
			const float a = i * 60.0f;
			const float rad = a * 0.01745329251f;
			Vector3 pos(center.x + std::cos(rad) * 3.8f, center.y + std::sin(rad) * 3.8f, center.z);
			float gz = pos.z;
			GET_GROUND_Z_FOR_3D_COORD(pos.x, pos.y, pos.z + 40.0f, &gz, FALSE, FALSE);
			pos.z = gz + 1.0f;
			Ped p = SpawnContractPed(models[i % 5], pos, a + 180.0f, WEAPON_MICROSMG, i == 0);
			if (p) GiveIdleTask(p, MissionType::GangAssault);
		}

		Game::Print::PrintBottomLeft("~y~Gang Assault:~s~ stay quiet until you aim or shoot.");
	}

	static void StartSoloHit()
	{
		ResetMissionSilent();
		s_type = MissionType::SoloHit;
		s_reward = 15000;
		s_startedAt = GetTickCount();

		Vector3 pos = FindOutdoorSpawn(75.0f, -30.0f, false);
		SET_NEW_WAYPOINT(pos.x, pos.y);

		Hash models[] = {
			GET_HASH_KEY("a_m_m_business_01"),
			GET_HASH_KEY("a_m_y_business_02"),
			GET_HASH_KEY("u_m_m_jewelsec_01")
		};
		Ped target = SpawnContractPed(models[rand() % 3], pos, GET_ENTITY_HEADING(PLAYER_PED_ID()) + 180.0f, WEAPON_PISTOL, true);
		if (target) GiveIdleTask(target, MissionType::SoloHit);

		Vector3 gpos = GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(target ? target : PLAYER_PED_ID(), 2.2f, -1.5f, 0.0f);
		float gz = gpos.z;
		GET_GROUND_Z_FOR_3D_COORD(gpos.x, gpos.y, gpos.z + 40.0f, &gz, FALSE, FALSE);
		gpos.z = gz + 1.0f;
		Ped guard = SpawnContractPed(GET_HASH_KEY("s_m_m_highsec_01"), gpos, 0.0f, WEAPON_COMBATPISTOL, false);
		if (guard)
			TASK_GUARD_CURRENT_POSITION(guard, 8.0f, 8.0f, true);

		Game::Print::PrintBottomLeft("~y~Solo Contract:~s~ eliminate the marked target (stealth OK).");
	}

	static void StartDriveByHit()
	{
		ResetMissionSilent();
		s_type = MissionType::DriveByHit;
		s_reward = 30000;
		s_startedAt = GetTickCount();

		Vector3 pos = FindOutdoorSpawn(95.0f, 20.0f, false);
		SET_NEW_WAYPOINT(pos.x, pos.y);

		Hash cars[] = {
			GET_HASH_KEY("schafter2"),
			GET_HASH_KEY("cognoscenti"),
			GET_HASH_KEY("baller2")
		};
		Vehicle veh = SpawnVehicleAt(cars[rand() % 3], pos, GET_ENTITY_HEADING(PLAYER_PED_ID()));
		if (!veh)
		{
			FailMission("could not spawn chase vehicle");
			return;
		}

		Ped driver = SpawnContractPed(GET_HASH_KEY("s_m_m_highsec_01"), pos, 0.0f, WEAPON_PISTOL, false);
		Ped target = SpawnContractPed(GET_HASH_KEY("a_m_m_soucent_03"), pos, 0.0f, 0, true);
		if (driver) SET_PED_INTO_VEHICLE(driver, veh, -1);
		if (target) SET_PED_INTO_VEHICLE(target, veh, 0);
		if (driver)
		{
			TASK_VEHICLE_DRIVE_WANDER(driver, veh, 22.0f, 786603); // careful
			SET_DRIVER_ABILITY(driver, 0.7f);
			SET_DRIVER_AGGRESSIVENESS(driver, 0.15f);
		}

		Game::Print::PrintBottomLeft("~y~Mobile Hit:~s~ target in the car. Don't alert until you engage.");
	}

	static void StartBoatHit()
	{
		ResetMissionSilent();
		s_type = MissionType::BoatHit;
		s_reward = 35000;
		s_startedAt = GetTickCount();

		Vector3 pos = FindOutdoorSpawn(140.0f, 90.0f, true);
		// If no water found, try marina-ish known coastal offset from player
		float waterZ = 0.0f;
		if (!GET_WATER_HEIGHT(pos.x, pos.y, pos.z + 40.0f, &waterZ))
		{
			pos = FindOutdoorSpawn(160.0f, 180.0f, true);
			GET_WATER_HEIGHT(pos.x, pos.y, pos.z + 40.0f, &waterZ);
		}
		if (waterZ > 0.1f)
			pos.z = waterZ + 0.8f;

		SET_NEW_WAYPOINT(pos.x, pos.y);

		Hash boats[] = {
			GET_HASH_KEY("speeder"),
			GET_HASH_KEY("jetmax"),
			GET_HASH_KEY("toro")
		};
		Vehicle boat = SpawnVehicleAt(boats[rand() % 3], pos, GET_ENTITY_HEADING(PLAYER_PED_ID()));
		if (!boat)
		{
			FailMission("could not spawn boat");
			return;
		}
		SET_BOAT_ANCHOR(boat, false);
		SET_VEHICLE_ENGINE_ON(boat, true, true, false);

		Ped captain = SpawnContractPed(GET_HASH_KEY("s_m_y_baywatch_01"), pos, 0.0f, WEAPON_PISTOL, false);
		Ped target = SpawnContractPed(GET_HASH_KEY("u_m_y_baygor"), pos, 0.0f, 0, true);
		Ped gunner = SpawnContractPed(GET_HASH_KEY("s_m_y_baywatch_01"), pos, 0.0f, WEAPON_SMG, false);
		if (captain) SET_PED_INTO_VEHICLE(captain, boat, -1);
		if (target) SET_PED_INTO_VEHICLE(target, boat, 0);
		if (gunner) SET_PED_INTO_VEHICLE(gunner, boat, 1);
		if (captain)
			TASK_VEHICLE_DRIVE_WANDER(captain, boat, 18.0f, 16777216);

		Game::Print::PrintBottomLeft("~y~Boat Hit:~s~ VIP on the boat. Engage to start the fight.");
	}

	static void StartSniperHit()
	{
		ResetMissionSilent();
		s_type = MissionType::SniperHit;
		s_reward = 40000;
		s_startedAt = GetTickCount();

		// Always ground-level outdoor — no rooftop freeze (caused floating)
		Vector3 pos = FindOutdoorSpawn(120.0f, 50.0f, false);
		SET_NEW_WAYPOINT(pos.x, pos.y);

		Ped target = SpawnContractPed(GET_HASH_KEY("a_m_y_business_03"), pos, 180.0f, WEAPON_PISTOL, true);
		if (target)
		{
			FREEZE_ENTITY_POSITION(target, false);
			GiveIdleTask(target, MissionType::SniperHit);
		}

		for (int i = 0; i < 2; ++i)
		{
			Vector3 gpos(pos.x + (i ? 5.5f : -5.5f), pos.y + 3.5f, pos.z);
			float gz = gpos.z;
			GET_GROUND_Z_FOR_3D_COORD(gpos.x, gpos.y, gpos.z + 40.0f, &gz, FALSE, FALSE);
			gpos.z = gz + 1.0f;
			Ped g = SpawnContractPed(GET_HASH_KEY("s_m_m_highsec_02"), gpos, 0.0f, WEAPON_CARBINERIFLE, false);
			if (g)
				TASK_GUARD_CURRENT_POSITION(g, 12.0f, 12.0f, true);
		}

		Ped me = PLAYER_PED_ID();
		GIVE_WEAPON_TO_PED(me, WEAPON_SNIPERRIFLE, 40, false, true);
		SET_CURRENT_PED_WEAPON(me, WEAPON_SNIPERRIFLE, true);
		Game::Print::PrintBottomLeft("~y~Sniper Contract:~s~ long-range outdoor target. Rifle given.");
	}

	static int CountAlivePrimary()
	{
		int n = 0;
		for (const auto& t : s_targets)
		{
			if (!t.isPrimary) continue;
			if (DOES_ENTITY_EXIST(t.ped) && !IS_PED_DEAD_OR_DYING(t.ped, true) && !IS_ENTITY_DEAD(t.ped, false))
				++n;
		}
		return n;
	}

	static int CountAliveAny()
	{
		int n = 0;
		for (const auto& t : s_targets)
		{
			if (DOES_ENTITY_EXIST(t.ped) && !IS_PED_DEAD_OR_DYING(t.ped, true) && !IS_ENTITY_DEAD(t.ped, false))
				++n;
		}
		return n;
	}

	static bool PlayerProvokedTarget(Ped target)
	{
		const Player pl = PLAYER_ID();
		const Ped me = PLAYER_PED_ID();
		if (!DOES_ENTITY_EXIST(target) || !DOES_ENTITY_EXIST(me))
			return false;

		if (IS_PLAYER_FREE_AIMING_AT_ENTITY(pl, target))
			return true;
		if (HAS_ENTITY_BEEN_DAMAGED_BY_ENTITY(target, me, true))
			return true;

		// Shooting while facing them at close-mid range
		if (IS_PED_SHOOTING(me))
		{
			Vector3 a = GET_ENTITY_COORDS(me, true);
			Vector3 b = GET_ENTITY_COORDS(target, true);
			const float dx = a.x - b.x, dy = a.y - b.y, dz = a.z - b.z;
			const float dist2 = dx * dx + dy * dy + dz * dz;
			if (dist2 < 90.0f * 90.0f && HAS_ENTITY_CLEAR_LOS_TO_ENTITY(me, target, 17))
				return true;
		}
		return false;
	}

	void Tick()
	{
		if (s_type == MissionType::None)
			return;

		Ped me = PLAYER_PED_ID();
		if (!DOES_ENTITY_EXIST(me) || IS_PED_DEAD_OR_DYING(me, true))
		{
			FailMission("you died");
			return;
		}

		// Provocation check — only then go hostile
		if (!s_anyAlerted)
		{
			for (auto& t : s_targets)
			{
				if (!DOES_ENTITY_EXIST(t.ped) || IS_PED_DEAD_OR_DYING(t.ped, true))
					continue;
				if (PlayerProvokedTarget(t.ped))
				{
					AlertAll();
					Game::Print::PrintBottomCentre("~o~Targets alerted!");
					break;
				}
			}
		}

		for (auto& t : s_targets)
		{
			if (!DOES_ENTITY_EXIST(t.ped) || IS_PED_DEAD_OR_DYING(t.ped, true) || IS_ENTITY_DEAD(t.ped, false))
				ClearBlip(t.blip);
		}

		const bool wipeAll = (s_type == MissionType::GangAssault);
		const bool done = wipeAll ? (CountAliveAny() == 0) : (CountAlivePrimary() == 0);
		if (done)
		{
			CompleteMission();
			return;
		}

		const DWORD now = GetTickCount();
		if (now - s_lastHud > 2800)
		{
			s_lastHud = now;
			char hud[128];
			if (wipeAll)
				sprintf_s(hud, "~y~%s~s~ | Left: %d | $%d", TypeName(s_type), CountAliveAny(), s_reward);
			else
				sprintf_s(hud, "~y~%s~s~ | Target alive | $%d%s", TypeName(s_type), s_reward, s_anyAlerted ? " | ALERT" : " | STEALTH");
			Game::Print::PrintBottomCentre(hud);
		}

		if (now - s_startedAt > 15 * 60 * 1000)
			FailMission("time expired");
	}

	void Menu()
	{
		AddTitle("Contract Hits");

		if (IsActive())
		{
			char status[96];
			sprintf_s(status, "Active: %s", TypeName(s_type));
			AddBreak(status);
			bool abort = false;
			AddOption("~r~Abort Contract", abort);
			if (abort) Abort();
			AddBreak("---Start New (replaces current)---");
		}
		else
			AddBreak("---Choose Mission Type---");

		bool gang = false, solo = false, drive = false, boat = false, sniper = false;
		AddOption("Gang Assault", gang);
		if (Menu::printingop == *Menu::currentopATM)
			SetOptionTip("Outdoor gang. They fight only if you aim or shoot.");
		if (gang) StartGangAssault();

		AddOption("Solo Contract", solo);
		if (Menu::printingop == *Menu::currentopATM)
			SetOptionTip("One outdoor target + guard. Stealth until engaged.");
		if (solo) StartSoloHit();

		AddOption("Mobile Hit", drive);
		if (Menu::printingop == *Menu::currentopATM)
			SetOptionTip("Target in a car on the street. Engage to alert.");
		if (drive) StartDriveByHit();

		AddOption("Boat Hit", boat);
		if (Menu::printingop == *Menu::currentopATM)
			SetOptionTip("VIP on a boat. Spawns on water when possible.");
		if (boat) StartBoatHit();

		AddOption("Sniper Contract", sniper);
		if (Menu::printingop == *Menu::currentopATM)
			SetOptionTip("Outdoor long-range target. Sniper rifle given.");
		if (sniper) StartSniperHit();

		AddBreak("---Info---");
		AddOption("Targets stay calm until aimed/shot", null);
		AddOption("Spawns use street / outdoor coords", null);
	}
}

REGISTER_SUBMENU(HITMAN_CONTRACTS, HitmanContracts::Menu)
