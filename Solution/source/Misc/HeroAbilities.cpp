/*
* Space Trainer - hero power presets (Superman, Batman, etc.)
*/
#include "HeroAbilities.h"

#include "SmashAbility.h"
#include "SpaceExtras.h"

#include "..\Menu\Menu.h"
#include "..\Menu\Routine.h"
#include "..\Menu\submenu_enum.h"
#include "..\Menu\submenu_switch.h"
#include "..\Natives\natives2.h"
#include "..\Scripting\Game.h"
#include <cctype>

namespace HeroAbilities
{
	Id activeId = Id::None;

	static bool s_setNightVision = false;
	static bool s_setSeeThrough = false;
	static float s_prevSwimMult = 1.0f;

	static std::string Lower(std::string s)
	{
		for (char& c : s)
			c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
		return s;
	}

	const char* DisplayName(Id id)
	{
		switch (id)
		{
		case Id::Superman: return "Superman";
		case Id::Batman: return "Batman";
		case Id::Flash: return "Flash";
		case Id::Hulk: return "Hulk";
		case Id::IronMan: return "Iron Man";
		case Id::SpiderMan: return "Spider-Man";
		case Id::Wolverine: return "Wolverine";
		case Id::Aquaman: return "Aquaman";
		default: return "None";
		}
	}

	Id ParseAbilityName(const std::string& name)
	{
		const std::string n = Lower(name);
		if (n.empty() || n == "none" || n == "off") return Id::None;
		if (n == "superman" || n == "super") return Id::Superman;
		if (n == "batman" || n == "bat") return Id::Batman;
		if (n == "flash" || n == "speedster") return Id::Flash;
		if (n == "hulk") return Id::Hulk;
		if (n == "ironman" || n == "iron_man" || n == "iron-man") return Id::IronMan;
		if (n == "spiderman" || n == "spider_man" || n == "spider-man") return Id::SpiderMan;
		if (n == "wolverine" || n == "logan") return Id::Wolverine;
		if (n == "aquaman" || n == "aqua") return Id::Aquaman;
		return Id::None;
	}

	void Clear(bool silent)
	{
		if (activeId == Id::None && !s_setNightVision && !s_setSeeThrough)
			return;

		// Turn off powers that hero presets typically enable.
		superman = false;
		supermanAuto = false;
		superJump = false;
		superRun = false;
		neverWanted = false;
		playerWalkUnderwater = false;
		ignoredByEveryone = false;
		forceField = 0;
		SpaceExtras::softLanding = false;
		if (SmashAbility::g_smashAbility.Enabled())
			SmashAbility::g_smashAbility.TurnOff();

		if (s_setNightVision)
		{
			SET_NIGHTVISION(FALSE);
			s_setNightVision = false;
		}
		if (s_setSeeThrough)
		{
			SET_SEETHROUGH(FALSE);
			s_setSeeThrough = false;
		}

		SET_RUN_SPRINT_MULTIPLIER_FOR_PLAYER(PLAYER_ID(), 1.0f);
		swimSpeedMult = s_prevSwimMult;
		SET_TIME_SCALE(1.0f);

		activeId = Id::None;
		if (!silent)
			Game::Print::PrintBottomLeft("Hero powers cleared.");
	}

	void Apply(Id id)
	{
		Clear(true);
		if (id == Id::None)
			return;

		activeId = id;
		s_prevSwimMult = swimSpeedMult;
		Ped me = PLAYER_PED_ID();

		playerInvincibility = true;
		if (DOES_ENTITY_EXIST(me))
			SetPedInvincibleOn(me);

		switch (id)
		{
		case Id::Superman:
			superman = true;
			superJump = true;
			superRun = true;
			neverWanted = true;
			SpaceExtras::softLanding = true;
			if (!SmashAbility::g_smashAbility.Enabled())
				SmashAbility::g_smashAbility.TurnOn();
			Game::Print::PrintBottomLeft("~b~Superman:~s~ fly + punch + jump. Num7/Num1/+/−");
			break;

		case Id::Batman:
			superJump = true;
			neverWanted = true;
			ignoredByEveryone = true;
			SET_NIGHTVISION(TRUE);
			s_setNightVision = true;
			Game::Print::PrintBottomLeft("~b~Batman:~s~ night vision + jump + stealth.");
			break;

		case Id::Flash:
			superRun = true;
			neverWanted = true;
			SpaceExtras::softLanding = true;
			swimSpeedMult = 1.49f;
			SET_RUN_SPRINT_MULTIPLIER_FOR_PLAYER(PLAYER_ID(), 1.49f);
			Game::Print::PrintBottomLeft("~b~Flash:~s~ max sprint speed.");
			break;

		case Id::Hulk:
			superJump = true;
			forceField = 1; // push out
			if (!SmashAbility::g_smashAbility.Enabled())
				SmashAbility::g_smashAbility.TurnOn();
			Game::Print::PrintBottomLeft("~g~Hulk:~s~ smash punch + forcefield.");
			break;

		case Id::IronMan:
			superman = true;
			neverWanted = true;
			SpaceExtras::softLanding = true;
			Game::Print::PrintBottomLeft("~y~Iron Man:~s~ flight suit active.");
			break;

		case Id::SpiderMan:
			superJump = true;
			superRun = true;
			SpaceExtras::softLanding = true;
			Game::Print::PrintBottomLeft("~r~Spider-Man:~s~ leap + soft landing.");
			break;

		case Id::Wolverine:
			superRun = true;
			if (!SmashAbility::g_smashAbility.Enabled())
				SmashAbility::g_smashAbility.TurnOn();
			Game::Print::PrintBottomLeft("~o~Wolverine:~s~ regen + claws (melee smash).");
			break;

		case Id::Aquaman:
			playerWalkUnderwater = true;
			neverWanted = true;
			swimSpeedMult = 1.49f;
			Game::Print::PrintBottomLeft("~b~Aquaman:~s~ underwater walk + fast swim.");
			break;

		default:
			break;
		}
	}

	void Tick()
	{
		if (activeId == Id::None)
			return;

		Ped me = PLAYER_PED_ID();
		if (!DOES_ENTITY_EXIST(me))
			return;

		if (playerInvincibility)
			SetPedInvincibleOn(me);

		switch (activeId)
		{
		case Id::Flash:
			SET_RUN_SPRINT_MULTIPLIER_FOR_PLAYER(PLAYER_ID(), 1.49f);
			if (IS_PED_SPRINTING(me) || IS_PED_RUNNING(me))
			{
				Vector3 fwd = GET_ENTITY_FORWARD_VECTOR(me);
				APPLY_FORCE_TO_ENTITY_CENTER_OF_MASS(me, 1, fwd.x * 1.8f, fwd.y * 1.8f, 0.0f, true, false, true, false);
			}
			break;

		case Id::Wolverine:
			if (GET_ENTITY_HEALTH(me) < GET_ENTITY_MAX_HEALTH(me))
				SET_ENTITY_HEALTH(me, GET_ENTITY_HEALTH(me) + 4, 0);
			break;

		case Id::Batman:
			if (s_setNightVision)
				SET_NIGHTVISION(TRUE);
			break;

		case Id::Superman:
		case Id::IronMan:
			// Flight handled by existing superman loop.
			break;

		default:
			break;
		}
	}

	void Menu()
	{
		AddTitle("Hero Abilities");

		bool clear = false;
		AddOption("Clear Hero Powers", clear);
		if (clear)
			Clear();

		AddBreak("---Presets---");
		const Id ids[] = {
			Id::Superman, Id::Batman, Id::Flash, Id::Hulk,
			Id::IronMan, Id::SpiderMan, Id::Wolverine, Id::Aquaman
		};

		for (Id id : ids)
		{
			bool pressed = false;
			AddTickol(DisplayName(id), activeId == id, pressed, pressed, TICKOL::BOXTICK, TICKOL::BOXBLANK);
			if (pressed)
				Apply(id);
		}

		AddBreak("---Tip---");
		AddOption("Combine with Custom Skins folder", null);
	}
}

REGISTER_SUBMENU(HEROABILITIES, HeroAbilities::Menu)
