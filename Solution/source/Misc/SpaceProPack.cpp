/*
* Space Trainer - Pro tools pack
*/
#include "SpaceProPack.h"
#include "ChaosModes.h"

#include "..\Menu\Menu.h"
#include "..\Menu\Routine.h"
#include "..\Menu\submenu_enum.h"
#include "..\Menu\submenu_switch.h"
#include "..\Natives\natives2.h"
#include "..\Scripting\Camera.h"
#include "..\Scripting\Game.h"
#include "..\Scripting\GTAped.h"
#include "..\Scripting\GTAvehicle.h"
#include "..\Scripting\Model.h"
#include "..\Scripting\World.h"
#include "..\Scripting\enums.h"
#include "..\Submenus\VehicleModShop.h"
#include "..\Misc\Gta2Cam.h"
#include "..\Util\ExePath.h"
#include "..\Util\StringManip.h"

#include <pugixml\src\pugixml.hpp>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <string>
#include <vector>
#include <Windows.h>

namespace SpaceProPack
{
	static std::vector<Vehicle> s_spawnedVehs;
	static bool s_safeMode = true;
	static bool s_autoPruneSpawned = true;

	// Traffic
	static bool s_realisticTraffic = false;
	static float s_trafficDensity = 0.85f;
	static float s_pedDensity = 0.90f;
	static float s_driverAbility = 0.65f;
	static float s_driverAggro = 0.25f;
	static DWORD s_trafficApplyTick = 0;

	// Film cam
	enum class FilmMode : int { Off = 0, Orbit, Chase, Dolly, Fixed, LowAngle };
	static FilmMode s_filmMode = FilmMode::Off;
	static Camera s_filmCam;
	static float s_filmFov = 50.0f;
	static float s_filmDist = 6.5f;
	static float s_filmHeight = 1.4f;
	static float s_filmOrbit = 0.0f;
	static float s_filmShake = 0.0f;
	static bool s_filmLockPlayer = true;

	static std::string s_gangNameInput = "MyGang";

	static std::string GangsDir()
	{
		const std::string dir = GetPathffA(Pathff::Main, true) + "Gangs";
		CreateDirectoryA(GetPathffA(Pathff::Main, false).c_str(), nullptr);
		CreateDirectoryA(dir.c_str(), nullptr);
		return dir + "\\";
	}

	void TrackSpawnedVehicle(Vehicle veh)
	{
		if (!veh || !DOES_ENTITY_EXIST(veh))
			return;
		for (Vehicle v : s_spawnedVehs)
		{
			if (v == veh)
				return;
		}
		s_spawnedVehs.push_back(veh);
		if (s_spawnedVehs.size() > 64)
			s_spawnedVehs.erase(s_spawnedVehs.begin(), s_spawnedVehs.begin() + 16);
	}

	bool IsSafeMode()
	{
		return s_safeMode;
	}

	static void PruneSpawnedList()
	{
		s_spawnedVehs.erase(
			std::remove_if(s_spawnedVehs.begin(), s_spawnedVehs.end(),
				[](Vehicle v) { return !v || !DOES_ENTITY_EXIST(v); }),
			s_spawnedVehs.end());
	}

	static bool VehicleIsEmpty(Vehicle v)
	{
		if (!DOES_ENTITY_EXIST(v))
			return true;
		const int maxi = GET_VEHICLE_MAX_NUMBER_OF_PASSENGERS(v);
		for (int i = -1; i <= maxi; ++i)
		{
			Ped p = GET_PED_IN_VEHICLE_SEAT(v, i, 0);
			if (DOES_ENTITY_EXIST(p))
				return false;
		}
		return true;
	}

	static void SmartDeleteVehicle(Vehicle v, bool keepCurrent)
	{
		if (!DOES_ENTITY_EXIST(v))
			return;
		Ped me = PLAYER_PED_ID();
		if (keepCurrent && IS_PED_IN_VEHICLE(me, v, true))
			return;

		const int maxi = GET_VEHICLE_MAX_NUMBER_OF_PASSENGERS(v);
		for (int i = -1; i <= maxi; ++i)
		{
			Ped p = GET_PED_IN_VEHICLE_SEAT(v, i, 0);
			if (DOES_ENTITY_EXIST(p) && p != me)
			{
				TASK_LEAVE_ANY_VEHICLE(p, 0, 16);
				CLEAR_PED_TASKS_IMMEDIATELY(p);
			}
		}
		GTAvehicle(v).RequestControlOnce();
		SET_ENTITY_AS_MISSION_ENTITY(v, true, true);
		SET_VEHICLE_HAS_BEEN_OWNED_BY_PLAYER(v, false);
		DELETE_VEHICLE(&v);
	}

	static int DeleteSpawned(bool onlyEmpty, bool keepCurrent, float maxDist)
	{
		PruneSpawnedList();
		Ped me = PLAYER_PED_ID();
		Vector3 myPos = GET_ENTITY_COORDS(me, true);
		int deleted = 0;
		std::vector<Vehicle> remain;
		remain.reserve(s_spawnedVehs.size());
		for (Vehicle v : s_spawnedVehs)
		{
			if (!DOES_ENTITY_EXIST(v))
				continue;
			if (keepCurrent && IS_PED_IN_VEHICLE(me, v, true))
			{
				remain.push_back(v);
				continue;
			}
			if (onlyEmpty && !VehicleIsEmpty(v))
			{
				remain.push_back(v);
				continue;
			}
			if (maxDist > 0.0f)
			{
				Vector3 p = GET_ENTITY_COORDS(v, true);
				const float dx = p.x - myPos.x, dy = p.y - myPos.y, dz = p.z - myPos.z;
				if ((dx * dx + dy * dy + dz * dz) > (maxDist * maxDist))
				{
					remain.push_back(v);
					continue;
				}
			}
			SmartDeleteVehicle(v, keepCurrent);
			++deleted;
		}
		s_spawnedVehs.swap(remain);
		return deleted;
	}

	static void StopFilmCam()
	{
		s_filmMode = FilmMode::Off;
		if (s_filmCam.Exists())
		{
			s_filmCam.SetActive(false);
			s_filmCam.Destroy();
		}
		Camera::RenderScriptCams(false);
		s_filmCam = Camera();
	}

	static void KillConflictingCams()
	{
		if (noClipToggle)
		{
			SetNoclipOff1();
			SetNoclipOff2();
			noClipToggle = false;
			noClip = false;
		}
		if (GTA2Cam::g_gta2Cam.Enabled())
			GTA2Cam::g_gta2Cam.TurnOff();
	}

	static void EnsureFilmCam()
	{
		if (!s_filmCam.Exists())
		{
			s_filmCam = World::CreateCamera();
			if (!s_filmCam.Exists())
				return;
		}
		s_filmCam.SetActive(true);
		Camera::RenderScriptCams(true);
		s_filmCam.SetFieldOfView(s_filmFov);
		if (s_filmShake > 0.01f)
		{
			s_filmCam.SetShakeType(CameraShake::Hand);
			s_filmCam.SetShakeAmplitude(s_filmShake);
			s_filmCam.SetShake(true);
		}
		else
			s_filmCam.SetShake(false);
	}

	static void TickFilmCam()
	{
		if (s_filmMode == FilmMode::Off)
			return;

		// FreeCam / Spooner freecam wins if the player enabled it after film.
		if (noClipToggle)
		{
			StopFilmCam();
			return;
		}

		Ped me = PLAYER_PED_ID();
		if (!DOES_ENTITY_EXIST(me))
			return;

		EnsureFilmCam();
		if (!s_filmCam.Exists())
			return;

		GTAentity target = me;
		if (IS_PED_IN_ANY_VEHICLE(me, false))
			target = GET_VEHICLE_PED_IS_IN(me, false);

		Vector3 tpos = GET_ENTITY_COORDS(target.Handle(), true);
		s_filmOrbit += 0.55f;
		if (s_filmOrbit > 360.0f)
			s_filmOrbit -= 360.0f;

		Vector3 camPos = tpos;
		switch (s_filmMode)
		{
		case FilmMode::Orbit:
		{
			const float rad = s_filmOrbit * 0.01745329251f;
			camPos.x = tpos.x + std::cos(rad) * s_filmDist;
			camPos.y = tpos.y + std::sin(rad) * s_filmDist;
			camPos.z = tpos.z + s_filmHeight;
			break;
		}
		case FilmMode::Chase:
			camPos = GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(target.Handle(), 0.0f, -s_filmDist, s_filmHeight);
			break;
		case FilmMode::Dolly:
			camPos = GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(target.Handle(), s_filmDist, 2.0f, s_filmHeight);
			break;
		case FilmMode::Fixed:
			camPos = s_filmCam.GetPosition();
			break;
		case FilmMode::LowAngle:
			camPos = GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(target.Handle(), 0.0f, s_filmDist, 0.4f);
			break;
		default:
			break;
		}

		if (s_filmMode != FilmMode::Fixed)
			s_filmCam.SetPosition(camPos);
		s_filmCam.PointAt(target);
		s_filmCam.SetFieldOfView(s_filmFov);
		s_filmCam.SetActive(true);
		Camera::RenderScriptCams(true);

		if (s_filmLockPlayer)
		{
			DISABLE_CONTROL_ACTION(0, 1, true);
			DISABLE_CONTROL_ACTION(0, 2, true);
		}
	}

	static void ApplyRealisticTrafficAround()
	{
		if (!s_realisticTraffic)
			return;

		SET_VEHICLE_DENSITY_MULTIPLIER_THIS_FRAME(s_trafficDensity);
		SET_RANDOM_VEHICLE_DENSITY_MULTIPLIER_THIS_FRAME(s_trafficDensity);
		SET_PARKED_VEHICLE_DENSITY_MULTIPLIER_THIS_FRAME(s_trafficDensity * 0.9f);
		SET_PED_DENSITY_MULTIPLIER_THIS_FRAME(s_pedDensity);
		SET_SCENARIO_PED_DENSITY_MULTIPLIER_THIS_FRAME(s_pedDensity, s_pedDensity);

		const DWORD now = GetTickCount();
		if (now - s_trafficApplyTick < 1200)
			return;
		s_trafficApplyTick = now;

		Ped me = PLAYER_PED_ID();
		std::vector<GTAvehicle> vehs;
		World::GetNearbyVehicles(vehs, GTAped(me), 55.0f, 28);
		for (auto& v : vehs)
		{
			if (!v.Exists())
				continue;
			Ped driver = GET_PED_IN_VEHICLE_SEAT(v.Handle(), -1, 0);
			if (!DOES_ENTITY_EXIST(driver) || driver == me)
				continue;
			SET_DRIVER_ABILITY(driver, s_driverAbility);
			SET_DRIVER_AGGRESSIVENESS(driver, s_driverAggro);
			SET_PED_KEEP_TASK(driver, true);
			SET_DRIVE_TASK_CRUISE_SPEED(driver, 18.0f + s_driverAbility * 12.0f);
			SET_DRIVE_TASK_DRIVING_STYLE(driver, 786603); // careful / obey lights
		}
	}

	void Tick()
	{
		if (s_autoPruneSpawned)
		{
			static DWORD lastPrune = 0;
			if (GetTickCount() - lastPrune > 4000)
			{
				lastPrune = GetTickCount();
				PruneSpawnedList();
			}
		}

		TickFilmCam();
		ApplyRealisticTrafficAround();
	}

	void VehicleCleanMenu()
	{
		AddTitle("Smart Vehicle Cleanup");
		PruneSpawnedList();

		char buf[64];
		sprintf_s(buf, "Tracked spawned: %u", (unsigned)s_spawnedVehs.size());
		AddBreak(buf);

		AddToggle("Auto-prune invalid handles", s_autoPruneSpawned);

		bool delEmpty = false, delAll = false, delNear = false, delFar = false;
		AddOption("Delete empty spawned", delEmpty);
		AddOption("Delete all spawned (keep current)", delAll);
		AddOption("Delete spawned within 40m", delNear);
		AddOption("Delete spawned farther than 80m", delFar);

		if (delEmpty)
		{
			int n = DeleteSpawned(true, true, 0.0f);
			Game::Print::PrintBottomLeft("~g~Removed empty spawned: " + std::to_string(n));
		}
		if (delAll)
		{
			int n = DeleteSpawned(false, true, 0.0f);
			Game::Print::PrintBottomLeft("~g~Removed spawned: " + std::to_string(n));
		}
		if (delNear)
		{
			int n = DeleteSpawned(false, true, 40.0f);
			Game::Print::PrintBottomLeft("~g~Removed nearby spawned: " + std::to_string(n));
		}
		if (delFar)
		{
			// Invert distance filter: delete those farther than 80 by custom pass
			PruneSpawnedList();
			Ped me = PLAYER_PED_ID();
			Vector3 myPos = GET_ENTITY_COORDS(me, true);
			int deleted = 0;
			std::vector<Vehicle> remain;
			for (Vehicle v : s_spawnedVehs)
			{
				if (!DOES_ENTITY_EXIST(v))
					continue;
				if (IS_PED_IN_VEHICLE(me, v, true))
				{
					remain.push_back(v);
					continue;
				}
				Vector3 p = GET_ENTITY_COORDS(v, true);
				const float dx = p.x - myPos.x, dy = p.y - myPos.y, dz = p.z - myPos.z;
				if ((dx * dx + dy * dy + dz * dz) <= (80.0f * 80.0f))
				{
					remain.push_back(v);
					continue;
				}
				SmartDeleteVehicle(v, true);
				++deleted;
			}
			s_spawnedVehs.swap(remain);
			Game::Print::PrintBottomLeft("~g~Removed far spawned: " + std::to_string(deleted));
		}

		bool clearWorldEmpty = false;
		AddOption("Clear empty NPC cars (30m)", clearWorldEmpty);
		if (clearWorldEmpty)
		{
			Ped me = PLAYER_PED_ID();
			std::vector<GTAvehicle> vehs;
			World::GetNearbyVehicles(vehs, GTAped(me), 30.0f);
			int n = 0;
			Vehicle myVeh = IS_PED_IN_ANY_VEHICLE(me, false) ? GET_VEHICLE_PED_IS_IN(me, false) : 0;
			for (auto& v : vehs)
			{
				if (!v.Exists() || v.Handle() == myVeh)
					continue;
				if (!VehicleIsEmpty(v.Handle()))
					continue;
				SmartDeleteVehicle(v.Handle(), true);
				++n;
			}
			Game::Print::PrintBottomLeft("~g~Cleared empty NPC cars: " + std::to_string(n));
		}
	}

	void TrafficMenu()
	{
		AddTitle("Realistic Traffic");
		AddToggle("Enable Realistic Traffic", s_realisticTraffic);

		bool dPlus = false, dMinus = false;
		AddNumber("Vehicle Density", s_trafficDensity, 2, null, dPlus, dMinus);
		if (dPlus && s_trafficDensity < 1.5f) s_trafficDensity += 0.05f;
		if (dMinus && s_trafficDensity > 0.1f) s_trafficDensity -= 0.05f;

		bool pPlus = false, pMinus = false;
		AddNumber("Ped Density", s_pedDensity, 2, null, pPlus, pMinus);
		if (pPlus && s_pedDensity < 1.5f) s_pedDensity += 0.05f;
		if (pMinus && s_pedDensity > 0.1f) s_pedDensity -= 0.05f;

		bool aPlus = false, aMinus = false;
		AddNumber("Driver Skill", s_driverAbility, 2, null, aPlus, aMinus);
		if (aPlus && s_driverAbility < 1.0f) s_driverAbility += 0.05f;
		if (aMinus && s_driverAbility > 0.05f) s_driverAbility -= 0.05f;

		bool gPlus = false, gMinus = false;
		AddNumber("Driver Aggression", s_driverAggro, 2, null, gPlus, gMinus);
		if (gPlus && s_driverAggro < 1.0f) s_driverAggro += 0.05f;
		if (gMinus && s_driverAggro > 0.0f) s_driverAggro -= 0.05f;

		AddOption("Tip: keep Safe Mode ON with other trainers", null);
	}

	void FilmCamMenu()
	{
		AddTitle("Film Cameras");

		bool stop = false;
		AddOption("Stop Film Camera", stop);
		if (stop) StopFilmCam();

		AddToggle("Lock Look Controls", s_filmLockPlayer);

		bool fovP = false, fovM = false;
		AddNumber("FOV", s_filmFov, 0, null, fovP, fovM);
		if (fovP && s_filmFov < 90.0f) s_filmFov += 2.0f;
		if (fovM && s_filmFov > 20.0f) s_filmFov -= 2.0f;

		bool distP = false, distM = false;
		AddNumber("Distance", s_filmDist, 1, null, distP, distM);
		if (distP && s_filmDist < 25.0f) s_filmDist += 0.5f;
		if (distM && s_filmDist > 2.0f) s_filmDist -= 0.5f;

		bool hP = false, hM = false;
		AddNumber("Height", s_filmHeight, 1, null, hP, hM);
		if (hP && s_filmHeight < 8.0f) s_filmHeight += 0.2f;
		if (hM && s_filmHeight > -1.0f) s_filmHeight -= 0.2f;

		bool shP = false, shM = false;
		AddNumber("Shake", s_filmShake, 2, null, shP, shM);
		if (shP && s_filmShake < 1.0f) s_filmShake += 0.05f;
		if (shM && s_filmShake > 0.0f) s_filmShake -= 0.05f;

		AddBreak("---Modes---");
		bool mOrbit = false, mChase = false, mDolly = false, mFixed = false, mLow = false;
		AddOption("Orbit Cam", mOrbit);
		AddOption("Chase Cam", mChase);
		AddOption("Dolly Side Cam", mDolly);
		AddOption("Fixed Point Cam", mFixed);
		AddOption("Low Angle Cam", mLow);

		auto start = [](FilmMode m) {
			KillConflictingCams();
			s_filmMode = m;
			EnsureFilmCam();
			if (m == FilmMode::Fixed)
			{
				Ped me = PLAYER_PED_ID();
				Vector3 pos = GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(me, 0.0f, -s_filmDist, s_filmHeight);
				s_filmCam.SetPosition(pos);
			}
			Game::Print::PrintBottomCentre("~b~Film camera ON");
		};
		if (mOrbit) start(FilmMode::Orbit);
		if (mChase) start(FilmMode::Chase);
		if (mDolly) start(FilmMode::Dolly);
		if (mFixed) start(FilmMode::Fixed);
		if (mLow) start(FilmMode::LowAngle);

		AddOption("Also: FreeCam in Functions", null, nullFunc, SUB::FUNCTIONS_HUB);
	}

	static bool SaveNearbyAsGang(const std::string& name)
	{
		if (name.empty())
			return false;
		Ped me = PLAYER_PED_ID();
		std::vector<GTAped> peds;
		World::GetNearbyPeds(peds, GTAped(me), 25.0f, 12);

		pugi::xml_document doc;
		auto decl = doc.append_child(pugi::node_declaration);
		decl.append_attribute("version") = "1.0";
		decl.append_attribute("encoding") = "UTF-8";
		auto root = doc.append_child("Gang");
		root.append_attribute("name") = name.c_str();

		int count = 0;
		for (auto& p : peds)
		{
			if (!p.Exists() || p.Handle() == me)
				continue;
			if (IS_PED_A_PLAYER(p.Handle()))
				continue;
			auto node = root.append_child("Member");
			Hash model = GET_ENTITY_MODEL(p.Handle());
			char modelHex[32];
			sprintf_s(modelHex, "0x%08X", (unsigned)model);
			node.append_attribute("model") = modelHex;
			Hash weap = 0;
			GET_CURRENT_PED_WEAPON(p.Handle(), &weap, true);
			node.append_attribute("weapon") = (unsigned)weap;
			++count;
			if (count >= 8)
				break;
		}
		if (count == 0)
			return false;

		std::string path = GangsDir() + name + ".xml";
		return doc.save_file(path.c_str());
	}

	static int SpawnGangFromFile(const std::string& path)
	{
		pugi::xml_document doc;
		if (doc.load_file(path.c_str()).status != pugi::status_ok)
			return 0;
		auto root = doc.document_element();
		Ped me = PLAYER_PED_ID();
		int group = GET_PLAYER_GROUP(PLAYER_ID());
		SET_PED_AS_GROUP_LEADER(me, group);
		int spawned = 0;
		float slot = 0.0f;
		for (auto node = root.child("Member"); node; node = node.next_sibling("Member"))
		{
			std::string modelStr = node.attribute("model").as_string();
			Hash modelHash = 0;
			if (modelStr.rfind("0x", 0) == 0 || modelStr.rfind("0X", 0) == 0)
				modelHash = (Hash)strtoul(modelStr.c_str(), nullptr, 16);
			else
				modelHash = GET_HASH_KEY(modelStr.c_str());

			GTAmodel::Model mdl(modelHash);
			if (!mdl.IsInCdImage() || !mdl.Load(5000))
				continue;

			Vector3 pos = GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(me, 1.2f + slot, 1.0f, 0.0f);
			slot += 1.1f;
			Ped ped = CREATE_PED(26, mdl.hash, pos.x, pos.y, pos.z, GET_ENTITY_HEADING(me), FALSE, FALSE);
			mdl.Unload();
			if (!DOES_ENTITY_EXIST(ped))
				continue;

			SET_ENTITY_AS_MISSION_ENTITY(ped, true, true);
			SET_PED_AS_GROUP_MEMBER(ped, group);
			SET_PED_NEVER_LEAVES_GROUP(ped, true);
			SET_PED_COMBAT_ATTRIBUTES(ped, 46, true);
			SET_PED_KEEP_TASK(ped, true);
			Hash weap = (Hash)node.attribute("weapon").as_uint(0);
			if (weap)
				GIVE_WEAPON_TO_PED(ped, weap, 250, false, true);
			else
				GIVE_WEAPON_TO_PED(ped, WEAPON_MICROSMG, 250, false, true);
			TASK_FOLLOW_TO_OFFSET_OF_ENTITY(ped, me, 0.8f, -1.0f - spawned * 0.4f, 0.0f, 2.0f, -1, 1.5f, true);
			++spawned;
		}
		return spawned;
	}

	void GangsMenu()
	{
		AddTitle("Custom Gangs");

		bool rename = false;
		AddOption("Gang Name: " + s_gangNameInput, rename);
		if (rename)
		{
			std::string in = Game::InputBox(s_gangNameInput, 28U, "Enter gang name:");
			if (!in.empty())
				s_gangNameInput = in;
		}

		bool save = false;
		AddOption("Save Nearby Peds As Gang", save);
		if (save)
		{
			if (SaveNearbyAsGang(s_gangNameInput))
				Game::Print::PrintBottomLeft("~g~Gang saved to SpaceStuff\\Gangs\\");
			else
				Game::Print::PrintBottomLeft("~r~No nearby peds to save.");
		}

		AddBreak("---Saved---");
		std::vector<std::string> files;
		get_all_filenames_with_extension(GangsDir(), ".xml", files, true);
		if (files.empty())
			AddOption("No saved gangs", null);
		for (const auto& f : files)
		{
			bool spawn = false;
			AddOption(f, spawn);
			if (spawn)
			{
				int n = SpawnGangFromFile(GangsDir() + f);
				Game::Print::PrintBottomLeft("~g~Spawned gang members: " + std::to_string(n));
			}
		}
	}

	void Menu()
	{
		AddTitle("Tools Pack");

		bool safeOn = false, safeOff = false;
		AddToggle("Safe Mode (mod compatibility)", s_safeMode, safeOn, safeOff);
		if (safeOn)
		{
			ChaosModes::StopAll();
			Game::Print::PrintBottomCentre("~g~Safe Mode ON — chaos paused, less conflicts.");
		}

		AddOption("Smart Vehicle Cleanup", null, nullFunc, SUB::SPACE_VEHICLECLEAN);
		AddOption("Film Cameras", null, nullFunc, SUB::SPACE_FILMCAM);
		AddOption("Car Tuning (Mod Shop)", null, nullFunc, SUB::MODSHOP);
		AddOption("Custom Gangs", null, nullFunc, SUB::SPACE_GANGS);
		AddOption("Realistic Traffic", null, nullFunc, SUB::SPACE_TRAFFIC);

		AddBreak("---Quick Tuning---");
		bool maxTune = false, repair = false;
		AddOption("Max Upgrade Current Car", maxTune);
		AddOption("Repair + Clean Current Car", repair);
		if (maxTune || repair)
		{
			Ped me = PLAYER_PED_ID();
			if (IS_PED_IN_ANY_VEHICLE(me, false))
			{
				Vehicle v = GET_VEHICLE_PED_IS_IN(me, false);
				if (repair)
				{
					SET_VEHICLE_FIXED(v);
					SET_VEHICLE_DIRT_LEVEL(v, 0.0f);
					SET_VEHICLE_ENGINE_HEALTH(v, 1000.0f);
					SET_VEHICLE_PETROL_TANK_HEALTH(v, 1000.0f);
				}
				if (maxTune)
					sub::SetVehicleMaxUpgrades(v, true, false);
				Game::Print::PrintBottomLeft("~g~Vehicle updated.");
			}
			else
				Game::Print::PrintBottomLeft("~r~Sit in a vehicle first.");
		}

		AddBreak("---Saved Vehicles---");
		AddOption("Vehicle Saver Folder", null, nullFunc, SUB::VEHICLE_SAVER);
	}
}

REGISTER_SUBMENU(SPACE_PROPACK, SpaceProPack::Menu)
REGISTER_SUBMENU(SPACE_FILMCAM, SpaceProPack::FilmCamMenu)
REGISTER_SUBMENU(SPACE_GANGS, SpaceProPack::GangsMenu)
REGISTER_SUBMENU(SPACE_TRAFFIC, SpaceProPack::TrafficMenu)
REGISTER_SUBMENU(SPACE_VEHICLECLEAN, SpaceProPack::VehicleCleanMenu)
