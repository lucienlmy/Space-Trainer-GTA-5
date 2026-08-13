/*
* Space Trainer - Tornado / Flood / Meteor hub
*/
#include "Disasters.h"
#include "MeteorShower.h"

#include "..\Menu\Menu.h"
#include "..\Menu\submenu_enum.h"
#include "..\Menu\submenu_switch.h"
#include "..\Natives\natives2.h"
#include "..\Scripting\Game.h"
#include "..\Scripting\GTAentity.h"
#include "..\Scripting\GTAped.h"
#include "..\Scripting\GTAvehicle.h"
#include "..\Scripting\World.h"
#include "..\Scripting\PTFX.h"

#include <cmath>
#include <vector>
#include <Windows.h>

namespace Disasters
{
	static bool s_tornado = false;
	static bool s_flood = false;
	static float s_floodLevel = 12.0f;
	static float s_tornadoAngle = 0.0f;
	static Vector3 s_tornadoCenter{};
	static DWORD s_tornadoFxTick = 0;

	void StopAll()
	{
		s_tornado = false;
		s_flood = false;
		if (MeteorShower::g_meteorShower.Enabled())
			MeteorShower::g_meteorShower.TurnOff();
	}

	static void TickTornado()
	{
		if (!s_tornado)
			return;

		Ped me = PLAYER_PED_ID();
		if (!DOES_ENTITY_EXIST(me))
			return;

		if (s_tornadoCenter.x == 0.0f && s_tornadoCenter.y == 0.0f)
			s_tornadoCenter = GET_ENTITY_COORDS(me, true);

		s_tornadoAngle += 6.5f;
		if (s_tornadoAngle > 360.0f)
			s_tornadoAngle -= 360.0f;

		const DWORD now = GetTickCount();
		if (now - s_tornadoFxTick > 180)
		{
			s_tornadoFxTick = now;
			for (int i = 0; i < 4; ++i)
			{
				const float a = (s_tornadoAngle + i * 90.0f) * 0.01745329251f;
				const float r = 2.5f + i * 1.8f;
				Vector3 p(
					s_tornadoCenter.x + std::cos(a) * r,
					s_tornadoCenter.y + std::sin(a) * r,
					s_tornadoCenter.z + 1.0f + i * 2.2f);
				PTFX::TriggerPTFX("core", "ent_dst_dust", GTAentity(0), p, Vector3(), 1.4f);
			}
		}

		std::vector<GTAped> peds;
		std::vector<GTAvehicle> vehs;
		World::GetNearbyPeds(peds, GTAped(me), 55.0f, 24);
		World::GetNearbyVehicles(vehs, GTAped(me), 55.0f, 18);

		for (auto& p : peds)
		{
			if (!p.Exists() || p.Handle() == me)
				continue;
			Vector3 pos = p.GetPosition();
			const float dx = pos.x - s_tornadoCenter.x;
			const float dy = pos.y - s_tornadoCenter.y;
			const float dist = std::sqrt(dx * dx + dy * dy) + 0.01f;
			if (dist > 50.0f)
				continue;
			const float fx = (-dy / dist) * 2.2f;
			const float fy = (dx / dist) * 2.2f;
			const float fz = 4.0f + (50.0f - dist) * 0.08f;
			APPLY_FORCE_TO_ENTITY(p.Handle(), 1, fx, fy, fz, 0, 0, 0, 0, false, true, true, false, true);
		}
		for (auto& v : vehs)
		{
			if (!v.Exists())
				continue;
			Vector3 pos = v.GetPosition();
			const float dx = pos.x - s_tornadoCenter.x;
			const float dy = pos.y - s_tornadoCenter.y;
			const float dist = std::sqrt(dx * dx + dy * dy) + 0.01f;
			if (dist > 55.0f)
				continue;
			const float fx = (-dy / dist) * 3.0f;
			const float fy = (dx / dist) * 3.0f;
			APPLY_FORCE_TO_ENTITY(v.Handle(), 1, fx, fy, 5.0f, 0, 0, 0.2f, 0, false, true, true, false, true);
		}
	}

	static void TickFlood()
	{
		if (!s_flood)
			return;
		Ped me = PLAYER_PED_ID();
		Vector3 pos = GET_ENTITY_COORDS(me, true);
		for (int i = -5; i <= 5; ++i)
		{
			for (int j = -5; j <= 5; ++j)
			{
				MODIFY_WATER(pos.x + i * 6.0f, pos.y + j * 6.0f, pos.z, s_floodLevel);
			}
		}
	}

	void Tick()
	{
		TickTornado();
		TickFlood();
	}

	void Menu()
	{
		AddTitle("Disasters");

		bool stop = false;
		AddOption("Stop All Disasters", stop);
		if (stop)
		{
			StopAll();
			Game::Print::PrintBottomLeft("~g~Disasters stopped.");
		}

		AddToggle("Tornado", s_tornado);
		if (Menu::printingop == *Menu::currentopATM)
			SetOptionTip("Spins debris and NPCs around your position.");
		if (s_tornado && (s_tornadoCenter.x == 0.0f && s_tornadoCenter.y == 0.0f))
			s_tornadoCenter = GET_ENTITY_COORDS(PLAYER_PED_ID(), true);
		if (s_tornado)
		{
			bool recenter = false;
			AddOption("Recenter Tornado On Me", recenter);
			if (recenter)
				s_tornadoCenter = GET_ENTITY_COORDS(PLAYER_PED_ID(), true);
		}

		AddToggle("Flood (Raised Water)", s_flood);
		if (Menu::printingop == *Menu::currentopATM)
			SetOptionTip("Raises water level around you.");
		bool fPlus = false, fMinus = false;
		AddNumber("Flood Height", s_floodLevel, 1, null, fPlus, fMinus);
		if (fPlus && s_floodLevel < 80.0f) s_floodLevel += 1.0f;
		if (fMinus && s_floodLevel > 2.0f) s_floodLevel -= 1.0f;

		AddLocal("Meteor Shower", MeteorShower::g_meteorShower.Enabled(), MeteorShower::ToggleOnOff, MeteorShower::ToggleOnOff);
		if (Menu::printingop == *Menu::currentopATM)
			SetOptionTip("Rocks fall from the sky with thunder weather.");
	}
}

REGISTER_SUBMENU(DISASTERS, Disasters::Menu)
