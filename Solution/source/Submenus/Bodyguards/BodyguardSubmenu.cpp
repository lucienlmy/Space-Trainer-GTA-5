#include "BodyguardManagement.h"
#include "BodyguardSettings.h"
#include "BodyguardSpawn.h"
#include "../../Menu/submenu_enum.h"
#include "../../Scripting/GTAped.h"
#include "../../Submenus/PedComponentChanger.h"
#include "BodyguardMenu.h"
#include "../../Submenus/WeaponOptions.h"
#include "../../Scripting/Camera.h"
#include "../../Scripting/World.h"
#include "../../Natives/natives2.h"
#include "../../Util/StringManip.h"
#include "../../Scripting/WeaponIndivs.h"
#include "../../Scripting/GTAvehicle.h"
#include "../../Scripting/Game.h"
#include "../../Scripting/enums.h"
#include "../../Menu/Routine.h"
#include "../../Menu/Menu.h"

namespace sub
{
    void ComponentChanger();
}

namespace sub::BodyguardMenu
{
    void SetEnt242() { g_Ped1= SelectedBodyguard->Handle.Handle(); }
    void BodyguardEntityOps()
    {
        // Determine the title dynamically
        std::string title = "Bodyguard";

        if (SelectedBodyguard)
        {
            if (SelectedBodyguard->Handle.Exists())
            {
                // Prefer a friendly name if provided
                if (!SelectedBodyguard->Name.empty())
                {
                    title = SelectedBodyguard->Name;
                }
                // Otherwise use a stored hash-name (if present)
                else if (!SelectedBodyguard->HashName.empty())
                {
                    title = SelectedBodyguard->HashName;
                }
                // Fallback: use the model hash as hex string
                else
                {
                    auto model = SelectedBodyguard->Handle.Model();
                    title = IntToHexString(model.hash, true);
                }
            }
            else
            {
                // Ped doesn't exist — show that in the title so it's obvious
                title = "Bodyguard (missing)";
            }
        }

        AddTitle(title);

        // Keep the rest of your existing logic unchanged
        if (!SelectedBodyguard)
        {
            AddOption("No bodyguard selected");
            return;
        }

        if (!SelectedBodyguard->Handle.Exists())
        {
            AddOption("Bodyguard no longer exists");
            return;
        }

        AddOption("Wardrobe", null, SetEnt242, SUB::COMPONENTS);
        if (g_cam_componentChanger.Exists())
        {
            g_cam_componentChanger.SetActive(false);
            g_cam_componentChanger.Destroy();
            World::SetRenderingCamera(0);
        }
        AddOption("Voice Changer", null, SetEnt242, SUB::VOICECHANGER);
        AddOption("Weapons", null, nullFunc, SUB::BODYGUARD_WEAPONOPS);
        AddOption("Loadouts", null, SetEnt242, SUB::WEAPONOPS_LOADOUTS);

        AddBreak("---Actions---");
        bool bTeleport = false, bHeal = false, bEnterVeh = false, bGiveWeap = false, bDelete = false;
        bool bSwitchTo = false;
        AddOption("Switch To Bodyguard (SP)", bSwitchTo);
        AddOption("Teleport To Me", bTeleport);
        AddOption("Heal", bHeal);
        AddOption("Give All Weapons", bGiveWeap);
        AddOption("Enter My Vehicle", bEnterVeh);
        AddOption("~r~Delete Bodyguard", bDelete);

        Ped ped = SelectedBodyguard->Handle.GetHandle();
        Ped playerPed = PLAYER_PED_ID();

        if (bSwitchTo)
        {
            if (NETWORK_IS_IN_SESSION())
            {
                Game::Print::PrintBottomLeft("~r~Switch only works in Single Player.");
            }
            else if (IS_PED_A_PLAYER(ped))
            {
                Game::Print::PrintBottomLeft("~y~Already controlling this ped.");
            }
            else
            {
                SetBecomePed(GTAped(ped));
                if (sub::BodyguardMenu::godmode)
                    SetPedInvincibleOn(PLAYER_PED_ID());
                Game::Print::PrintBottomLeft("~g~Switched to bodyguard.");
            }
        }
        if (bTeleport && DOES_ENTITY_EXIST(playerPed))
        {
            Vector3 pos = GET_ENTITY_COORDS(playerPed, true);
            Vector3 fwd = GET_ENTITY_FORWARD_VECTOR(playerPed);
            SET_ENTITY_COORDS_NO_OFFSET(ped, pos.x + fwd.x * 2.0f, pos.y + fwd.y * 2.0f, pos.z + 0.2f, false, false, false);
            Game::Print::PrintBottomLeft("Bodyguard teleported.");
        }
        if (bHeal)
        {
            SET_ENTITY_MAX_HEALTH(ped, sub::BodyguardMenu::health);
            SET_ENTITY_HEALTH(ped, sub::BodyguardMenu::health, 0);
            SET_PED_ARMOUR(ped, sub::BodyguardMenu::armor);
            if (sub::BodyguardMenu::godmode) SetPedInvincibleOn(ped);
            Game::Print::PrintBottomLeft("Bodyguard healed.");
        }
        if (bGiveWeap)
        {
            GiveAllWeaponsToPed(SelectedBodyguard->Handle, true);
            Game::Print::PrintBottomLeft("Weapons given.");
        }
        if (bEnterVeh)
        {
            if (IS_PED_IN_ANY_VEHICLE(playerPed, false))
            {
                Vehicle veh = GET_VEHICLE_PED_IS_IN(playerPed, false);
                GTAvehicle gtaVeh(veh);
                for (int s = 0; s <= 15; ++s)
                {
                    if (gtaVeh.IsSeatFree((VehicleSeat)s))
                    {
                        SET_PED_INTO_VEHICLE(ped, veh, s);
                        Game::Print::PrintBottomLeft("Bodyguard seated.");
                        break;
                    }
                }
            }
            else Game::Print::PrintBottomLeft("~r~You are not in a vehicle.");
        }
        if (bDelete)
        {
            BodyguardManagement::DeleteBodyguard(*SelectedBodyguard);
            SelectedBodyguard = nullptr;
            Menu::SetPreviousMenu();
            Game::Print::PrintBottomLeft("Bodyguard deleted.");
        }
    }
    void BodyguardWeaponOps()
    {
        if (!SelectedBodyguard || !SelectedBodyguard->Handle.Exists())
            return;

        Ped ped = SelectedBodyguard->Handle.GetHandle();

        g_WeaponOpsPedOverride = ped;
        g_WeaponOpsPlayerOverride = -1;
        g_WeaponMenuPedOverride = ped;


        WeaponIndivs_catind::Sub_CategoriesList();

        g_WeaponOpsPedOverride = 0;
        g_WeaponOpsPlayerOverride = -1;
        g_WeaponMenuPedOverride = 0;
    }
    void BodyguardWeaponLoadoutOps()
    {
        if (!SelectedBodyguard || !SelectedBodyguard->Handle.Exists())
            return;

        Ped ped = SelectedBodyguard->Handle.GetHandle();

        g_WeaponOpsPedOverride = ped;
        g_WeaponOpsPlayerOverride = -1;
        g_WeaponMenuPedOverride = ped;

        if (g_WeaponOpsPedOverride != 0)
        {
            g_Ped1 = g_WeaponOpsPedOverride;
            g_Ped2 = g_WeaponOpsPlayerOverride;
        }
        else
        {
            g_Ped1 = PLAYER::PLAYER_PED_ID();
            g_Ped2 = PLAYER::PLAYER_ID();
        }

        WeaponsLoadouts_catind::Sub_Loadouts_InItem();

        g_WeaponOpsPedOverride = 0;
        g_WeaponOpsPlayerOverride = -1;
        g_WeaponMenuPedOverride = 0;
    }

}

#include "..\..\Menu\submenu_switch.h"
#include "..\..\Menu\submenu_enum.h"
REGISTER_SUBMENU(BODYGUARD_ENTITYOPS,   sub::BodyguardMenu::BodyguardEntityOps)
REGISTER_SUBMENU(BODYGUARD_WEAPONOPS,   sub::BodyguardMenu::BodyguardWeaponOps)