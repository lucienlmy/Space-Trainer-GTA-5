/*
* Space Trainer - Firefighter & Cash Collector (Inkassator) professions
*/
#include "Professions.h"

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
#include "..\Submenus\PedModelChanger.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>
#include <Windows.h>

namespace Professions
{
	enum class Job : int { None = 0, Firefighter, Inkassator };

	static Job s_job = Job::None;
	static Vehicle s_jobVeh = 0;
	static Hash s_hostileGroup = 0;
	static std::vector<Ped> s_npcs;
	static std::vector<Vehicle> s_extraVehs;
	static std::vector<FireId> s_fires;
	static std::vector<Vector3> s_firePos;
	static std::vector<Blip> s_blips;
	static DWORD s_jobStarted = 0;
	static int s_firesOut = 0;
	static int s_firesTotal = 0;
	static int s_robbersLeft = 0;
	static bool s_heistActive = false;
	static bool s_defendActive = false;

	static Hash HostileGroup()
	{
		if (s_hostileGroup == 0)
			s_hostileGroup = World::AddRelationshipGroup("SPACE_PROF_HOSTILE");
		Hash playerGroup = GET_HASH_KEY("PLAYER");
		SET_RELATIONSHIP_BETWEEN_GROUPS(5, s_hostileGroup, playerGroup);
		SET_RELATIONSHIP_BETWEEN_GROUPS(5, playerGroup, s_hostileGroup);
		return s_hostileGroup;
	}

	static void ClearBlips()
	{
		for (Blip b : s_blips)
		{
			if (b && DOES_BLIP_EXIST(b))
				REMOVE_BLIP(&b);
		}
		s_blips.clear();
	}

	static void ClearNpcs(bool deleteAll)
	{
		for (Ped p : s_npcs)
		{
			if (deleteAll && DOES_ENTITY_EXIST(p))
			{
				SET_ENTITY_AS_MISSION_ENTITY(p, true, true);
				DELETE_PED(&p);
			}
		}
		s_npcs.clear();
	}

	static void ClearFires()
	{
		for (FireId f : s_fires)
		{
			if (f)
				REMOVE_SCRIPT_FIRE(f);
		}
		s_fires.clear();
		s_firePos.clear();
		s_firesOut = 0;
		s_firesTotal = 0;
	}

	static void DeleteJobVehicle()
	{
		if (DOES_ENTITY_EXIST(s_jobVeh))
		{
			SET_ENTITY_AS_MISSION_ENTITY(s_jobVeh, true, true);
			DELETE_VEHICLE(&s_jobVeh);
		}
		s_jobVeh = 0;
		for (Vehicle v : s_extraVehs)
		{
			if (DOES_ENTITY_EXIST(v))
			{
				SET_ENTITY_AS_MISSION_ENTITY(v, true, true);
				DELETE_VEHICLE(&v);
			}
		}
		s_extraVehs.clear();
	}

	static void GiveCashCases(int amount)
	{
		Ped me = PLAYER_PED_ID();
		Vector3 p = GET_ENTITY_COORDS(me, true);
		const int per = (std::max)(400, amount / 5);
		for (int i = 0; i < 5; ++i)
		{
			CREATE_AMBIENT_PICKUP(
				GET_HASH_KEY("PICKUP_MONEY_CASE"),
				p.x + (i % 3) * 0.45f,
				p.y + (i / 3) * 0.45f,
				p.z + 0.35f,
				0, per, 1, false, true);
		}
	}

	static Vehicle SpawnJobVehicle(const char* modelName, float forwardOffset)
	{
		Ped me = PLAYER_PED_ID();
		Vector3 pos = GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(me, 0.0f, forwardOffset, 0.2f);
		float heading = GET_ENTITY_HEADING(me);
		GTAmodel::Model model(modelName);
		if (!model.IsInCdImage())
			return 0;
		GTAvehicle v = World::CreateVehicle(model, pos, heading, true);
		if (!v.Exists())
			return 0;
		Vehicle h = v.Handle();
		SET_ENTITY_AS_MISSION_ENTITY(h, true, true);
		SET_VEHICLE_ON_GROUND_PROPERLY(h, 5.0f);
		SET_VEHICLE_ENGINE_ON(h, true, true, false);
		SET_VEHICLE_HAS_BEEN_OWNED_BY_PLAYER(h, true);
		return h;
	}

	static void BecomeFirefighter()
	{
		StopAll();
		s_job = Job::Firefighter;
		sub::ChangeModel(GTAmodel::Model("s_m_y_fireman_01"));
		Ped me = PLAYER_PED_ID();
		SET_PED_ARMOUR(me, 50);
		GIVE_WEAPON_TO_PED(me, WEAPON_FIREEXTINGUISHER, 2000, false, true);
		GIVE_WEAPON_TO_PED(me, WEAPON_FLASHLIGHT, 1, false, false);
		SET_CURRENT_PED_WEAPON(me, WEAPON_FIREEXTINGUISHER, true);
		DeleteJobVehicle();
		s_jobVeh = SpawnJobVehicle("firetruk", 6.5f);
		Game::Print::PrintBottomCentre("~o~Firefighter ready — extinguisher + fire truck.");
	}

	static void BecomeInkassator()
	{
		StopAll();
		s_job = Job::Inkassator;
		// Prefer classic armored guard model; fallback to alt / MP variant.
		GTAmodel::Model m1("s_m_m_armoured_01");
		if (!m1.IsInCdImage())
			m1 = GTAmodel::Model("s_m_m_armoured_02");
		if (!m1.IsInCdImage())
			m1 = GTAmodel::Model("mp_s_m_armoured_01");
		sub::ChangeModel(m1);

		Ped me = PLAYER_PED_ID();
		SET_PED_ARMOUR(me, 100);
		SET_ENTITY_MAX_HEALTH(me, 250);
		SET_ENTITY_HEALTH(me, 250, 0);
		GIVE_WEAPON_TO_PED(me, WEAPON_COMBATPISTOL, 120, false, true);
		GIVE_WEAPON_TO_PED(me, WEAPON_PUMPSHOTGUN, 40, false, false);
		GIVE_WEAPON_TO_PED(me, WEAPON_NIGHTSTICK, 1, false, false);
		SET_CURRENT_PED_WEAPON(me, WEAPON_COMBATPISTOL, true);
		DeleteJobVehicle();
		s_jobVeh = SpawnJobVehicle("stockade", 7.0f);
		Game::Print::PrintBottomCentre("~b~Inkassator ready — armored van + duty weapons.");
	}

	static void StartFireCall()
	{
		if (s_job != Job::Firefighter)
			BecomeFirefighter();

		ClearFires();
		ClearBlips();
		Ped me = PLAYER_PED_ID();
		Vector3 base = GET_ENTITY_COORDS(me, true);
		s_firesTotal = 6 + (std::rand() % 4);
		s_firesOut = 0;
		s_jobStarted = GetTickCount();

		for (int i = 0; i < s_firesTotal; ++i)
		{
			const float ang = ((float)(std::rand() % 360)) * 0.01745329251f;
			const float dist = 12.0f + (float)(std::rand() % 28);
			Vector3 p(base.x + std::cos(ang) * dist, base.y + std::sin(ang) * dist, base.z);
			float gz = p.z;
			if (GET_GROUND_Z_FOR_3D_COORD(p.x, p.y, p.z + 40.0f, &gz, false, false))
				p.z = gz;
			FireId f = START_SCRIPT_FIRE(p.x, p.y, p.z, 8, false);
			s_fires.push_back(f);
			s_firePos.push_back(p);
			Blip b = ADD_BLIP_FOR_COORD(p.x, p.y, p.z);
			SET_BLIP_COLOUR(b, 1); // red
			SET_BLIP_SCALE(b, 0.75f);
			SET_BLIP_AS_SHORT_RANGE(b, false);
			BEGIN_TEXT_COMMAND_SET_BLIP_NAME("STRING");
			ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME("Fire");
			END_TEXT_COMMAND_SET_BLIP_NAME(b);
			s_blips.push_back(b);
		}
		Game::Print::PrintBottomLeft("~o~Fire call: extinguish all marked fires.");
	}

	static Ped SpawnRobber(const Vector3& pos, float heading)
	{
		static const char* models[] = {
			"g_m_y_lost_01", "g_m_y_lost_02", "g_m_y_mexgoon_01",
			"g_m_y_ballaorig_01", "a_m_m_hillbilly_01", "g_m_y_korean_01"
		};
		GTAmodel::Model model(models[std::rand() % 6]);
		if (!model.IsInCdImage())
			return 0;
		GTAped ped = World::CreatePed(model, pos, heading, true);
		if (!ped.Exists())
			return 0;
		Ped h = ped.Handle();
		SET_ENTITY_AS_MISSION_ENTITY(h, true, true);
		SET_PED_RELATIONSHIP_GROUP_HASH(h, HostileGroup());
		SET_PED_AS_ENEMY(h, true);
		SET_PED_COMBAT_ABILITY(h, 2);
		SET_PED_COMBAT_ATTRIBUTES(h, 46, true);
		SET_PED_ACCURACY(h, 40);
		SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(h, true);
		GIVE_WEAPON_TO_PED(h, (std::rand() % 2) ? WEAPON_MICROSMG : WEAPON_PISTOL, 180, false, true);
		TASK_COMBAT_PED(h, PLAYER_PED_ID(), 0, 16);
		s_npcs.push_back(h);
		return h;
	}

	static void StartDefendDelivery()
	{
		if (s_job != Job::Inkassator)
			BecomeInkassator();

		ClearNpcs(true);
		ClearBlips();
		s_heistActive = false;
		s_defendActive = true;
		s_jobStarted = GetTickCount();
		s_robbersLeft = 0;

		Ped me = PLAYER_PED_ID();
		Vector3 base = GET_ENTITY_COORDS(me, true);
		const int count = 5 + (std::rand() % 3);
		for (int i = 0; i < count; ++i)
		{
			const float ang = ((float)(std::rand() % 360)) * 0.01745329251f;
			const float dist = 18.0f + (float)(std::rand() % 16);
			Vector3 p(base.x + std::cos(ang) * dist, base.y + std::sin(ang) * dist, base.z);
			float gz = p.z;
			if (GET_GROUND_Z_FOR_3D_COORD(p.x, p.y, p.z + 40.0f, &gz, false, false))
				p.z = gz + 1.0f;
			if (SpawnRobber(p, (float)(std::rand() % 360)))
				++s_robbersLeft;
		}
		Game::Print::PrintBottomLeft("~b~Defend the cash van — robbers incoming!");
	}

	static void StartArmoredHeist()
	{
		ClearNpcs(true);
		ClearBlips();
		s_defendActive = false;
		s_heistActive = true;
		s_jobStarted = GetTickCount();
		s_robbersLeft = 0;

		Ped me = PLAYER_PED_ID();
		Vector3 base = GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(me, 0.0f, 35.0f, 0.0f);
		float gz = base.z;
		if (GET_GROUND_Z_FOR_3D_COORD(base.x, base.y, base.z + 50.0f, &gz, false, false))
			base.z = gz;

		GTAmodel::Model stock("stockade");
		GTAvehicle van = World::CreateVehicle(stock, base, GET_ENTITY_HEADING(me), true);
		if (van.Exists())
		{
			Vehicle vh = van.Handle();
			SET_ENTITY_AS_MISSION_ENTITY(vh, true, true);
			SET_VEHICLE_DOORS_LOCKED(vh, 2);
			s_extraVehs.push_back(vh);
			Blip vb = ADD_BLIP_FOR_ENTITY(vh);
			SET_BLIP_COLOUR(vb, 2);
			SET_BLIP_SCALE(vb, 0.9f);
			s_blips.push_back(vb);
		}

		// Guards / crew around the van (armored models)
		static const char* guards[] = { "s_m_m_armoured_01", "s_m_m_armoured_02", "s_m_m_security_01" };
		for (int i = 0; i < 4; ++i)
		{
			const float ang = i * 1.5708f;
			Vector3 p(base.x + std::cos(ang) * 4.5f, base.y + std::sin(ang) * 4.5f, base.z + 1.0f);
			GTAmodel::Model gm(guards[i % 3]);
			if (!gm.IsInCdImage())
				continue;
			GTAped ped = World::CreatePed(gm, p, ang * 57.3f, true);
			if (!ped.Exists())
				continue;
			Ped h = ped.Handle();
			SET_ENTITY_AS_MISSION_ENTITY(h, true, true);
			SET_PED_RELATIONSHIP_GROUP_HASH(h, HostileGroup());
			SET_PED_AS_ENEMY(h, true);
			SET_PED_ARMOUR(h, 80);
			GIVE_WEAPON_TO_PED(h, WEAPON_PUMPSHOTGUN, 40, false, true);
			TASK_COMBAT_PED(h, me, 0, 16);
			s_npcs.push_back(h);
			++s_robbersLeft;
		}

		// Become robber look? Keep player model; optional tip.
		Game::Print::PrintBottomLeft("~y~Armored van heist — eliminate guards and take the cash.");
	}

	void StopAll()
	{
		ClearFires();
		ClearBlips();
		ClearNpcs(true);
		DeleteJobVehicle();
		s_heistActive = false;
		s_defendActive = false;
		s_job = Job::None;
		s_robbersLeft = 0;
	}

	void Tick()
	{
		const DWORD now = GetTickCount();

		// Fire call progress
		if (s_job == Job::Firefighter && !s_firePos.empty())
		{
			int alive = 0;
			for (size_t i = 0; i < s_firePos.size(); ++i)
			{
				const Vector3& p = s_firePos[i];
				const int n = GET_NUMBER_OF_FIRES_IN_RANGE(p.x, p.y, p.z, 2.5f);
				if (n <= 0)
				{
					if (i < s_blips.size() && s_blips[i] && DOES_BLIP_EXIST(s_blips[i]))
					{
						REMOVE_BLIP(&s_blips[i]);
						s_blips[i] = 0;
					}
					continue;
				}
				++alive;
			}
			s_firesOut = s_firesTotal - alive;
			if (alive == 0 && s_firesTotal > 0)
			{
				const int reward = 8000 + s_firesTotal * 1200;
				GiveCashCases(reward);
				ClearFires();
				ClearBlips();
				char msg[96];
				sprintf_s(msg, "~g~Fire call complete. Reward ~w~$%d", reward);
				Game::Print::PrintBottomLeft(msg);
			}
			else if ((now - s_jobStarted) > 180000u)
			{
				ClearFires();
				ClearBlips();
				Game::Print::PrintBottomLeft("~r~Fire call timed out.");
			}
		}

		// Defend / heist progress
		if (s_defendActive || s_heistActive)
		{
			int alive = 0;
			for (Ped p : s_npcs)
			{
				if (DOES_ENTITY_EXIST(p) && !IS_PED_DEAD_OR_DYING(p, true))
					++alive;
			}
			s_robbersLeft = alive;
			if (alive == 0 && s_npcs.size() > 0)
			{
				const bool wasHeist = s_heistActive;
				const int reward = wasHeist ? 45000 : 22000;
				GiveCashCases(reward);
				ClearNpcs(true);
				ClearBlips();
				s_defendActive = false;
				s_heistActive = false;
				char msg[96];
				sprintf_s(msg, "~g~%s complete. Reward ~w~$%d", wasHeist ? "Heist" : "Delivery defense", reward);
				Game::Print::PrintBottomLeft(msg);
			}
			else if ((now - s_jobStarted) > 240000u)
			{
				ClearNpcs(true);
				ClearBlips();
				s_defendActive = false;
				s_heistActive = false;
				Game::Print::PrintBottomLeft("~r~Job timed out.");
			}
		}
	}

	void Menu()
	{
		AddTitle("Professions");

		bool stop = false;
		AddOption("Stop Job / Cleanup", stop);
		if (stop)
		{
			StopAll();
			Game::Print::PrintBottomLeft("~g~Profession cleaned up.");
		}

		AddBreak("---Firefighter---");
		bool fireOutfit = false, fireCall = false;
		AddOption("Become Firefighter", fireOutfit);
		if (Menu::printingop == *Menu::currentopATM)
			SetOptionTip("Model s_m_y_fireman_01 + firetruk + extinguisher.");
		if (fireOutfit) BecomeFirefighter();

		AddOption("Start Fire Call", fireCall);
		if (Menu::printingop == *Menu::currentopATM)
			SetOptionTip("Spawn realistic street fires — put them all out.");
		if (fireCall) StartFireCall();

		if (s_firesTotal > 0)
		{
			char st[64];
			sprintf_s(st, "Fires left: %d / %d", s_firesTotal - s_firesOut, s_firesTotal);
			AddOption(st, null);
		}

		AddBreak("---Inkassator (Cash Collector)---");
		bool inkOutfit = false, defend = false, heist = false;
		AddOption("Become Inkassator", inkOutfit);
		if (Menu::printingop == *Menu::currentopATM)
			SetOptionTip("Armored guard model + Stockade van + duty guns.");
		if (inkOutfit) BecomeInkassator();

		AddOption("Defend Cash Delivery", defend);
		if (Menu::printingop == *Menu::currentopATM)
			SetOptionTip("Robbers attack your armored van — survive and clear them.");
		if (defend) StartDefendDelivery();

		AddOption("Armored Van Robbery", heist);
		if (Menu::printingop == *Menu::currentopATM)
			SetOptionTip("Hit a Stockade with armored guards — steal the cash.");
		if (heist) StartArmoredHeist();

		if (s_defendActive || s_heistActive)
		{
			char st[64];
			sprintf_s(st, "Hostiles left: %d", s_robbersLeft);
			AddOption(st, null);
		}
	}
}

REGISTER_SUBMENU(PROFESSIONS, Professions::Menu)
