/*
* Space Trainer - chaotic gradual Zombie Apocalypse
* Random pedestrians & drivers turn infected (keep their look).
* Kills still rise after a delay. Cops get overrun / converted too.
*/
#include "ZombieApocalypse.h"

#include "..\Menu\Menu.h"
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
#include <cstdlib>
#include <set>
#include <string>
#include <vector>
#include <Windows.h>

namespace ZombieApocalypse
{
	struct PendingRise
	{
		Ped corpse = 0;
		Vector3 pos{};
		float heading = 0.0f;
		DWORD riseAt = 0;
		bool twitching = false;
		bool keepAppearance = true; // rise as same look / convert in place when possible
	};

	struct TrackedSurvivor
	{
		Ped ped = 0;
	};

	static bool s_active = false;
	static bool s_survivorsFight = true;
	static bool s_darkWeather = true;
	static bool s_chaosSpread = true;
	static bool s_infectDrivers = true;
	static bool s_infectCops = true;
	static int s_maxZombies = 55;
	static float s_huntRadius = 90.0f;
	static DWORD s_infectDelayMs = 5500;
	static Hash s_zombieGroup = 0;
	static Hash s_survivorGroup = 0;
	static std::vector<Ped> s_zombies;
	static std::vector<PendingRise> s_pending;
	static std::vector<TrackedSurvivor> s_survivors;
	static std::set<Ped> s_processedCorpses;
	static DWORD s_nextAiTick = 0;
	static DWORD s_nextSurvivorTick = 0;
	static DWORD s_nextChaosTick = 0;
	static DWORD s_nextCleanup = 0;
	static int s_totalInfected = 0;

	bool IsActive() { return s_active; }

	static bool IsInList(const std::vector<Ped>& list, Ped p)
	{
		return std::find(list.begin(), list.end(), p) != list.end();
	}

	static Hash ZombieGroup()
	{
		if (s_zombieGroup == 0)
			s_zombieGroup = World::AddRelationshipGroup("SPACE_ZOMBIE");
		return s_zombieGroup;
	}

	static Hash SurvivorGroup()
	{
		if (s_survivorGroup == 0)
			s_survivorGroup = World::AddRelationshipGroup("SPACE_SURVIVOR");
		return s_survivorGroup;
	}

	static void SetupRelationships()
	{
		const Hash z = ZombieGroup();
		const Hash s = SurvivorGroup();
		const Hash player = GET_HASH_KEY("PLAYER");
		const char* hateGroups[] = {
			"CIVMALE", "CIVFEMALE", "COP", "SECURITY_GUARD", "FIREMAN", "MEDIC",
			"GANG_1", "GANG_2", "GANG_9", "GANG_10", "AMBIENT_GANG_LOST",
			"AMBIENT_GANG_MEXICAN", "AMBIENT_GANG_FAMILY", "AMBIENT_GANG_BALLAD",
			"DEALER", "HATES_PLAYER", "PRISONER", "DOMESTIC_ANIMAL"
		};

		auto hate = [](Hash a, Hash b) {
			SET_RELATIONSHIP_BETWEEN_GROUPS(5, a, b);
			SET_RELATIONSHIP_BETWEEN_GROUPS(5, b, a);
		};
		auto like = [](Hash a, Hash b) {
			SET_RELATIONSHIP_BETWEEN_GROUPS(0, a, b);
			SET_RELATIONSHIP_BETWEEN_GROUPS(0, b, a);
		};

		SET_RELATIONSHIP_BETWEEN_GROUPS(1, z, z);
		hate(z, player);
		hate(z, s);
		for (const char* g : hateGroups)
			hate(z, GET_HASH_KEY(g));

		like(s, player);
		like(s, s);
		hate(s, z);
	}

	static bool IsZombiePed(Ped ped)
	{
		if (!DOES_ENTITY_EXIST(ped))
			return false;
		if (IsInList(s_zombies, ped))
			return true;
		return GET_PED_RELATIONSHIP_GROUP_HASH(ped) == ZombieGroup();
	}

	static bool IsSurvivorPed(Ped ped)
	{
		if (!DOES_ENTITY_EXIST(ped))
			return false;
		if (GET_PED_RELATIONSHIP_GROUP_HASH(ped) == SurvivorGroup())
			return true;
		for (const auto& s : s_survivors)
		{
			if (s.ped == ped)
				return true;
		}
		return false;
	}

	static void AssignZombieCombat(Ped ped)
	{
		CLEAR_PED_TASKS_IMMEDIATELY(ped);
		SET_PED_KEEP_TASK(ped, true);
		SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(ped, true);

		if (IS_PED_IN_ANY_VEHICLE(ped, false))
		{
			Vehicle v = GET_VEHICLE_PED_IS_IN(ped, false);
			SET_DRIVER_AGGRESSIVENESS(ped, 1.0f);
			SET_DRIVER_ABILITY(ped, 1.0f);
			SET_PED_KEEP_TASK(ped, true);
			// Aggressive drive / ram style chaos
			TASK_VEHICLE_DRIVE_WANDER(ped, v, 40.0f, 787004);
			return;
		}

		TASK_COMBAT_HATED_TARGETS_AROUND_PED(ped, s_huntRadius, 0);
	}

	static void ConfigureInfectedPed(Ped ped, bool fromVehicle)
	{
		if (!DOES_ENTITY_EXIST(ped) || IS_PED_A_PLAYER(ped))
			return;

		SET_ENTITY_AS_MISSION_ENTITY(ped, true, true);
		SET_PED_RELATIONSHIP_GROUP_HASH(ped, ZombieGroup());
		SET_PED_AS_ENEMY(ped, true);
		SET_PED_COMBAT_ABILITY(ped, 2);
		SET_PED_COMBAT_MOVEMENT(ped, 2);
		SET_PED_COMBAT_RANGE(ped, 2);
		SET_PED_COMBAT_ATTRIBUTES(ped, 5, true);
		SET_PED_COMBAT_ATTRIBUTES(ped, 46, true);
		SET_PED_COMBAT_ATTRIBUTES(ped, 0, false);
		SET_PED_COMBAT_ATTRIBUTES(ped, 1, false);
		SET_PED_COMBAT_ATTRIBUTES(ped, 3, true); // can use vehicles
		SET_PED_FLEE_ATTRIBUTES(ped, 0, false);
		SET_PED_SUFFERS_CRITICAL_HITS(ped, false);
		SET_PED_CAN_BE_TARGETTED(ped, true);
		SET_PED_CONFIG_FLAG(ped, 281, true);
		SET_ENTITY_MAX_HEALTH(ped, 450);
		SET_ENTITY_HEALTH(ped, 450, 0);
		SET_PED_ARMOUR(ped, 40);
		SET_PED_ACCURACY(ped, 5);

		// Strip guns so they "bite"/melee — drivers keep vehicle
		if (!fromVehicle)
		{
			REMOVE_ALL_PED_WEAPONS(ped, true);
			SET_PED_PATH_CAN_USE_CLIMBOVERS(ped, true);
		}

		if (!IsInList(s_zombies, ped))
		{
			s_zombies.push_back(ped);
			++s_totalInfected;
		}

		AssignZombieCombat(ped);
	}

	static Ped FindNearestPrey(Ped zombie, float radius)
	{
		Ped me = PLAYER_PED_ID();
		Ped best = 0;
		float bestDist = radius * radius;

		std::vector<GTAped> nearby;
		World::GetNearbyPeds(nearby, GTAped(zombie), radius, 28);
		for (auto& p : nearby)
		{
			if (!p.Exists())
				continue;
			Ped h = p.GetHandle();
			if (h == zombie || IS_PED_DEAD_OR_DYING(h, true))
				continue;
			if (IsZombiePed(h))
				continue;

			Vector3 a = GET_ENTITY_COORDS(zombie, true);
			Vector3 b = GET_ENTITY_COORDS(h, true);
			const float dx = a.x - b.x, dy = a.y - b.y, dz = a.z - b.z;
			const float d2 = dx * dx + dy * dy + dz * dz;
			if (d2 < bestDist)
			{
				bestDist = d2;
				best = h;
			}
		}

		if (best == 0 && DOES_ENTITY_EXIST(me) && !IS_PED_DEAD_OR_DYING(me, true))
			best = me;
		return best;
	}

	static bool CanInfectTarget(Ped h, Ped me)
	{
		if (!DOES_ENTITY_EXIST(h) || h == me)
			return false;
		if (IS_PED_A_PLAYER(h) || IS_PED_DEAD_OR_DYING(h, true))
			return false;
		if (IsZombiePed(h) || IsSurvivorPed(h))
			return false;
		return true;
	}

	static void InfectLivingPed(Ped h)
	{
		if (!DOES_ENTITY_EXIST(h) || (int)s_zombies.size() >= s_maxZombies)
			return;

		const bool inVeh = IS_PED_IN_ANY_VEHICLE(h, false);
		// Short stumble then turn
		if (!inVeh)
		{
			SET_PED_TO_RAGDOLL(h, 900, 900, 0, false, false, false);
		}
		ConfigureInfectedPed(h, inVeh);
	}

	static Ped InfectNearestAmbientAsPatientZero()
	{
		Ped me = PLAYER_PED_ID();
		std::vector<GTAped> nearby;
		World::GetNearbyPeds(nearby, GTAped(me), 45.0f, 30);
		std::vector<Ped> candidates;
		for (auto& p : nearby)
		{
			if (!p.Exists())
				continue;
			Ped h = p.GetHandle();
			if (!CanInfectTarget(h, me))
				continue;
			candidates.push_back(h);
		}
		if (candidates.empty())
			return 0;

		Ped pick = candidates[std::rand() % candidates.size()];
		InfectLivingPed(pick);
		return pick;
	}

	static void ChaosInfectRandomNearby()
	{
		if (!s_chaosSpread || (int)s_zombies.size() >= s_maxZombies)
			return;

		Ped me = PLAYER_PED_ID();
		std::vector<GTAped> nearby;
		World::GetNearbyPeds(nearby, GTAped(me), s_huntRadius, 36);
		std::vector<Ped> walkers;
		std::vector<Ped> drivers;
		std::vector<Ped> cops;

		for (auto& p : nearby)
		{
			if (!p.Exists())
				continue;
			Ped h = p.GetHandle();
			if (!CanInfectTarget(h, me))
				continue;

			Hash model = GET_ENTITY_MODEL(h);
			const bool isCopish =
				IS_PED_MODEL(h, GET_HASH_KEY("s_m_y_cop_01")) ||
				IS_PED_MODEL(h, GET_HASH_KEY("s_f_y_cop_01")) ||
				IS_PED_MODEL(h, GET_HASH_KEY("s_m_y_hwaycop_01")) ||
				IS_PED_MODEL(h, GET_HASH_KEY("s_m_y_sheriff_01")) ||
				IS_PED_MODEL(h, GET_HASH_KEY("s_f_y_sheriff_01")) ||
				IS_PED_MODEL(h, GET_HASH_KEY("s_m_m_security_01")) ||
				GET_PED_TYPE(h) == 6; // cop ped type

			if (IS_PED_IN_ANY_VEHICLE(h, false))
			{
				if (GET_PED_IN_VEHICLE_SEAT(GET_VEHICLE_PED_IS_IN(h, false), -1, false) == h)
					drivers.push_back(h);
			}
			else if (isCopish)
				cops.push_back(h);
			else
				walkers.push_back(h);
		}

		int converted = 0;
		auto take = [&](std::vector<Ped>& pool, int count) {
			while (converted < count && !pool.empty() && (int)s_zombies.size() < s_maxZombies)
			{
				const int i = std::rand() % (int)pool.size();
				Ped h = pool[i];
				pool.erase(pool.begin() + i);
				InfectLivingPed(h);
				++converted;
			}
		};

		// Prefer walkers, then drivers, then cops — full chaos
		take(walkers, 1 + (std::rand() % 2));
		if (s_infectDrivers)
			take(drivers, (std::rand() % 100) < 55 ? 1 : 0);
		if (s_infectCops)
			take(cops, (std::rand() % 100) < 45 ? 1 : 0);
	}

	static bool WasKilledByZombie(Ped corpse)
	{
		if (!DOES_ENTITY_EXIST(corpse))
			return false;

		Entity src = GET_PED_SOURCE_OF_DEATH(corpse);
		if (src && IsZombiePed((Ped)src))
			return true;

		for (Ped z : s_zombies)
		{
			if (!DOES_ENTITY_EXIST(z))
				continue;
			if (HAS_ENTITY_BEEN_DAMAGED_BY_ENTITY(corpse, z, true))
				return true;
			Vector3 a = GET_ENTITY_COORDS(corpse, true);
			Vector3 b = GET_ENTITY_COORDS(z, true);
			const float dx = a.x - b.x, dy = a.y - b.y, dz = a.z - b.z;
			if (dx * dx + dy * dy + dz * dz < 4.5f * 4.5f)
				return true;
		}
		return false;
	}

	static bool AlreadyPending(Ped corpse)
	{
		for (const auto& p : s_pending)
		{
			if (p.corpse == corpse)
				return true;
		}
		return false;
	}

	static void QueueInfectionFromCorpse(Ped corpse)
	{
		if (!DOES_ENTITY_EXIST(corpse))
			return;
		if (IsZombiePed(corpse) || AlreadyPending(corpse))
			return;
		if (s_processedCorpses.count(corpse))
			return;
		if (IS_PED_A_PLAYER(corpse))
			return;
		if ((int)s_zombies.size() + (int)s_pending.size() >= s_maxZombies)
			return;

		s_processedCorpses.insert(corpse);
		SET_ENTITY_AS_MISSION_ENTITY(corpse, true, true);
		SET_PED_TO_RAGDOLL(corpse, 4000, 4000, 0, false, false, false);

		PendingRise rise{};
		rise.corpse = corpse;
		rise.pos = GET_ENTITY_COORDS(corpse, true);
		rise.heading = GET_ENTITY_HEADING(corpse);
		rise.riseAt = GetTickCount() + s_infectDelayMs + (DWORD)(2000 + std::rand() % 3500);
		rise.twitching = false;
		rise.keepAppearance = true;
		s_pending.push_back(rise);
	}

	static void ScanForNewVictims()
	{
		Ped me = PLAYER_PED_ID();
		std::vector<GTAped> nearby;
		World::GetNearbyPeds(nearby, GTAped(me), s_huntRadius + 15.0f, 48);
		for (auto& p : nearby)
		{
			if (!p.Exists())
				continue;
			Ped h = p.GetHandle();
			if (h == me || !IS_PED_DEAD_OR_DYING(h, true))
				continue;
			if (IsZombiePed(h))
				continue;
			if (WasKilledByZombie(h))
				QueueInfectionFromCorpse(h);
		}
	}

	static void ProcessPendingRises(DWORD now)
	{
		std::vector<PendingRise> keep;
		keep.reserve(s_pending.size());

		for (auto& rise : s_pending)
		{
			if (!DOES_ENTITY_EXIST(rise.corpse))
			{
				if (now >= rise.riseAt)
					continue; // lost corpse — skip (chaos spread will refill)
				keep.push_back(rise);
				continue;
			}

			if (!rise.twitching && rise.riseAt > now && (rise.riseAt - now) < 1800u)
			{
				rise.twitching = true;
				SET_PED_TO_RAGDOLL(rise.corpse, 1600, 1600, 0, false, false, false);
				APPLY_FORCE_TO_ENTITY(rise.corpse, 1, 0.0f, 0.0f, 0.7f, 0, 0, 0, 0, false, true, true, false, true);
			}

			if (now < rise.riseAt)
			{
				keep.push_back(rise);
				continue;
			}

			// Revive same ped as infected (keeps appearance — no model swap vanish)
			Ped corpse = rise.corpse;
			RESURRECT_PED(corpse);
			REVIVE_INJURED_PED(corpse);
			CLEAR_PED_TASKS_IMMEDIATELY(corpse);
			SET_ENTITY_COORDS_NO_OFFSET(corpse, rise.pos.x, rise.pos.y, rise.pos.z + 0.2f, false, false, false);
			SET_ENTITY_HEADING(corpse, rise.heading);
			SET_PED_TO_RAGDOLL(corpse, 700, 700, 0, false, false, false);
			ConfigureInfectedPed(corpse, false);
		}

		s_pending.swap(keep);
	}

	static void UpdateZombieAI(DWORD now)
	{
		if (now < s_nextAiTick)
			return;
		s_nextAiTick = now + 1100u;

		for (Ped z : s_zombies)
		{
			if (!DOES_ENTITY_EXIST(z) || IS_PED_DEAD_OR_DYING(z, true))
				continue;

			if (IS_PED_IN_ANY_VEHICLE(z, false))
			{
				Vehicle v = GET_VEHICLE_PED_IS_IN(z, false);
				SET_DRIVER_AGGRESSIVENESS(z, 1.0f);
				// Occasionally chase player / nearest prey with vehicle
				Ped prey = FindNearestPrey(z, s_huntRadius);
				if (prey && (std::rand() % 100) < 40)
					TASK_VEHICLE_MISSION_PED_TARGET(z, v, prey, 6, 45.0f, 787004, 2.0f, 2.0f, true);
				continue;
			}

			Ped prey = FindNearestPrey(z, s_huntRadius);
			if (prey && !IS_PED_IN_COMBAT(z, prey))
				TASK_COMBAT_PED(z, prey, 0, 16);
			else if (!prey)
				TASK_COMBAT_HATED_TARGETS_AROUND_PED(z, s_huntRadius, 0);
		}
	}

	static void TryArmSurvivors(DWORD now)
	{
		if (!s_survivorsFight || now < s_nextSurvivorTick)
			return;
		s_nextSurvivorTick = now + 3200u;
		if (s_zombies.empty())
			return;

		Ped me = PLAYER_PED_ID();
		std::vector<GTAped> nearby;
		World::GetNearbyPeds(nearby, GTAped(me), 60.0f, 22);

		int armed = 0;
		for (auto& p : nearby)
		{
			if (!p.Exists() || armed >= 2)
				break;
			Ped h = p.GetHandle();
			if (!CanInfectTarget(h, me))
				continue;
			if (IS_PED_IN_ANY_VEHICLE(h, false))
				continue;
			if ((std::rand() % 100) > 28)
				continue;

			bool zombieNear = false;
			Vector3 pp = GET_ENTITY_COORDS(h, true);
			for (Ped z : s_zombies)
			{
				if (!DOES_ENTITY_EXIST(z) || IS_PED_DEAD_OR_DYING(z, true))
					continue;
				Vector3 zp = GET_ENTITY_COORDS(z, true);
				const float dx = pp.x - zp.x, dy = pp.y - zp.y, dz = pp.z - zp.z;
				if (dx * dx + dy * dy + dz * dz < 40.0f * 40.0f)
				{
					zombieNear = true;
					break;
				}
			}
			if (!zombieNear)
				continue;

			SET_ENTITY_AS_MISSION_ENTITY(h, true, true);
			SET_PED_RELATIONSHIP_GROUP_HASH(h, SurvivorGroup());
			SET_PED_COMBAT_ABILITY(h, 2);
			SET_PED_COMBAT_ATTRIBUTES(h, 46, true);
			SET_PED_ACCURACY(h, 40 + (std::rand() % 25));
			SET_PED_KEEP_TASK(h, true);
			SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(h, true);
			const Hash guns[] = { WEAPON_PISTOL, WEAPON_COMBATPISTOL, WEAPON_MICROSMG, WEAPON_PUMPSHOTGUN, WEAPON_BAT };
			Hash gun = guns[std::rand() % 5];
			GIVE_WEAPON_TO_PED(h, gun, 90, false, true);
			SET_CURRENT_PED_WEAPON(h, gun, true);
			TASK_COMBAT_HATED_TARGETS_AROUND_PED(h, 55.0f, 0);
			s_survivors.push_back({ h });
			++armed;
		}
	}

	static void CleanupFarOrDead(DWORD now)
	{
		if (now < s_nextCleanup)
			return;
		s_nextCleanup = now + 3500u;

		Ped me = PLAYER_PED_ID();
		Vector3 myPos = GET_ENTITY_COORDS(me, true);

		std::vector<Ped> keepZ;
		keepZ.reserve(s_zombies.size());
		for (Ped z : s_zombies)
		{
			if (!DOES_ENTITY_EXIST(z))
				continue;
			if (IS_PED_DEAD_OR_DYING(z, true))
			{
				SET_PED_AS_NO_LONGER_NEEDED(&z);
				continue;
			}
			Vector3 zp = GET_ENTITY_COORDS(z, true);
			const float dx = myPos.x - zp.x, dy = myPos.y - zp.y, dz = myPos.z - zp.z;
			if (dx * dx + dy * dy + dz * dz > 250.0f * 250.0f)
			{
				SET_PED_AS_NO_LONGER_NEEDED(&z);
				continue;
			}
			keepZ.push_back(z);
		}
		s_zombies.swap(keepZ);

		std::vector<TrackedSurvivor> keepS;
		for (auto& s : s_survivors)
		{
			if (DOES_ENTITY_EXIST(s.ped) && !IS_PED_DEAD_OR_DYING(s.ped, true))
				keepS.push_back(s);
		}
		s_survivors.swap(keepS);

		if (s_processedCorpses.size() > 100)
			s_processedCorpses.clear();
	}

	void Stop()
	{
		s_active = false;
		for (Ped p : s_zombies)
		{
			if (DOES_ENTITY_EXIST(p))
				SET_PED_AS_NO_LONGER_NEEDED(&p);
		}
		s_zombies.clear();
		s_pending.clear();
		for (auto& s : s_survivors)
		{
			if (DOES_ENTITY_EXIST(s.ped))
				SET_PED_AS_NO_LONGER_NEEDED(&s.ped);
		}
		s_survivors.clear();
		s_processedCorpses.clear();
		s_totalInfected = 0;

		CLEAR_WEATHER_TYPE_PERSIST();
		SET_WEATHER_TYPE_NOW_PERSIST("CLEAR");
		SET_ARTIFICIAL_LIGHTS_STATE(FALSE);
		SET_MAX_WANTED_LEVEL(5);
	}

	void Tick()
	{
		if (!s_active)
			return;

		const DWORD now = GetTickCount();

		if (s_darkWeather)
		{
			SET_WEATHER_TYPE_NOW_PERSIST("FOGGY");
			SET_ARTIFICIAL_LIGHTS_STATE(TRUE);
		}

		// Chaos density — lots of people/cars to infect
		SET_PED_DENSITY_MULTIPLIER_THIS_FRAME(1.15f);
		SET_VEHICLE_DENSITY_MULTIPLIER_THIS_FRAME(1.05f);
		SET_SCENARIO_PED_DENSITY_MULTIPLIER_THIS_FRAME(1.0f, 1.0f);
		SET_CREATE_RANDOM_COPS(s_infectCops);
		SET_MAX_WANTED_LEVEL(0);
		CLEAR_PLAYER_WANTED_LEVEL(PLAYER_ID());

		ScanForNewVictims();
		ProcessPendingRises(now);
		UpdateZombieAI(now);
		TryArmSurvivors(now);
		CleanupFarOrDead(now);

		if (s_chaosSpread && now >= s_nextChaosTick)
		{
			s_nextChaosTick = now + 2200u + (DWORD)(std::rand() % 1800);
			ChaosInfectRandomNearby();
		}
	}

	void Menu()
	{
		AddTitle("Zombie Apocalypse");

		bool stop = false;
		AddOption(s_active ? "~r~Stop Outbreak" : "Start Outbreak (Chaos)", stop);
		if (Menu::printingop == *Menu::currentopATM)
			SetOptionTip(s_active
				? "Clear infection and restore the world."
				: "Random pedestrians/drivers turn. Kill → rise. Full chaos.");
		if (stop)
		{
			if (s_active)
			{
				Stop();
				Game::Print::PrintBottomLeft("~g~Outbreak stopped.");
			}
			else
			{
				s_active = true;
				s_pending.clear();
				s_survivors.clear();
				s_processedCorpses.clear();
				s_zombies.clear();
				s_totalInfected = 0;
				s_nextAiTick = GetTickCount();
				s_nextSurvivorTick = GetTickCount() + 3500;
				s_nextChaosTick = GetTickCount() + 1500;
				s_nextCleanup = GetTickCount() + 4000;
				SetupRelationships();

				Ped z = InfectNearestAmbientAsPatientZero();
				if (z)
					Game::Print::PrintBottomCentre("~r~Infection started — chaos spreading...");
				else
				{
					// No nearby ped yet — chaos tick will catch the first one
					Game::Print::PrintBottomCentre("~y~Waiting for pedestrians to infect...");
				}
			}
		}

		AddToggle("Chaos Spread (walkers/drivers)", s_chaosSpread);
		AddToggle("Infect Drivers", s_infectDrivers);
		AddToggle("Infect Cops Too", s_infectCops);
		AddToggle("Armed Survivors", s_survivorsFight);
		AddToggle("Fog + Blackout Mood", s_darkWeather);

		bool maxP = false, maxM = false;
		AddNumber("Max Infected", (float)s_maxZombies, 0, null, maxP, maxM);
		if (maxP && s_maxZombies < 100) s_maxZombies += 5;
		if (maxM && s_maxZombies > 10) s_maxZombies -= 5;

		bool dP = false, dM = false;
		float delaySec = (float)s_infectDelayMs / 1000.0f;
		AddNumber("Rise Delay (sec)", delaySec, 1, null, dP, dM);
		if (dP && s_infectDelayMs < 12000) s_infectDelayMs += 500;
		if (dM && s_infectDelayMs > 2500) s_infectDelayMs -= 500;

		char st[96];
		sprintf_s(st, "Alive: %u | Rising: %u | Total: %d",
			(unsigned)s_zombies.size(), (unsigned)s_pending.size(), s_totalInfected);
		AddOption(st, null);
		sprintf_s(st, "Armed survivors: %u", (unsigned)s_survivors.size());
		AddOption(st, null);
	}
}

REGISTER_SUBMENU(ZOMBIE_APOCALYPSE, ZombieApocalypse::Menu)
