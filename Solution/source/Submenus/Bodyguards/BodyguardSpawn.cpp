#include "../../Natives/types.h"
#include "../../Natives/natives2.h"
#include "../../Natives/natives.h"

#include "BodyguardSpawn.h"
#include "BodyguardManagement.h"

#include "../../Menu/Menu.h"
#include "../../Scripting/Game.h"
#include "../../Scripting/GTAped.h"
#include "../../Scripting/GTAentity.h"
#include "../../Scripting/Model.h"
#include "../../macros.h"
#include <vector>
#include <string>

#include "../../Menu/Routine.h"
#include "../../Util/StringManip.h"
#include "BodyguardMenu.h"
#include "../../Scripting/enums.h"
#include "../../Scripting/World.h"

namespace sub::BodyguardMenu
{
	extern int health;
	extern int armor;
	extern bool godmode;
	std::string  _searchStr = std::string();

	static void QuickSpawn(const char* name, Hash modelHash)
	{
		sub::BodyguardMenu::BodyguardManagement::SpawnBodyguardPed(name, GTAmodel::Model(modelHash));
	}

	void BodyguardSpawn()
	{
			AddTitle("Spawn Bodyguard");

			AddBreak("---Quick Spawn---");
			bool q1 = false, q2 = false, q3 = false, q4 = false, q5 = false, q6 = false, q7 = false, q8 = false;
			AddOption("SWAT", q1);
			AddOption("Merryweather", q2);
			AddOption("Army", q3);
			AddOption("Police", q4);
			AddOption("FIB", q5);
			AddOption("Gang Ballas", q6);
			AddOption("Marine", q7);
			AddOption("Clone Player Model", q8);
			if (q1) QuickSpawn("SWAT", GET_HASH_KEY("s_m_y_swat_01"));
			if (q2) QuickSpawn("Merryweather", GET_HASH_KEY("s_m_y_blackops_01"));
			if (q3) QuickSpawn("Army", GET_HASH_KEY("s_m_y_marine_01"));
			if (q4) QuickSpawn("Police", GET_HASH_KEY("s_m_y_cop_01"));
			if (q5) QuickSpawn("FIB", GET_HASH_KEY("s_m_m_fibsec_01"));
			if (q6) QuickSpawn("Ballas", GET_HASH_KEY("g_m_y_ballaorig_01"));
			if (q7) QuickSpawn("Marine", GET_HASH_KEY("s_m_m_marine_01"));
			if (q8) QuickSpawn("Clone", GET_ENTITY_MODEL(PLAYER_PED_ID()));

			AddOption("Favourites", null, nullFunc, SUB::MODELCHANGER_FAVOURITES);
			AddOption("~b~Search~s~ Peds", null, nullFunc, SUB::MODELCHANGER_SEARCH);

			AddBreak("---Categories---");
			AddOption("Player", null, nullFunc, SUB::MODELCHANGER_PLAYER);
			AddOption("Animals", null, nullFunc, SUB::MODELCHANGER_ANIMAL);
			AddOption("Ambient Females", null, nullFunc, SUB::MODELCHANGER_AMBFEMALES);
			AddOption("Ambient Males", null, nullFunc, SUB::MODELCHANGER_AMBMALES);
			AddOption("Cutscene Models", null, nullFunc, SUB::MODELCHANGER_CS);
			AddOption("Gang Females", null, nullFunc, SUB::MODELCHANGER_GANGFEMALES);
			AddOption("Gang Males", null, nullFunc, SUB::MODELCHANGER_GANGMALES);
			AddOption("Story Models", null, nullFunc, SUB::MODELCHANGER_STORY);
			AddOption("Multiplayer Models", null, nullFunc, SUB::MODELCHANGER_MP);
			AddOption("Scenario Females", null, nullFunc, SUB::MODELCHANGER_SCENARIOFEMALES);
			AddOption("Scenario Males", null, nullFunc, SUB::MODELCHANGER_SCENARIOMALES);
			AddOption("Story Scenario Females", null, nullFunc, SUB::MODELCHANGER_ST_SCENARIOFEMALES);
			AddOption("Story Scenario Males", null, nullFunc, SUB::MODELCHANGER_ST_SCENARIOMALES);
			AddOption("Others", null, nullFunc, SUB::MODELCHANGER_OTHERS);
	}

}
namespace sub::BodyguardMenu::BodyguardManagement
{
	std::vector<Ped> s_bodyguards;
	void SpawnBodyguardPed(const std::string& text, const GTAmodel::Model& model)
	{
		if (sub::BodyguardMenu::BodyguardDb.size() >= MAX_BODYGUARDS)
		{
			Game::Print::PrintBottomLeft("Maximum of 7 bodyguards reached.");
			return;
		}

		GTAmodel::Model mdl = model;
		if (!mdl.IsInCdImage() || !mdl.IsPed())
		{
			Game::Print::PrintBottomLeft("~r~Invalid ped model.");
			return;
		}

		GTAped player(PLAYER_PED_ID());
		if (!player.Exists())
			return;

		if (!mdl.Load(8000))
		{
			Game::Print::PrintBottomLeft("~r~Failed to load bodyguard model.");
			return;
		}

		Vector3 pos = player.GetOffsetInWorldCoords(1.2f, 1.0f, 0.0f);
		float groundZ = pos.z;
		GET_GROUND_Z_FOR_3D_COORD(pos.x, pos.y, pos.z + 30.0f, &groundZ, FALSE, FALSE);
		pos.z = groundZ + 1.0f;
		const float heading = player.GetHeading();

		Ped ped = CREATE_PED(26, mdl.hash, pos.x, pos.y, pos.z, heading, FALSE, FALSE);
		if (!DOES_ENTITY_EXIST(ped) || ped == 0)
		{
			Game::Print::PrintBottomLeft("~r~Bodyguard spawn failed.");
			mdl.Unload();
			return;
		}

		SET_ENTITY_AS_MISSION_ENTITY(ped, true, true);
		SET_PED_DEFAULT_COMPONENT_VARIATION(ped);
		SET_PED_RANDOM_COMPONENT_VARIATION(ped, 0);
		SET_ENTITY_VISIBLE(ped, true, false);
		FREEZE_ENTITY_POSITION(ped, false);
		SET_ENTITY_COLLISION(ped, true, true);
		GTAentity(ped).PlaceOnGround();

		SET_ENTITY_MAX_HEALTH(ped, sub::BodyguardMenu::health);
		SET_ENTITY_HEALTH(ped, sub::BodyguardMenu::health, 0);
		SET_PED_ARMOUR(ped, sub::BodyguardMenu::armor);
		if (sub::BodyguardMenu::godmode)
			SetPedInvincibleOn(ped);
		else
			SetPedInvincibleOff(ped);

		int group = GET_PLAYER_GROUP(PLAYER_ID());
		SET_PED_AS_GROUP_LEADER(player.Handle(), group);
		SET_PED_AS_GROUP_MEMBER(ped, group);
		SET_PED_NEVER_LEAVES_GROUP(ped, true);
		SET_PED_CAN_BE_TARGETTED(ped, false);
		SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(ped, false);
		SET_PED_RELATIONSHIP_GROUP_HASH(ped, GET_PED_RELATIONSHIP_GROUP_HASH(player.Handle()));

		SET_PED_COMBAT_ABILITY(ped, sub::BodyguardMenu::aggressiveCombat ? 2 : 1);
		SET_PED_COMBAT_MOVEMENT(ped, sub::BodyguardMenu::aggressiveCombat ? 2 : 1);
		SET_PED_COMBAT_RANGE(ped, sub::BodyguardMenu::aggressiveCombat ? 2 : 1);
		SET_PED_COMBAT_ATTRIBUTES(ped, 0, true);
		SET_PED_COMBAT_ATTRIBUTES(ped, 1, true);
		SET_PED_COMBAT_ATTRIBUTES(ped, 2, true);
		SET_PED_COMBAT_ATTRIBUTES(ped, 3, true);
		SET_PED_COMBAT_ATTRIBUTES(ped, 5, sub::BodyguardMenu::aggressiveCombat);
		SET_PED_COMBAT_ATTRIBUTES(ped, 46, true);
		SET_PED_FLEE_ATTRIBUTES(ped, 0, false);
		SET_PED_SEEING_RANGE(ped, sub::BodyguardMenu::aggressiveCombat ? 120.0f : 60.0f);
		SET_PED_HEARING_RANGE(ped, sub::BodyguardMenu::aggressiveCombat ? 100.0f : 50.0f);
		SET_PED_ACCURACY(ped, sub::BodyguardMenu::aggressiveCombat ? 85 : 55);
		SET_PED_SHOOT_RATE(ped, sub::BodyguardMenu::aggressiveCombat ? 800 : 400);
		SET_PED_HIGHLY_PERCEPTIVE(ped, true);
		SET_PED_KEEP_TASK(ped, true);
		SET_PED_CAN_BE_KNOCKED_OFF_VEHICLE(ped, 1);
		SET_PED_CONFIG_FLAG(ped, 32, false);
		SET_PED_CONFIG_FLAG(ped, 184, true);
		SET_PED_CAN_RAGDOLL(ped, false);

		GIVE_WEAPON_TO_PED(ped, WEAPON_COMBATPISTOL, 250, false, true);
		GIVE_WEAPON_TO_PED(ped, WEAPON_CARBINERIFLE, 250, false, false);
		GIVE_WEAPON_TO_PED(ped, WEAPON_PUMPSHOTGUN, 60, false, false);
		SET_CURRENT_PED_WEAPON(ped, WEAPON_COMBATPISTOL, true);

		SET_GROUP_SEPARATION_RANGE(group, 9999.0f);
		SET_GROUP_FORMATION_SPACING(group, 1.5f, 2.5f, 3.5f);
		SET_PED_CAN_TELEPORT_TO_GROUP_LEADER(ped, group, true);
		TASK_FOLLOW_TO_OFFSET_OF_ENTITY(ped, player.Handle(), 0.8f, -1.2f, 0.0f, 2.0f, -1, 1.5f, true);

		BodyguardEntity ent{};
		ent.Handle = GTAentity(ped);
		ent.Type = EntityType::PED;
		ent.Name = text;
		ent.HashName = IntToHexString(mdl.hash, true);

		if (!DOES_ENTITY_EXIST(ped))
		{
			Game::Print::PrintBottomLeft("~r~Bodyguard vanished after spawn.");
			mdl.Unload();
			return;
		}

		BodyguardDb.push_back(std::move(ent));
		ApplyBodyguardBlip(ped, sub::BodyguardMenu::blipIcon);
		s_bodyguards.push_back(ped);

		char msg[96];
		sprintf_s(msg, "~g~Bodyguard spawned (%u/7)", (unsigned)BodyguardDb.size());
		Game::Print::PrintBottomLeft(msg);

		// Keep model referenced lightly; safe release for streaming.
		SET_MODEL_AS_NO_LONGER_NEEDED(mdl.hash);
	}

	void AddOptionBodyGuardPed(const std::string& text, const GTAmodel::Model& model)
	{
		bool bPressed = false;
		AddOption(text, bPressed);
		if (bPressed)
			SpawnBodyguardPed(text, model);
	}

}
