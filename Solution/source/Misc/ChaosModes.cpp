#include "ChaosModes.h"

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
#include "..\Submenus\AnimalRiding.h"
#include "..\Submenus\VehicleSpawner.h"
#include "..\Misc\SpaceExtras.h"
#include "..\Misc\SpaceProPack.h"

#include <vector>
#include <string>
#include <cmath>
#include <Windows.h>

namespace ChaosModes
{
	enum class Mode : int
	{
		None = 0,
		MadRuns,
		JetStunt,
		JohnWick,
		AlienInvasion,
		AnimalChaos
	};

	static Mode s_active = Mode::None;
	static Hash s_enemyGroup = 0;
	static std::vector<Ped> s_spawned;
	static DWORD s_alienNextHostile = 0;

	static void ClearSpawned()
	{
		for (Ped h : s_spawned)
		{
			if (DOES_ENTITY_EXIST(h))
			{
				SET_ENTITY_AS_MISSION_ENTITY(h, true, true);
				DELETE_ENTITY(&h);
			}
		}
		s_spawned.clear();
	}

	static Hash EnemyGroup()
	{
		if (s_enemyGroup == 0)
			s_enemyGroup = World::AddRelationshipGroup("SPACE_CHAOS_ENEMY");
		Hash playerGroup = GET_HASH_KEY("PLAYER");
		SET_RELATIONSHIP_BETWEEN_GROUPS(5, s_enemyGroup, playerGroup);
		SET_RELATIONSHIP_BETWEEN_GROUPS(5, playerGroup, s_enemyGroup);
		return s_enemyGroup;
	}

	static Ped SpawnHostilePed(Hash modelHash, const Vector3& pos, float heading, Hash weaponHash)
	{
		GTAmodel::Model model(modelHash);
		if (!model.IsInCdImage())
			return 0;
		GTAped ped = World::CreatePed(model, pos, heading, true);
		if (!ped.Exists())
			return 0;

		Ped h = ped.Handle();
		SET_ENTITY_AS_MISSION_ENTITY(h, true, true);
		SET_PED_RELATIONSHIP_GROUP_HASH(h, EnemyGroup());
		SET_PED_AS_ENEMY(h, true);
		SET_PED_COMBAT_ABILITY(h, 2);
		SET_PED_COMBAT_ATTRIBUTES(h, 46, true);
		SET_PED_ACCURACY(h, 45);
		SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(h, true);
		if (weaponHash)
		{
			GIVE_WEAPON_TO_PED(h, weaponHash, 250, false, true);
			SET_CURRENT_PED_WEAPON(h, weaponHash, true);
		}
		TASK_COMBAT_PED(h, PLAYER_PED_ID(), 0, 16);
		s_spawned.push_back(h);
		return h;
	}

	static void MakeNearbyHostile(float radius)
	{
		GTAped me = PLAYER_PED_ID();
		if (!me.Exists()) return;
		const Hash group = EnemyGroup();

		std::vector<GTAped> nearby;
		World::GetNearbyPeds(nearby, me, radius);
		for (auto& ped : nearby)
		{
			if (!ped.Exists() || ped.Handle() == me.Handle() || IS_PED_A_PLAYER(ped.Handle()) || IS_PED_DEAD_OR_DYING(ped.Handle(), true))
				continue;
			// Drivers getting combat tasks exit cars — skip vehicle occupants.
			if (IS_PED_IN_ANY_VEHICLE(ped.Handle(), true))
				continue;
			SET_PED_RELATIONSHIP_GROUP_HASH(ped.Handle(), group);
			SET_PED_AS_ENEMY(ped.Handle(), true);
			TASK_COMBAT_PED(ped.Handle(), me.Handle(), 0, 16);
		}
	}

	void StopAll()
	{
		ClearSpawned();
		s_active = Mode::None;
		SET_TIME_SCALE(1.0f);
		PAUSE_CLOCK(false);
		bulletTime = false;
		selfTriggerbot = false;
		SpaceExtras::oneHitKill = false;
		// Do not leave EMP blackout on — it forces civilians out of cars every frame.
		if (blackoutMode)
		{
			blackoutMode = false;
			SET_ARTIFICIAL_LIGHTS_STATE(FALSE);
		}
		CLEAR_TIMECYCLE_MODIFIER();
		Game::Print::PrintBottomLeft("Chaos mode stopped.");
	}

	static void ActivateMadRuns()
	{
		StopAll();
		s_active = Mode::MadRuns;

		explosiveRounds = true;
		bitInfiniteAmmo = true;
		playerInvincibility = true;

		GTAped me = PLAYER_PED_ID();
		Vector3 base = me.GetPosition();
		float heading = me.GetHeading();
		const Hash swat = GET_HASH_KEY("s_m_y_swat_01");
		const Hash rifle = GET_HASH_KEY("WEAPON_CARBINERIFLE");

		for (int i = 0; i < 8; ++i)
		{
			const float a = heading + (i * 45.0f);
			const float rad = a * 0.01745329251f;
			Vector3 pos(base.x + cosf(rad) * 18.0f, base.y + sinf(rad) * 18.0f, base.z);
			SpawnHostilePed(swat, pos, a + 180.0f, rifle);
		}

		Game::Print::PrintBottomLeft("Mad Runs: explosive ammo, infinite clip, SWAT squad.");
	}

	static void ActivateJetStunt()
	{
		StopAll();
		s_active = Mode::JetStunt;

		SET_CLOCK_TIME(0, 0, 0);
		PAUSE_CLOCK(true);
		SET_WEATHER_TYPE_NOW_PERSIST("CLEAR");

		GTAped me = PLAYER_PED_ID();
		Vehicle jet = sub::SpawnVehicle(GTAmodel::Model("lazer"), me, true, true);
		if (!DOES_ENTITY_EXIST(jet))
			jet = sub::SpawnVehicle(GTAmodel::Model("hydra"), me, true, true);

		if (DOES_ENTITY_EXIST(jet))
		{
			Vector3 pos = GET_ENTITY_COORDS(jet, true);
			SET_ENTITY_COORDS_NO_OFFSET(jet, pos.x, pos.y, pos.z + 220.0f, false, false, false);
			SET_VEHICLE_ENGINE_ON(jet, true, true, false);
			SET_VEHICLE_FORWARD_SPEED(jet, 60.0f);
		}

		Game::Print::PrintBottomLeft("Jet Stunt: midnight sky, fighter ready.");
	}

	static void ActivateJohnWick()
	{
		StopAll();
		s_active = Mode::JohnWick;

		bulletTime = true;
		selfTriggerbot = true;
		bitInfiniteAmmo = true;
		SpaceExtras::oneHitKill = true;
		playerNoRagdoll = true;
		SET_TIME_SCALE(0.42f);

		GTAped me = PLAYER_PED_ID();
		Ped h = me.Handle();
		GIVE_WEAPON_TO_PED(h, GET_HASH_KEY("WEAPON_PISTOL"), 999, false, true);
		GIVE_WEAPON_TO_PED(h, GET_HASH_KEY("WEAPON_COMBATPISTOL"), 999, false, true);
		GIVE_WEAPON_TO_PED(h, GET_HASH_KEY("WEAPON_PUMPSHOTGUN"), 80, false, true);
		SET_CURRENT_PED_WEAPON(h, GET_HASH_KEY("WEAPON_COMBATPISTOL"), true);
		SET_PED_ARMOUR(h, 100);
		SET_ENTITY_HEALTH(h, 200, 0);

		Game::Print::PrintBottomLeft("John Wick: slow-mo, triggerbot, one-hit kills.");
	}

	static void ActivateAlienInvasion()
	{
		StopAll();
		s_active = Mode::AlienInvasion;

		SET_WEATHER_TYPE_NOW_PERSIST("HALLOWEEN");
		SET_CLOCK_TIME(22, 30, 0);
		SET_TIMECYCLE_MODIFIER("ufo");
		SET_TIMECYCLE_MODIFIER_STRENGTH(0.85f);
		// Visual blackout only — full EMP mode ejects every driver from cars.
		SET_ARTIFICIAL_LIGHTS_STATE(TRUE);

		GTAped me = PLAYER_PED_ID();
		Vector3 base = me.GetPosition();
		Hash alien = GET_HASH_KEY("s_m_m_movalien_01");
		if (!IS_MODEL_IN_CDIMAGE(alien))
			alien = GET_HASH_KEY("u_m_y_rsranger_01");

		for (int i = 0; i < 6; ++i)
		{
			const float a = i * 60.0f;
			const float rad = a * 0.01745329251f;
			Vector3 pos(base.x + cosf(rad) * 14.0f, base.y + sinf(rad) * 14.0f, base.z);
			SpawnHostilePed(alien, pos, a + 180.0f, GET_HASH_KEY("WEAPON_RAYCARBINE"));
		}
		MakeNearbyHostile(60.0f);
		s_alienNextHostile = GetTickCount() + 4000;

		Game::Print::PrintBottomLeft("Alien Invasion: hostile streets, UFO sky.");
	}

	static void ActivateAnimalChaos()
	{
		StopAll();
		s_active = Mode::AnimalChaos;

		if (!sub::AnimalRiding::Enabled())
			sub::AnimalRiding::ToggleOnOff();

		GTAped me = PLAYER_PED_ID();
		Vector3 base = me.GetPosition();
		const Hash animals[] = {
			GET_HASH_KEY("a_c_mtlion"),
			GET_HASH_KEY("a_c_chop"),
			GET_HASH_KEY("a_c_husky"),
			GET_HASH_KEY("a_c_coyote"),
			GET_HASH_KEY("a_c_retriever"),
			GET_HASH_KEY("a_c_rottweiler")
		};

		for (int i = 0; i < 6; ++i)
		{
			const float a = i * 60.0f;
			const float rad = a * 0.01745329251f;
			Vector3 pos(base.x + cosf(rad) * 8.0f, base.y + sinf(rad) * 8.0f, base.z);
			GTAmodel::Model model(animals[i]);
			if (!model.IsInCdImage()) continue;
			GTAped animal = World::CreatePed(model, pos, a, true);
			if (!animal.Exists()) continue;
			Ped h = animal.Handle();
			SET_ENTITY_AS_MISSION_ENTITY(h, true, true);
			SET_PED_AS_ENEMY(h, true);
			SET_PED_RELATIONSHIP_GROUP_HASH(h, EnemyGroup());
			TASK_COMBAT_PED(h, me.Handle(), 0, 16);
			s_spawned.push_back(h);
		}

		Game::Print::PrintBottomLeft("Animal Chaos: riding enabled, animals nearby. Approach and mount.");
	}

	void Tick()
	{
		if (s_active == Mode::None)
			return;

		if (SpaceProPack::IsSafeMode())
		{
			StopAll();
			return;
		}

		if (s_active == Mode::JohnWick)
		{
			SET_TIME_SCALE(0.42f);
			bulletTime = true;
			selfTriggerbot = true;
			SpaceExtras::oneHitKill = true;
		}
		else if (s_active == Mode::AlienInvasion)
		{
			if (GetTickCount() >= s_alienNextHostile)
			{
				MakeNearbyHostile(45.0f);
				s_alienNextHostile = GetTickCount() + 10000;
			}
		}
		else if (s_active == Mode::MadRuns)
		{
			explosiveRounds = true;
			bitInfiniteAmmo = true;
		}
	}

	void Menu()
	{
		AddTitle("Chaos Modes");

		bool mad = false, jet = false, wick = false, alien = false, animal = false, stop = false;

		AddOption("Mad Runs", mad);
		if (Menu::printingop == *Menu::currentopATM)
			SetOptionTip("Explosive bullets, infinite ammo, fight a SWAT team.");
		if (mad) ActivateMadRuns();

		AddOption("Jet Stunt Mode", jet);
		if (Menu::printingop == *Menu::currentopATM)
			SetOptionTip("Spawn a fighter jet at midnight and fly the city.");
		if (jet) ActivateJetStunt();

		AddOption("John Wick Mode", wick);
		if (Menu::printingop == *Menu::currentopATM)
			SetOptionTip("Slow motion, triggerbot and lethal shots.");
		if (wick) ActivateJohnWick();

		AddOption("Alien Invasion", alien);
		if (Menu::printingop == *Menu::currentopATM)
			SetOptionTip("Weird weather, hostile NPCs, sci-fi vibe.");
		if (alien) ActivateAlienInvasion();

		AddOption("Animal Chaos", animal);
		if (Menu::printingop == *Menu::currentopATM)
			SetOptionTip("Animal riding on, wild animals around you.");
		if (animal) ActivateAnimalChaos();

		AddOption("Stop Active Mode", stop);
		if (stop) StopAll();
	}
}

REGISTER_SUBMENU(CHAOSMODES, ChaosModes::Menu)
