#include "BodyguardMenu.h"

#include "../../Menu/Menu.h"
#include "../../Scripting/Game.h"
#include "../../Natives/natives2.h"
#include "../../Submenus/Spooner/Submenus.h"
#include "../../Submenus/PedAnimation.h"
#include "../../Submenus/PedModelChanger.h"
#include "../../Submenus/PedSpeech.h"

#include <functional>
#include "../../Menu/Routine.h"
#include "../../Misc/MeteorShower.h"
#include "../../Scripting/GTAvehicle.h"
#include "../../Scripting/Model.h"
#include "../../Submenus/Spooner/MenuOptions.h"
#include "../../Scripting/ModelNames.h"
#include "../../Util/StringManip.h"
#include "../../Util\ExePath.h"
#include "../../Submenus/Spooner/EntityManagement.h"
#include "../../Submenus/Spooner/SpoonerEntity.h"
#include "../../Util/FileLogger.h"

#include "BodyguardManagement.h"
#include "BodyguardSpawn.h"
#include "../../Scripting/WeaponIndivs.h"
#include "../../Scripting/GTAvehicle.h"
#include "../../Scripting/GTAped.h"
#include "../../Scripting/enums.h"

#include "../../Scripting/GTAblip.h"
using namespace sub::BodyguardMenu;

namespace sub::BodyguardMenu
{
    int armor = 200;
    int health = 200;
    bool godmode = true;
    bool aggressiveCombat = true;
    int blipIcon = 1;           // 1 = Standard, 280 = Friend, 480 = VIP
    int currentFormation = 0;
}

static constexpr int BLIP_COLOUR_BLUELIGHT = 3;

void sub::BodyguardMenu::RemoveBodyguardBlip(Ped ped)
{
    if (!ped || !ENTITY::DOES_ENTITY_EXIST(ped))
        return;

    Blip blip = GET_BLIP_FROM_ENTITY(ped);
    if (!blip)
        return;

    GTAblip gtaBlip(blip);
    if (gtaBlip.Exists())
        gtaBlip.Remove();
}

void sub::BodyguardMenu::ApplyBodyguardBlip(Ped ped, int icon)
{
    if (!ped || !ENTITY::DOES_ENTITY_EXIST(ped))
        return;

    RemoveBodyguardBlip(ped);

    Blip blip = ADD_BLIP_FOR_ENTITY(ped);
    if (!blip)
        return;

    SET_BLIP_SPRITE(blip, icon);
    SET_BLIP_SCALE(blip, 0.80f);
    SET_BLIP_COLOUR(blip, BLIP_COLOUR_BLUELIGHT);
    SET_BLIP_AS_FRIENDLY(blip, true);
}

void sub::BodyguardMenu::RefreshAllBodyguardBlips()
{
    for (unsigned int i = 0; i < sub::BodyguardMenu::BodyguardDb.size(); ++i)
    {
        auto& bg = sub::BodyguardMenu::BodyguardDb[i];
        if (!bg.Handle.Exists())
            continue;

        Ped ped = bg.Handle.GetHandle();
        if (!ped || !ENTITY::DOES_ENTITY_EXIST(ped))
            continue;

        int hp = ENTITY::GET_ENTITY_HEALTH(ped);
        if (hp <= 0)
            ApplyBodyguardBlip(ped, 274); // dead blip
        else
            ApplyBodyguardBlip(ped, sub::BodyguardMenu::blipIcon);
    }
}

void sub::BodyguardMenu::UpdateBodyguardBlipsOnDeath()
{
    for (unsigned int i = 0; i < sub::BodyguardMenu::BodyguardDb.size(); ++i)
    {
        auto& bg = sub::BodyguardMenu::BodyguardDb[i];
        if (!bg.Handle.Exists())
            continue;

        Ped ped = bg.Handle.GetHandle();
        if (!ped || !ENTITY::DOES_ENTITY_EXIST(ped))
            continue;

        int hp = ENTITY::GET_ENTITY_HEALTH(ped);
        if (hp <= 0)
        {
            Blip blip = GET_BLIP_FROM_ENTITY(ped);
            if (!blip || GET_BLIP_SPRITE(blip) != 274)
                ApplyBodyguardBlip(ped, 274);
        }
    }
}

namespace sub
{
    void BodyguardMainMenu()
    {
        bool bHealth_plus = false, bHealth_minus = false, bHealth_input = false;
        bool bArmor_plus = false, bArmor_minus = false, bArmor_input = false;
        int blipIndex = 0;
        static const std::vector<std::pair<int, std::string>> blipOptions =
        {
            { 1,   "Standard" },
            { 280, "Friend"   },
            { 480, "VIP"      }
        };
        bool bBlipInput = false;
        bool bIcon_Plus = false, bIcon_Minus = false;
        bool oldGodmode = sub::BodyguardMenu::godmode;

        int formationIndex = 0;
        static const std::vector<std::pair<int, std::string>> formationOptions =
        {
            { 0, "Default Formation" },
            { 1, "Circle (Inward)" },
            { 2, "Circle (North)" },
            { 3, "Line" }
        };

        for (size_t i = 0; i < blipOptions.size(); ++i)
        {
            if (blipOptions[i].first == sub::BodyguardMenu::blipIcon)
            {
                blipIndex = (int)i;
                break;
            }
        }

        for (size_t i = 0; i < formationOptions.size(); ++i)
        {
            if (formationOptions[i].first == sub::BodyguardMenu::currentFormation)
            {
                formationIndex = (int)i;
                break;
            }
        }

        bool bFormationInput = false;
        bool bForm_Plus = false, bForm_Minus = false;
        bool bTeleportBodyguards = false;

        AddTitle("Bodyguards");

        AddOption("Spawn Bodyguard", null, nullFunc, SUB::BODYGUARD_SPAWN);
        AddOption("Bodyguard List", null, nullFunc, SUB::BODYGUARD_LIST);
        AddOption("Animations", null, nullFunc, SUB::ANIMATIONSUB);
        AddOption("Scenarios", null, nullFunc, SUB::AnimationTaskScenarios);

        AddNumber("Default Health", sub::BodyguardMenu::health, 0, bHealth_input, bHealth_plus, bHealth_minus);
        if (bHealth_plus && sub::BodyguardMenu::health < INT_MAX) ++sub::BodyguardMenu::health;
        if (bHealth_minus && sub::BodyguardMenu::health > 0) --sub::BodyguardMenu::health;
        if (bHealth_input)
        {
            std::string inputStr = Game::InputBox("", 5U, "", std::to_string(sub::BodyguardMenu::health));
            if (!inputStr.empty())
            {
                try { sub::BodyguardMenu::health = std::stoi(inputStr); }
                catch (...) { Game::Print::PrintErrorInvalidInput(inputStr); }
            }
        }

        AddNumber("Default Armor", sub::BodyguardMenu::armor, 0, bArmor_input, bArmor_plus, bArmor_minus);
        if (bArmor_plus && sub::BodyguardMenu::armor < INT_MAX) ++sub::BodyguardMenu::armor;
        if (bArmor_minus && sub::BodyguardMenu::armor > 0) --sub::BodyguardMenu::armor;
        if (bArmor_input)
        {
            std::string inputStr = Game::InputBox("", 5U, "", std::to_string(sub::BodyguardMenu::armor));
            if (!inputStr.empty())
            {
                try { sub::BodyguardMenu::armor = std::stoi(inputStr); }
                catch (...) { Game::Print::PrintErrorInvalidInput(inputStr); }
            }
        }

        AddToggle("Godmode", sub::BodyguardMenu::godmode);

        if (oldGodmode != sub::BodyguardMenu::godmode)
        {
            for (unsigned int i = 0; i < sub::BodyguardMenu::BodyguardDb.size(); ++i)
            {
                auto& bg = sub::BodyguardMenu::BodyguardDb[i];
                if (!bg.Handle.Exists()) continue;

                Ped ped = bg.Handle.GetHandle();
                if (sub::BodyguardMenu::godmode) SetPedInvincibleOn(ped);
                else SetPedInvincibleOff(ped);
            }
        }


        AddTexter("Bodyguard Blip", 0, { blipOptions[blipIndex].second }, bBlipInput, bIcon_Plus, bIcon_Minus);

        if (bIcon_Plus) 
        {
            addlog(ige::LogType::LOG_TRACE, "Bodyguard Blip Plus Pressed");
            blipIndex = (blipIndex + 1) % blipOptions.size();
            sub::BodyguardMenu::blipIcon = blipOptions[blipIndex].first;
            sub::BodyguardMenu::RefreshAllBodyguardBlips();
			addlog(ige::LogType::LOG_DEBUG, "Bodyguard Blip Set: " + std::to_string(blipIndex));
        }
        if (bIcon_Minus)
        {
            addlog(ige::LogType::LOG_TRACE, "Bodyguard Blip Minus Pressed");
            blipIndex = (blipIndex == 0 ? (int)blipOptions.size() - 1 : blipIndex - 1);
            sub::BodyguardMenu::blipIcon = blipOptions[blipIndex].first;
            sub::BodyguardMenu::RefreshAllBodyguardBlips();
            addlog(ige::LogType::LOG_DEBUG, "Bodyguard Blip Set: " + std::to_string(blipIndex));
        }

		AddTexter("Formation", 0, { formationOptions[formationIndex].second }, bFormationInput, bForm_Plus, bForm_Minus);

        if (bForm_Plus)
        {
            addlog(ige::LogType::LOG_TRACE, "Formation Plus Pressed");
            formationIndex = (formationIndex + 1) % formationOptions.size();
            sub::BodyguardMenu::currentFormation = formationOptions[formationIndex].first; // ADD THIS
            int playerGroup = PLAYER::GET_PLAYER_GROUP(PLAYER::PLAYER_ID());
            PED::SET_GROUP_FORMATION(playerGroup, formationOptions[formationIndex].first);
            addlog(ige::LogType::LOG_DEBUG, "Formation Set: " + std::to_string(formationIndex));
        }
        if (bForm_Minus)
        {
            addlog(ige::LogType::LOG_TRACE, "Formation Minus Pressed");
            formationIndex = (formationIndex == 0 ? (int)formationOptions.size() - 1 : formationIndex - 1);
            sub::BodyguardMenu::currentFormation = formationOptions[formationIndex].first; // ADD THIS
            int playerGroup = PLAYER::GET_PLAYER_GROUP(PLAYER::PLAYER_ID());
            PED::SET_GROUP_FORMATION(playerGroup, formationOptions[formationIndex].first);
            addlog(ige::LogType::LOG_DEBUG, "Formation Set: " + std::to_string(formationIndex));
        }

        AddBreak("---Group Actions---");

        bool bGiveWeapons = false;
        bool bHealAll = false;
        bool bEnterVehicle = false;
        bool bDismissAll = false;

        AddToggle("Aggressive Combat", sub::BodyguardMenu::aggressiveCombat);
        AddOption("Give All Weapons", bGiveWeapons);
        AddOption("Heal All Bodyguards", bHealAll);
        AddOption("Put Bodyguards In My Vehicle", bEnterVehicle);
        AddOption("Bring Bodyguards To Self", bTeleportBodyguards);
        bool bCycleBodyguard = false;
        AddOption("Cycle Control (Next Bodyguard) (SP)", bCycleBodyguard);
        AddOption("Dismiss All Bodyguards", bDismissAll);

        if (bCycleBodyguard)
        {
            if (NETWORK_IS_IN_SESSION())
            {
                Game::Print::PrintBottomLeft("~r~Switch only works in Single Player.");
            }
            else if (sub::BodyguardMenu::BodyguardDb.empty())
            {
                Game::Print::PrintBottomLeft("~r~No bodyguards.");
            }
            else
            {
                static unsigned int s_cycleIndex = 0;
                if (s_cycleIndex >= sub::BodyguardMenu::BodyguardDb.size())
                    s_cycleIndex = 0;

                Ped current = PLAYER_PED_ID();
                unsigned int tries = 0;
                while (tries < sub::BodyguardMenu::BodyguardDb.size())
                {
                    auto& bg = sub::BodyguardMenu::BodyguardDb[s_cycleIndex];
                    s_cycleIndex = (s_cycleIndex + 1) % sub::BodyguardMenu::BodyguardDb.size();
                    ++tries;
                    if (!bg.Handle.Exists())
                        continue;
                    Ped ped = bg.Handle.GetHandle();
                    if (ped == current || IS_PED_DEAD_OR_DYING(ped, true))
                        continue;
                    SetBecomePed(GTAped(ped));
                    if (sub::BodyguardMenu::godmode)
                        SetPedInvincibleOn(PLAYER_PED_ID());
                    Game::Print::PrintBottomLeft("~g~Switched to bodyguard.");
                    break;
                }
            }
        }

        if (bGiveWeapons)
        {
            for (auto& bg : sub::BodyguardMenu::BodyguardDb)
            {
                if (!bg.Handle.Exists()) continue;
                GiveAllWeaponsToPed(bg.Handle, true);
            }
            Game::Print::PrintBottomLeft("Weapons given to bodyguards.");
        }

        if (bHealAll)
        {
            for (auto& bg : sub::BodyguardMenu::BodyguardDb)
            {
                if (!bg.Handle.Exists()) continue;
                Ped ped = bg.Handle.GetHandle();
                ENTITY::SET_ENTITY_MAX_HEALTH(ped, sub::BodyguardMenu::health);
                ENTITY::SET_ENTITY_HEALTH(ped, sub::BodyguardMenu::health, 0);
                PED::SET_PED_ARMOUR(ped, sub::BodyguardMenu::armor);
                if (sub::BodyguardMenu::godmode) SetPedInvincibleOn(ped);
            }
            Game::Print::PrintBottomLeft("Bodyguards healed.");
        }

        if (bEnterVehicle)
        {
            Ped playerPed = PLAYER_PED_ID();
            if (IS_PED_IN_ANY_VEHICLE(playerPed, false))
            {
                Vehicle veh = GET_VEHICLE_PED_IS_IN(playerPed, false);
                GTAvehicle gtaVeh(veh);
                int seated = 0;
                for (auto& bg : sub::BodyguardMenu::BodyguardDb)
                {
                    if (!bg.Handle.Exists()) continue;
                    bool found = false;
                    for (int s = 0; s <= 15; ++s)
                    {
                        if (gtaVeh.IsSeatFree((VehicleSeat)s))
                        {
                            SET_PED_INTO_VEHICLE(bg.Handle.GetHandle(), veh, s);
                            found = true;
                            seated++;
                            break;
                        }
                    }
                    if (!found) break;
                }
                Game::Print::PrintBottomLeft(oss_ << "Seated " << seated << " bodyguard(s).");
            }
            else
            {
                Game::Print::PrintBottomLeft("~r~You are not in a vehicle.");
            }
        }

        if (bDismissAll)
        {
            while (!sub::BodyguardMenu::BodyguardDb.empty())
            {
                auto& bg = sub::BodyguardMenu::BodyguardDb.back();
                sub::BodyguardMenu::BodyguardManagement::DeleteBodyguard(bg);
            }
            Game::Print::PrintBottomLeft("All bodyguards dismissed.");
        }

        if (bTeleportBodyguards)
        {
            Ped playerPed = PLAYER_PED_ID();
            if (ENTITY::DOES_ENTITY_EXIST(playerPed))
            {
                Vector3 playerPos = ENTITY::GET_ENTITY_COORDS(playerPed, true);
                Vector3 forward = ENTITY::GET_ENTITY_FORWARD_VECTOR(playerPed);

                float baseDist = 3.0f;
                float spacing = 0.75f;
                int placed = 0;

                for (unsigned int i = 0; i < sub::BodyguardMenu::BodyguardDb.size(); ++i)
                {
                    auto& bg = sub::BodyguardMenu::BodyguardDb[i];
                    if (!bg.Handle.Exists())
                        continue;

                    Ped ped = bg.Handle.GetHandle();

                    Vector3 targetPos =
                        playerPos +
                        (forward * baseDist) +
                        Vector3(0.0f, 0.0f, 0.2f) +
                        (forward * (spacing * placed));

                    ENTITY::SET_ENTITY_COORDS_NO_OFFSET(
                        ped,
                        targetPos.x,
                        targetPos.y,
                        targetPos.z,
                        false, false, false
                    );

                    placed++;
                }

                Game::Print::PrintBottomLeft("Bodyguards teleported.");
            }
        }
    }
}


#include "..\..\Menu\submenu_switch.h"
#include "..\..\Menu\submenu_enum.h"
REGISTER_SUBMENU(BODYGUARDMAINMENU,         sub::BodyguardMainMenu)
REGISTER_SUBMENU(BODYGUARD_SPAWN,           sub::BodyguardMenu::BodyguardSpawn)