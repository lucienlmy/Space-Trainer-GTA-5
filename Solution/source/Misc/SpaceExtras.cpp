/*
* Space Trainer exclusive extras
*/
#include "SpaceExtras.h"

#include "..\Menu\Menu.h"
#include "..\Menu\submenu_enum.h"
#include "..\Menu\submenu_switch.h"
#include "..\Menu\Routine.h"
#include "..\Natives\natives2.h"
#include "..\Scripting\Game.h"
#include "..\Scripting\GTAped.h"
#include "..\Scripting\GTAvehicle.h"
#include "..\Scripting\World.h"
#include "..\Util\ExePath.h"

#include <vector>
#include <Windows.h>
#include <fstream>

namespace SpaceExtras
{
	bool slowMotion = false;
	bool softLanding = false;
	bool freezeNearby = false;
	bool attractPickups = false;
	bool vehicleHover = false;
	bool cleanAura = false;
	bool megaJump = false;
	bool infiniteStamina = false;
	bool quietSteps = false;
	bool forceField = false;
	bool autoFlipCar = false;
	bool nightVision = false;
	bool thermalVision = false;
	bool walkOnWater = false;
	bool tinyPlayer = false;
	bool explosiveMeleeQuick = false;
	bool oneHitKill = false;
	bool magnetPickups = false;

	void EnsureSpaceStuffFolders()
	{
		static bool done = false;
		if (done) return;
		done = true;

		const char* folders[] = {
			"\\SpaceStuff",
			"\\SpaceStuff\\Vehicle",
			"\\SpaceStuff\\Outfit",
			"\\SpaceStuff\\Spooner",
			"\\SpaceStuff\\Audio",
			"\\SpaceStuff\\Graphics",
			"\\SpaceStuff\\Graphics\\Speedo",
			"\\SpaceStuff\\WeaponsLoadout",
			"\\SpaceStuff\\Language",
			"\\SpaceStuff\\CustomSkins",
			"\\SpaceStuff\\Themes"
		};

		const std::string root = ExePathA(false);
		for (const char* rel : folders)
		{
			const std::string path = root + rel;
			CreateDirectoryA(path.c_str(), nullptr);
		}

		const std::string cfg = root + "\\SpaceStuff\\spaceConfig.ini";
		if (!does_file_exist(cfg))
		{
			std::ofstream out(cfg, std::ios::out | std::ios::trunc);
			if (out.is_open())
			{
				out << "[settings]\n";
				out << "open_key=116\n";
				out << "loglevel=1\n";
				out << "\n[hax-values]\n";
				out << "space_ui_opacity=0.78\n";
				out << "space_ui_scale=1.0\n";
				out << "space_ui_blur=0.22\n";
				out << "space_ui_accent=5\n";
				out.close();
			}
		}
	}

	static void UnfreezeNearby()
	{
		Ped me = PLAYER_PED_ID();
		std::vector<GTAped> peds;
		std::vector<GTAvehicle> vehs;
		World::GetNearbyPeds(peds, GTAped(me), 45.0f);
		World::GetNearbyVehicles(vehs, GTAped(me), 45.0f);
		for (auto& p : peds)
		{
			if (p.Exists() && p.Handle() != me)
				FREEZE_ENTITY_POSITION(p.Handle(), false);
		}
		for (auto& v : vehs)
		{
			if (v.Exists())
				FREEZE_ENTITY_POSITION(v.Handle(), false);
		}
	}

	void Tick()
	{
		Ped me = PLAYER_PED_ID();
		if (!DOES_ENTITY_EXIST(me))
			return;

		SET_TIME_SCALE(slowMotion ? 0.35f : 1.0f);

		if (softLanding)
		{
			SET_ENTITY_PROOFS(me,
				playerInvincibility ? TRUE : FALSE,
				playerInvincibility ? TRUE : FALSE,
				playerInvincibility ? TRUE : FALSE,
				TRUE,
				playerInvincibility ? TRUE : FALSE,
				playerInvincibility ? TRUE : FALSE,
				playerInvincibility ? TRUE : FALSE,
				playerInvincibility ? TRUE : FALSE);
		}

		if (freezeNearby)
		{
			std::vector<GTAped> peds;
			std::vector<GTAvehicle> vehs;
			World::GetNearbyPeds(peds, GTAped(me), 35.0f);
			World::GetNearbyVehicles(vehs, GTAped(me), 35.0f);
			for (auto& p : peds)
			{
				if (!p.Exists() || p.Handle() == me || IS_PED_A_PLAYER(p.Handle()))
					continue;
				FREEZE_ENTITY_POSITION(p.Handle(), true);
			}
			for (auto& v : vehs)
			{
				if (!v.Exists() || v.Handle() == g_myVeh)
					continue;
				FREEZE_ENTITY_POSITION(v.Handle(), true);
			}
		}

		if (vehicleHover && DOES_ENTITY_EXIST(g_myVeh))
		{
			SET_VEHICLE_GRAVITY(g_myVeh, false);
			APPLY_FORCE_TO_ENTITY_CENTER_OF_MASS(g_myVeh, 1, 0.0f, 0.0f, 0.18f, true, false, true, false);
		}

		if (cleanAura)
		{
			CLEAR_PED_BLOOD_DAMAGE(me);
			RESET_PED_VISIBLE_DAMAGE(me);
			if (DOES_ENTITY_EXIST(g_myVeh))
			{
				SET_VEHICLE_DIRT_LEVEL(g_myVeh, 0.0f);
				WASH_DECALS_FROM_VEHICLE(g_myVeh, 1.0f);
			}
		}

		if (megaJump)
			SET_SUPER_JUMP_THIS_FRAME(PLAYER_ID());

		if (infiniteStamina)
			RESTORE_PLAYER_STAMINA(PLAYER_ID(), 100.0f);

		if (quietSteps)
			SET_PED_STEALTH_MOVEMENT(me, true, "");

		if (forceField)
		{
			std::vector<GTAped> peds;
			World::GetNearbyPeds(peds, GTAped(me), 8.0f);
			Vector3 myPos = GET_ENTITY_COORDS(me, true);
			for (auto& p : peds)
			{
				if (!p.Exists() || p.Handle() == me || IS_PED_A_PLAYER(p.Handle()))
					continue;
				Vector3 pos = GET_ENTITY_COORDS(p.Handle(), true);
				APPLY_FORCE_TO_ENTITY(p.Handle(), 1, (pos.x - myPos.x) * 1.8f, (pos.y - myPos.y) * 1.8f, 1.2f, 0.0f, 0.0f, 0.0f, 0, false, true, true, false, true);
			}
		}

		if (autoFlipCar && DOES_ENTITY_EXIST(g_myVeh) && IS_ENTITY_UPSIDEDOWN(g_myVeh))
		{
			Vector3 rot = GET_ENTITY_ROTATION(g_myVeh, 2);
			SET_ENTITY_ROTATION(g_myVeh, 0.0f, 0.0f, rot.z, 2, true);
			SET_VEHICLE_ON_GROUND_PROPERLY(g_myVeh, 5.0f);
		}

		SET_NIGHTVISION(nightVision);
		SET_SEETHROUGH(thermalVision);

		if (walkOnWater)
		{
			Vector3 pos = GET_ENTITY_COORDS(me, true);
			float waterZ = 0.0f;
			if (GET_WATER_HEIGHT(pos.x, pos.y, pos.z, &waterZ))
			{
				if (pos.z < waterZ + 1.2f && !IS_PED_IN_ANY_VEHICLE(me, false))
					SET_ENTITY_COORDS_NO_OFFSET(me, pos.x, pos.y, waterZ + 1.0f, false, false, false);
			}
		}

		if (tinyPlayer)
			SET_PED_CONFIG_FLAG(me, 223, true);
		else
			SET_PED_CONFIG_FLAG(me, 223, false);

		if (explosiveMeleeQuick)
			SET_EXPLOSIVE_MELEE_THIS_FRAME(PLAYER_ID());

		if (oneHitKill)
			SET_PLAYER_WEAPON_DAMAGE_MODIFIER(PLAYER_ID(), 100.0f);
		else
			SET_PLAYER_WEAPON_DAMAGE_MODIFIER(PLAYER_ID(), 1.0f);

		if (magnetPickups)
		{
			Vector3 pos = GET_ENTITY_COORDS(me, true);
			for (int i = 0; i < 8; ++i)
			{
				CREATE_AMBIENT_PICKUP(
					GET_HASH_KEY("PICKUP_HEALTH_STANDARD"),
					pos.x + GET_RANDOM_FLOAT_IN_RANGE(-1.5f, 1.5f),
					pos.y + GET_RANDOM_FLOAT_IN_RANGE(-1.5f, 1.5f),
					pos.z + 0.2f,
					0, 0, 1, false, true);
			}
			magnetPickups = false;
			Game::Print::PrintBottomLeft("Health pickups dropped.");
		}
	}

	static void BurstMoney()
	{
		Ped me = PLAYER_PED_ID();
		Vector3 pos = GET_ENTITY_COORDS(me, true);
		for (int i = 0; i < 10; ++i)
		{
			CREATE_AMBIENT_PICKUP(
				GET_HASH_KEY("PICKUP_MONEY_CASE"),
				pos.x + GET_RANDOM_FLOAT_IN_RANGE(-2.0f, 2.0f),
				pos.y + GET_RANDOM_FLOAT_IN_RANGE(-2.0f, 2.0f),
				pos.z + 0.4f,
				0, 2000, 1, false, true);
		}
		Game::Print::PrintBottomLeft("Cash drop spawned.");
	}

	static void WarpForward()
	{
		Ped me = PLAYER_PED_ID();
		Vector3 fwd = GET_ENTITY_FORWARD_VECTOR(me);
		Vector3 pos = GET_ENTITY_COORDS(me, true);
		SET_ENTITY_COORDS_NO_OFFSET(me, pos.x + fwd.x * 12.0f, pos.y + fwd.y * 12.0f, pos.z + 0.4f, false, false, false);
	}

	static void PartyGodmode()
	{
		Ped me = PLAYER_PED_ID();
		SetPedInvincibleOn(me);
		playerInvincibility = true;
		if (DOES_ENTITY_EXIST(g_myVeh))
		{
			SetVehicleInvincibleOn(g_myVeh);
			vehicleInvincibility = true;
		}
		Game::Print::PrintBottomLeft("~g~Party Godmode~s~ enabled.");
	}

	static void ClearWanted()
	{
		SET_PLAYER_WANTED_LEVEL(PLAYER_ID(), 0, false);
		SET_PLAYER_WANTED_LEVEL_NOW(PLAYER_ID(), false);
		Game::Print::PrintBottomLeft("Wanted cleared.");
	}

	static void MaxWanted()
	{
		SET_PLAYER_WANTED_LEVEL(PLAYER_ID(), 5, false);
		SET_PLAYER_WANTED_LEVEL_NOW(PLAYER_ID(), false);
	}

	static void RandomOutfit()
	{
		Ped me = PLAYER_PED_ID();
		SET_PED_RANDOM_COMPONENT_VARIATION(me, false);
		Game::Print::PrintBottomLeft("Random outfit applied.");
	}

	static void Skydive()
	{
		Ped me = PLAYER_PED_ID();
		Vector3 pos = GET_ENTITY_COORDS(me, true);
		SET_ENTITY_COORDS_NO_OFFSET(me, pos.x, pos.y, pos.z + 250.0f, false, false, false);
		TASK_SKY_DIVE(me, true);
		Game::Print::PrintBottomLeft("Skydiving!");
	}

	static void Suicide()
	{
		SET_ENTITY_HEALTH(PLAYER_PED_ID(), 0, 0);
	}

	static void CleanMe()
	{
		Ped me = PLAYER_PED_ID();
		CLEAR_PED_BLOOD_DAMAGE(me);
		RESET_PED_VISIBLE_DAMAGE(me);
		CLEAR_PED_WETNESS(me);
		CLEAR_PED_ENV_DIRT(me);
	}

	void Menu()
	{
		AddTitle("Space Extras");

		bool slowOff = false;
		AddToggle("Slow Motion", slowMotion, null, slowOff);
		if (slowOff)
			SET_TIME_SCALE(1.0f);

		bool softOff = false;
		AddToggle("Soft Landing (No Fall Damage)", softLanding, null, softOff);
		if (softOff && !playerInvincibility)
			SET_ENTITY_PROOFS(PLAYER_PED_ID(), FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE);

		bool freezeOff = false;
		AddToggle("Freeze Nearby NPCs/Cars", freezeNearby, null, freezeOff);
		if (freezeOff)
			UnfreezeNearby();

		bool hoverOff = false;
		AddToggle("Vehicle Hover (Anti-Gravity)", vehicleHover, null, hoverOff);
		if (hoverOff && DOES_ENTITY_EXIST(g_myVeh))
			SET_VEHICLE_GRAVITY(g_myVeh, true);

		AddToggle("Auto Clean Aura", cleanAura);
		AddToggle("Mega Jump", megaJump);
		AddToggle("Infinite Stamina", infiniteStamina);
		AddToggle("Quiet Steps", quietSteps);
		AddToggle("Force Field (Push NPCs)", forceField);
		AddToggle("Auto Flip Car", autoFlipCar);
		AddToggle("Night Vision", nightVision);
		AddToggle("Thermal Vision", thermalVision);
		AddToggle("Walk On Water", walkOnWater);
		AddToggle("Tiny Player", tinyPlayer);
		AddToggle("Explosive Melee", explosiveMeleeQuick);
		AddToggle("One-Hit Kill", oneHitKill);

		bool money = false, warp = false, party = false, repairAll = false;
		bool clearWanted = false, maxWanted = false, rndOutfit = false;
		bool skydive = false, suicide = false, clean = false, healthPacks = false;
		AddOption("Cash Rain Around Me", money);
		AddOption("Health Pack Drop", healthPacks);
		AddOption("Warp Forward 12m", warp);
		AddOption("Skydive From Height", skydive);
		AddOption("Party Godmode (You + Car)", party);
		AddOption("Repair All Nearby Vehicles", repairAll);
		AddOption("Clear Wanted", clearWanted);
		AddOption("Max Wanted (5 Stars)", maxWanted);
		AddOption("Random Outfit", rndOutfit);
		AddOption("Clean Ped", clean);
		AddOption("Suicide", suicide);

		if (money) BurstMoney();
		if (healthPacks) magnetPickups = true;
		if (warp) WarpForward();
		if (skydive) Skydive();
		if (party) PartyGodmode();
		if (clearWanted) ClearWanted();
		if (maxWanted) MaxWanted();
		if (rndOutfit) RandomOutfit();
		if (clean) CleanMe();
		if (suicide) Suicide();
		if (repairAll)
		{
			std::vector<GTAvehicle> vehs;
			World::GetNearbyVehicles(vehs, GTAped(PLAYER_PED_ID()), 50.0f);
			for (auto& v : vehs)
			{
				if (!v.Exists()) continue;
				v.RequestControlOnce();
				SET_VEHICLE_FIXED(v.Handle());
				SET_VEHICLE_DEFORMATION_FIXED(v.Handle());
				SET_VEHICLE_DIRT_LEVEL(v.Handle(), 0.0f);
			}
			Game::Print::PrintBottomLeft("Nearby vehicles repaired.");
		}
	}
}

REGISTER_SUBMENU(SPACEEXTRAS, SpaceExtras::Menu)
