#include "PedModelChanger.h"

#include "..\macros.h"

#include "..\Menu\Menu.h"
#include "..\Menu\Routine.h"

#include "..\Natives\natives2.h"
#include "..\Scripting\Model.h"
#include "..\Util\ExePath.h"
#include "..\Util\FileLogger.h"
#include "..\Util\StringManip.h"
#include "..\Util\keyboard.h"
#include "..\Scripting\Game.h"
#include "..\Scripting\ModelNames.h"
#include "..\Scripting\GTAped.h"
#include "..\Scripting\GTAvehicle.h"
#include "..\Scripting\Model.h"
#include "..\Scripting\PTFX.h"
#include "..\Scripting\WeaponIndivs.h"

#include "..\Submenus\PedComponentChanger.h"
#include "..\Submenus\WeaponOptions.h"
#include "..\Submenus\Spooner\SpoonerEntity.h"
#include "..\Submenus\Spooner\Databases.h"
#include "..\Submenus\Spooner\EntityManagement.h"
#include "..\Submenus\Spooner\MenuOptions.h"
#include "..\Submenus\Bodyguards\BodyguardSpawn.h"

#include <string>
#include <vector>
#include <pugixml\src\pugixml.hpp>

namespace sub
{
	namespace PedFavourites
	{
		std::string xmlFavouritePeds = "FavouritePeds.xml";
		std::string searchStr = std::string();

		void ClearSearchStr() 
		{ 
			searchStr.clear(); 
		}

		bool IsPedAFavourite(GTAmodel::Model model)
		{
			pugi::xml_document doc;
			if (doc.load_file((const char*)(GetPathffA(Pathff::Main, true) + xmlFavouritePeds).c_str()).status != pugi::status_ok)
			{
				return false;
			}
			pugi::xml_node nodeRoot = doc.document_element();
			return nodeRoot.find_child_by_attribute("hash", IntToHexString(model.hash, true).c_str()) != NULL;
		}
		bool AddPedToFavourites(GTAmodel::Model model, const std::string& customName)
		{
			if (customName.empty())
			{
				return false;
			}
			pugi::xml_document doc;
			if (doc.load_file((const char*)(GetPathffA(Pathff::Main, true) + xmlFavouritePeds).c_str()).status != pugi::status_ok)
			{
				doc.reset();
				auto nodeDecleration = doc.append_child(pugi::node_declaration);
				nodeDecleration.append_attribute("version") = "1.0";
				nodeDecleration.append_attribute("encoding") = "ISO-8859-1";
				auto nodeRoot = doc.append_child("FavouriteWeapons");
				doc.save_file((const char*)(GetPathffA(Pathff::Main, true) + xmlFavouritePeds).c_str());
			}
			pugi::xml_node nodeRoot = doc.document_element();
			auto nodeOldLoc = nodeRoot.find_child_by_attribute("hash", IntToHexString(model.hash, true).c_str());
			if (nodeOldLoc) // If not null
			{
				nodeOldLoc.parent().remove_child(nodeOldLoc);
			}
			auto nodeNewLoc = nodeRoot.append_child("Ped");
			nodeNewLoc.append_attribute("hash") = IntToHexString(model.hash, true).c_str();
			nodeNewLoc.append_attribute("customName") = customName.c_str();
			return (doc.save_file((const char*)(GetPathffA(Pathff::Main, true) + xmlFavouritePeds).c_str()));
		}

		bool RemovePedFromFavourites(GTAmodel::Model model)
		{
			pugi::xml_document doc;
			if (doc.load_file((const char*)(GetPathffA(Pathff::Main, true) + xmlFavouritePeds).c_str()).status != pugi::status_ok)
			{
				return false;
			}
			pugi::xml_node nodeRoot = doc.document_element();
			auto nodeOldLoc = nodeRoot.find_child_by_attribute("hash", IntToHexString(model.hash, true).c_str());
			if (nodeOldLoc) // If not null
			{
				nodeOldLoc.parent().remove_child(nodeOldLoc);
			}
			return (doc.save_file((const char*)(GetPathffA(Pathff::Main, true) + xmlFavouritePeds).c_str()));
		}

		void ShowInstructionalButton(GTAmodel::Model model)
		{
			bool bIsAFav = IsPedAFavourite(model);
			if (Menu::bitController)
			{
				Menu::add_IB(INPUT_SCRIPT_RLEFT, (!bIsAFav ? "Add to" : "Remove from") + (std::string)" favourites");
				if (IS_DISABLED_CONTROL_JUST_PRESSED(2, INPUT_SCRIPT_RLEFT))
				{
					!bIsAFav ? AddPedToFavourites(model, Game::InputBox("", 28U, "Enter custom name:", GetPedModelLabel(model, true))) : RemovePedFromFavourites(model);
				}
			}
			else
			{
				Menu::add_IB(VirtualKey::B, (!bIsAFav ? "Add to" : "Remove from") + (std::string)" favourites");
				if (IsKeyJustUp(VirtualKey::B))
				{
					!bIsAFav ? AddPedToFavourites(model, Game::InputBox("", 28U, "Enter custom name:", GetPedModelLabel(model, true))) : RemovePedFromFavourites(model);
				}
			}
		}

		void PedFavouritesMenu()
		{
			Menu::OnSubBack = ClearSearchStr;
			AddTitle("Favourites");

			pugi::xml_document doc;
			if (doc.load_file((const char*)(GetPathffA(Pathff::Main, true) + xmlFavouritePeds).c_str()).status != pugi::status_ok)
			{
				doc.reset();
				auto nodeDeclaration = doc.append_child(pugi::node_declaration);
				nodeDeclaration.append_attribute("version") = "1.0";
				nodeDeclaration.append_attribute("encoding") = "ISO-8859-1";
				auto nodeRoot = doc.append_child("FavouritePeds");
				doc.save_file((const char*)(GetPathffA(Pathff::Main, true) + xmlFavouritePeds).c_str());
				return;
			}
			pugi::xml_node nodeRoot = doc.document_element();

			bool inputAdd = false;
			AddOption("Add New Ped Model", inputAdd); if (inputAdd)
			{
				std::string hashNameStr = Game::InputBox("", 40U, "Enter model name (e.g. IG_BENNY):");
				if (hashNameStr.length())
				{
					Model hashNameHash = GET_HASH_KEY(hashNameStr);
					if (hashNameHash.IsInCdImage())
					{
						WAIT(500);
						std::string customNameStr = Game::InputBox("", 28U, "Enter custom name:", GetPedModelLabel(hashNameHash, true));
						if (customNameStr.length())
						{
							if (AddPedToFavourites(hashNameHash, customNameStr))
							{
								Game::Print::PrintBottomLeft("Ped model ~b~added~s~.");
							}
							else 
							{
								Game::Print::PrintBottomLeft("~r~Error:~s~ Unable to add ped model.");
							}
						}
						else 
						{
							Game::Print::PrintErrorInvalidInput(customNameStr);
						}
					}
					else 
					{
						Game::Print::PrintErrorInvalidModel(hashNameStr);
					}
				}
				else 
				{
					Game::Print::PrintErrorInvalidInput(hashNameStr);
				}
			}

			if (nodeRoot.first_child())
			{
				AddBreak("---Added Ped Models---");

				bool bSearchPressed = false;
				AddOption(searchStr.empty() ? "SEARCH" : searchStr, bSearchPressed, nullFunc, -1, true); if (bSearchPressed)
				{
					searchStr = Game::InputBox(searchStr, 126U, "SEARCH", boost::to_lower_copy(searchStr));
					boost::to_upper(searchStr);
				}

				for (auto nodeLocToLoad = nodeRoot.first_child(); nodeLocToLoad; nodeLocToLoad = nodeLocToLoad.next_sibling())
				{
					const std::string& customName = nodeLocToLoad.attribute("customName").as_string();
					Model model = nodeLocToLoad.attribute("hash").as_uint();

					if (!searchStr.empty()) 
					{
						if (boost::to_upper_copy(customName).find(searchStr) == std::string::npos) 
						{
							continue;
						}
					}
					AddModelOption(customName, model);
				}
			}
		}
	}

	void ChangeModel(GTAmodel::Model model)
	{
		if (!model.IsInCdImage() || !model.IsPed())
		{
			Game::Print::PrintBottomLeft("~r~Invalid ped model.");
			return;
		}

		if (!model.Load(5000))
		{
			Game::Print::PrintBottomLeft("~r~Failed to load model.");
			model.Unload();
			return;
		}

		GTAped playerPed = PLAYER_PED_ID();
		if (!playerPed.Exists())
		{
			model.Unload();
			return;
		}

		if (sub::PedDamageTextures::vPedsAndDamagePacks.count(playerPed.Handle()))
			sub::PedDamageTextures::vPedsAndDamagePacks.erase(playerPed.Handle());
		if (sub::PedDecals::vPedsAndDecals.count(playerPed.Handle()))
			sub::PedDecals::vPedsAndDecals.erase(playerPed.Handle());

		std::vector<s_Weapon_Components_Tint> weaponsBackup;
		playerPed.StoreWeaponsInArray(weaponsBackup);
		Hash currWeaponHash = playerPed.GetWeapon();

		GTAentity att;
		auto spi = sub::Spooner::EntityManagement::GetEntityIndexInDb(playerPed);
		if (spi >= 0)
		{
			auto& spe = sub::Spooner::Databases::EntityDb[spi];
			sub::Spooner::EntityManagement::GetEntityThisEntityIsAttachedTo(spe.handle, att);
		}

		bool wasInVehicle = playerPed.IsInVehicle();
		GTAvehicle vehicle;
		VehicleSeat currentVehSeat;
		if (wasInVehicle)
		{
			vehicle = playerPed.CurrentVehicle();
			currentVehSeat = playerPed.GetCurrentVehicleSeat();
		}

		bool hasCollision = playerPed.GetIsCollisionEnabled();
		SET_PLAYER_MODEL(PLAYER_ID(), model.hash);

		// Wait briefly for the new player ped handle.
		for (int i = 0; i < 30 && (!DOES_ENTITY_EXIST(PLAYER_PED_ID()) || GET_ENTITY_MODEL(PLAYER_PED_ID()) != model.hash); ++i)
			WAIT(0);

		playerPed = PLAYER_PED_ID();
		if (!playerPed.Exists())
		{
			model.Unload();
			Game::Print::PrintBottomLeft("~r~Model change failed.");
			return;
		}

		playerPed.SetIsCollisionEnabled(hasCollision);
		SET_PED_DEFAULT_COMPONENT_VARIATION(playerPed.Handle());
		SET_PED_RANDOM_COMPONENT_VARIATION(playerPed.Handle(), 0);
		model.Unload();

		if (wasInVehicle && vehicle.Exists())
			playerPed.SetIntoVehicle(vehicle, currentVehSeat);

		if (playerPed.PedType() == PedType::Animal && !HAS_ANIM_DICT_LOADED("creatures@rottweiler@melee@streamed_core@"))
		{
			REQUEST_ANIM_DICT("creatures@rottweiler@melee@streamed_core@");
			REQUEST_ANIM_DICT("creatures@cougar@melee@streamed_core@");
		}

		playerPed.GiveWeaponsFromArray(weaponsBackup);
		if (IS_WEAPON_VALID(currWeaponHash))
			playerPed.SetWeapon(currWeaponHash);

		SET_PED_INFINITE_AMMO_CLIP(playerPed.Handle(), bitInfiniteAmmo);

		// Don't auto-restore a missing/broken defaultPed.xml over the new skin.
		defaultPedSet = true;

		if (spi >= 0)
		{
			auto& spe = sub::Spooner::Databases::EntityDb[spi];
			GTAentity oldPlayerPed = spe.handle;
			spe.handle = playerPed;
			spe.hashName = GetPedModelLabel(model, true);
			if (spe.hashName.length() == 0)
				IntToHexString(model.hash, true);
			spe.ClearLastAnimations();
			spe.currentScenario.clear();
			if (att.Exists() && spe.attachmentArgs.isAttached)
			{
				spe.handle.AttachTo(att, spe.attachmentArgs.boneIndex, spe.handle.GetIsCollisionEnabled(), spe.attachmentArgs.offset, spe.attachmentArgs.rotation);
			}
			spe.taskSequence.Reset();
			if (sub::Spooner::selectedEntity.handle.Equals(oldPlayerPed))
				sub::Spooner::selectedEntity = spe;
		}

		Game::Print::PrintBottomLeft("~g~Model changed.");
	}

	void AddModelChangerOption(const std::string& text, const GTAmodel::Model& model, int tickTrue)
	{
		const GTAped& ped = Game::PlayerPed();
		const Model& pedModel = ped.Model();

		bool pressed = false;
		AddTickol(text, model.Equals(pedModel), pressed, pressed, static_cast<TICKOL>(tickTrue)); if (pressed)
		{
			ChangeModel(model);
			addlog(ige::LogType::LOG_TRACE, "Changed model to: " + text);
		}
	}

	static int GetPedSelectContext()
	{
		for (int ci = Menu::currentArrayIndex; ci >= 0; ci--)
		{
			if (Menu::currentArray[ci] == SUB::SPOONER_SPAWN_PED) return 1;
			if (Menu::currentArray[ci] == SUB::BODYGUARD_SPAWN) return 2;
			if (Menu::currentArray[ci] == SUB::PEDGUN_ALLPEDS) return 3;
		}
		return 0;
	}

	static void ApplyPedModelSelection(const std::string& text, const GTAmodel::Model& model, bool* extraOptionCode = nullptr)
	{
		if (!model.IsInCdImage())
		{
			Game::Print::PrintBottomLeft("~r~Ped list empty or model missing. Copy PedList.xml into SpaceStuff.");
			return;
		}

		switch (GetPedSelectContext())
		{
		case 1:
			sub::Spooner::MenuOptions::AddOptionAddPed(text, model);
			break;
		case 2:
			sub::BodyguardMenu::BodyguardManagement::AddOptionBodyGuardPed(text, model);
			break;
		case 3:
			AddPedGunOption(text, model.hash, extraOptionCode);
			break;
		default:
			ChangeModel(model);
			break;
		}
	}

	void AddModelOption(const std::string& text, const GTAmodel::Model& model, bool* extraOptionCode, int tickTrue)
	{
		if (!model.IsInCdImage())
			return;

		int context = GetPedSelectContext();
		switch (context)
		{
		case 1:
			sub::Spooner::MenuOptions::AddOptionAddPed(text, model);
			break;
		case 2:
			sub::BodyguardMenu::BodyguardManagement::AddOptionBodyGuardPed(text, model);
			break;
		case 3:
			AddPedGunOption(text, model.hash, extraOptionCode);
			break;
		default:
			AddModelChangerOption(text, model.hash, tickTrue);
			break;
		}

		if (*Menu::currentopATM == Menu::printingop)
			PedFavourites::ShowInstructionalButton(model);
	}

	std::pair<std::string, std::string> rngped;

	static void AddRandomPedOption(std::vector<std::pair<std::string, std::string>>& list)
	{
		bool pressed = false;
		AddOption("Random", pressed);
		if (!pressed)
			return;

		if (list.empty())
		{
			Game::Print::PrintBottomLeft("~r~No peds loaded. Put PedList.xml in SpaceStuff.");
			return;
		}

		rngped = list[std::rand() % list.size()];
		GTAmodel::Model model = rngped.first;
		const int context = GetPedSelectContext();
		if (context == 2)
			sub::BodyguardMenu::BodyguardManagement::SpawnBodyguardPed(rngped.second, model);
		else if (context == 0)
			ChangeModel(model);
		else if (context == 3)
			AddPedGunOption(rngped.second, model.hash, nullptr);
		else
			Game::Print::PrintBottomLeft("~y~Pick a ped from the list.");
	}

	void ModelChangerMenu()
	{
		bool modelChangerRandomPedVariation = false;
		bool modelChangerInput = false;
		rngped = { "", "" };
		dict3.clear();

		g_Ped1 = PLAYER_PED_ID();
		AddTitle("Model Changer");
		AddOption("~b~Search~s~ Peds", null, nullFunc, SUB::MODELCHANGER_SEARCH);
		AddOption("Favourites", null, nullFunc, SUB::MODELCHANGER_FAVOURITES);
		AddOption("Randomize Ped Variation", modelChangerRandomPedVariation);

		AddBreak("---Categories---");
		AddOption("Player", null, nullFunc, SUB::MODELCHANGER_PLAYER);
		AddOption("Story Models", null, nullFunc, SUB::MODELCHANGER_STORY);
		AddOption("Cutscene Models", null, nullFunc, SUB::MODELCHANGER_CS);
		AddOption("Multiplayer Models", null, nullFunc, SUB::MODELCHANGER_MP);
		AddOption("Animals", null, nullFunc, SUB::MODELCHANGER_ANIMAL);

		AddBreak("---Ambient---");
		AddOption("Ambient Females", null, nullFunc, SUB::MODELCHANGER_AMBFEMALES);
		AddOption("Ambient Males", null, nullFunc, SUB::MODELCHANGER_AMBMALES);
		AddOption("Gang Female", null, nullFunc, SUB::MODELCHANGER_GANGFEMALES);
		AddOption("Gang Males", null, nullFunc, SUB::MODELCHANGER_GANGMALES);

		AddBreak("---Scenario---");
		AddOption("Scenario Females", null, nullFunc, SUB::MODELCHANGER_SCENARIOFEMALES);
		AddOption("Scenario Males", null, nullFunc, SUB::MODELCHANGER_SCENARIOMALES);
		AddOption("Story Scenario Females", null, nullFunc, SUB::MODELCHANGER_ST_SCENARIOFEMALES);
		AddOption("Story Scenario Males", null, nullFunc, SUB::MODELCHANGER_ST_SCENARIOMALES);
		AddOption("Others", null, nullFunc, SUB::MODELCHANGER_OTHERS);
		AddOption("~b~Input~s~ Model", modelChangerInput);

		if (modelChangerRandomPedVariation)
		{
			addlog(ige::LogType::LOG_TRACE, "Random Ped Selected");
			SET_PED_RANDOM_COMPONENT_VARIATION(g_Ped1, 0);
			SET_PED_RANDOM_PROPS(g_Ped1);
			return;
		}

		if (modelChangerInput)
		{
			std::string inputStr = Game::InputBox("", 64U, "Enter ped model name (e.g. IG_BENNY):");
			if (inputStr.length() > 0)
			{
				Model model = (inputStr);
				if (model.IsInCdImage())
				{
					ChangeModel(model);
				}
				else
				{
					Game::Print::PrintErrorInvalidModel(inputStr);
				}
				return;
			}
		}
	}

	GTAmodel::Model ModelChangerRandom(std::vector<std::pair<std::string, std::string>> pedModels)
	{
		addlog(ige::LogType::LOG_TRACE, "Getting Random Ped Model");
		if (pedModels.empty()) return 0;
		rngped = pedModels[std::rand() % pedModels.size()];
		addlog(ige::LogType::LOG_TRACE, "Got Random Ped Model: " + rngped.first + ", " + rngped.second);
		return rngped.first;
	}

	void ModelChangerPlayer()
	{
		AddTitle("Player");
		AddRandomPedOption(g_pedModels_Player);
		for (auto& pmn : g_pedModels_Player)
		{
			AddModelOption(pmn.second, (pmn.first));
		}
	}

	void ModelChangerAnimal()
	{
		AddTitle("Animals");
		AddRandomPedOption(g_pedModels_Animal);
		for (auto& pmn : g_pedModels_Animal)
		{
			AddModelOption(pmn.second, (pmn.first));
		}
	}

	void ModelChangerAmbientFemale()
	{
		AddTitle("Ambient Females");
		AddRandomPedOption(g_pedModels_AmbientFemale);
		for (auto& pmn : g_pedModels_AmbientFemale)
		{
			AddModelOption(pmn.second, (pmn.first));
		}
	}

	void ModelChangerAmbientMale()
	{
		AddTitle("Ambient Males");
		AddRandomPedOption(g_pedModels_AmbientMale);
		for (auto& pmn : g_pedModels_AmbientMale)
		{
			AddModelOption(pmn.second, (pmn.first));
		}
	}

	void ModelChangerCutscene()
	{
		AddTitle("Cutscene Models");
		AddRandomPedOption(g_pedModels_Cutscene);
		for (auto& pmn : g_pedModels_Cutscene)
		{
			AddModelOption(pmn.second, (pmn.first));
		}
	}

	void ModelChangerGangFemale()
	{
		AddTitle("Gang Females");
		AddRandomPedOption(g_pedModels_GangFemale);
		for (auto& pmn : g_pedModels_GangFemale)
		{
			AddModelOption(pmn.second, (pmn.first));
		}
	}

	void ModelChangerGangMale()
	{
		AddTitle("Gang Males");
		AddRandomPedOption(g_pedModels_GangMale);
		for (auto& pmn : g_pedModels_GangMale)
		{
			AddModelOption(pmn.second, (pmn.first));
		}
	}

	void ModelChangerStory()
	{
		AddTitle("Story Models");
		AddRandomPedOption(g_pedModels_Story);
		for (auto& pmn : g_pedModels_Story)
		{
			AddModelOption(pmn.second, (pmn.first));
		}
	}

	void ModelChangerMultiplayer()
	{
		AddTitle("Multiplayer Models");
		AddRandomPedOption(g_pedModels_Multiplayer);
		for (auto& pmn : g_pedModels_Multiplayer)
		{
			AddModelOption(pmn.second, (pmn.first));
		}
	}

	void ModelChangerScenarioFemale()
	{
		AddTitle("Scenario Females");
		AddRandomPedOption(g_pedModels_ScenarioFemale);
		for (auto& pmn : g_pedModels_ScenarioFemale)
		{
			AddModelOption(pmn.second, (pmn.first));
		}
	}

	void ModelChangerScenarioMale()
	{
		AddTitle("Scenario Males");
		AddRandomPedOption(g_pedModels_ScenarioMale);
		for (auto& pmn : g_pedModels_ScenarioMale)
		{
			AddModelOption(pmn.second, (pmn.first));
		}
	}

	void ModelChangerStoryScenarioFemale()
	{
		AddTitle("Story Scenario Females");
		AddRandomPedOption(g_pedModels_StoryScenarioFemale);
		for (auto& pmn : g_pedModels_StoryScenarioFemale)
		{
			AddModelOption(pmn.second, (pmn.first));
		}
	}

	void ModelChangerStoryScenarioMale()
	{
		AddTitle("Story Scenario Males");
		AddRandomPedOption(g_pedModels_StoryScenarioMale);
		for (auto& pmn : g_pedModels_StoryScenarioMale)
		{
			AddModelOption(pmn.second, (pmn.first));
		}
	}
	
	void ModelChangerOthers()
	{
		AddTitle("Others");
		AddRandomPedOption(g_pedModels_Others);
		for (auto& pmn : g_pedModels_Others)
		{
			AddModelOption(pmn.second, (pmn.first));
		}
	}

	namespace PedSearch
	{
		static int sortIndex = 0;
		static int categoryFilter = 0;
		static std::vector<std::pair<std::string, std::string>> results;
		static std::vector<std::string> categoryOptions;
		static bool dirty = true;
		static std::string lastSearch;
		static int lastSort = -1, lastCategory = -1;

		static const std::vector<std::string> sortOptions = { "Name (A-Z)", "Name (Z-A)", "Model Name" };

		struct CategoryEntry {
			const char* name;
			std::vector<std::pair<std::string, std::string>>* vec;
		};

		static const CategoryEntry categories[] = {
			{ "Player", &g_pedModels_Player },
			{ "Animals", &g_pedModels_Animal },
			{ "Ambient Females", &g_pedModels_AmbientFemale },
			{ "Ambient Males", &g_pedModels_AmbientMale },
			{ "Cutscene", &g_pedModels_Cutscene },
			{ "Gang Females", &g_pedModels_GangFemale },
			{ "Gang Males", &g_pedModels_GangMale },
			{ "Story", &g_pedModels_Story },
			{ "Multiplayer", &g_pedModels_Multiplayer },
			{ "Scenario Females", &g_pedModels_ScenarioFemale },
			{ "Scenario Males", &g_pedModels_ScenarioMale },
			{ "Story Scenario F", &g_pedModels_StoryScenarioFemale },
			{ "Story Scenario M", &g_pedModels_StoryScenarioMale },
			{ "Others", &g_pedModels_Others },
		};
		static const int numCategories = 14;

		void BuildCategoryOptions()
		{
			categoryOptions.clear();
			categoryOptions.push_back("All (" + std::to_string(g_pedModels.size()) + ")");
			for (int i = 0; i < numCategories; i++)
			{
				categoryOptions.push_back(std::string(categories[i].name) + " (" + std::to_string(categories[i].vec->size()) + ")");
			}
		}

		void RebuildResults(const std::string& searchStr)
		{
			results.clear();
			std::string searchUpper = boost::to_upper_copy(searchStr);

			auto* sourceVec = &g_pedModels;
			if (categoryFilter > 0 && categoryFilter <= numCategories)
				sourceVec = categories[categoryFilter - 1].vec;

			for (auto& ped : *sourceVec)
			{
				if (!searchUpper.empty())
				{
					std::string nameUpper = boost::to_upper_copy(ped.second);
					std::string modelUpper = boost::to_upper_copy(ped.first);
					if (nameUpper.find(searchUpper) == std::string::npos &&
						modelUpper.find(searchUpper) == std::string::npos)
						continue;
				}
				results.push_back(ped);
			}

			switch (sortIndex)
			{
			case 0:
				std::sort(results.begin(), results.end(), [](const auto& a, const auto& b) {
					return boost::to_lower_copy(a.second) < boost::to_lower_copy(b.second);
				});
				break;
			case 1:
				std::sort(results.begin(), results.end(), [](const auto& a, const auto& b) {
					return boost::to_lower_copy(a.second) > boost::to_lower_copy(b.second);
				});
				break;
			case 2:
				std::sort(results.begin(), results.end(), [](const auto& a, const auto& b) {
					return boost::to_lower_copy(a.first) < boost::to_lower_copy(b.first);
				});
				break;
			}
		}
	}

	void ModelChangerSearchMenu()
	{
		using namespace PedSearch;
		auto& searchStr = dict3;

		if (categoryOptions.empty()) BuildCategoryOptions();

		AddTitle("Ped Search");

		bool bSearchPressed = false;
		AddOption(searchStr.empty() ? "~b~SEARCH~s~" : ("~b~" + searchStr + "~s~"), bSearchPressed, nullFunc, -1, true);
		if (bSearchPressed)
		{
			searchStr = Game::InputBox(searchStr, 64U, "Search peds:", boost::to_lower_copy(searchStr));
			boost::to_upper(searchStr);
		}

		bool sortRight = false, sortLeft = false;
		AddTexter("Sort", sortIndex, sortOptions, null, sortRight, sortLeft);
		if (sortRight && sortIndex < (int)(sortOptions.size() - 1)) sortIndex++;
		if (sortLeft && sortIndex > 0) sortIndex--;

		bool catRight = false, catLeft = false;
		AddTexter("Category", categoryFilter, categoryOptions, null, catRight, catLeft);
		if (catRight && categoryFilter < (int)(categoryOptions.size() - 1)) categoryFilter++;
		if (catLeft && categoryFilter > 0) categoryFilter--;

		if (dirty || searchStr != lastSearch || sortIndex != lastSort || categoryFilter != lastCategory)
		{
			RebuildResults(searchStr);
			lastSearch = searchStr;
			lastSort = sortIndex;
			lastCategory = categoryFilter;
			dirty = false;
		}

		AddBreak("---Results: " + std::to_string(results.size()) + "---");

		for (auto& ped : results)
		{
			AddModelOption(ped.second, (ped.first));
		}
	}

} // namespace sub


#include "..\Menu\submenu_switch.h"
#include "..\Menu\submenu_enum.h"
REGISTER_SUBMENU(MODELCHANGER,                     sub::ModelChangerMenu)
REGISTER_SUBMENU(MODELCHANGER_FAVOURITES,          sub::PedFavourites::PedFavouritesMenu)
REGISTER_SUBMENU(MODELCHANGER_PLAYER,              sub::ModelChangerPlayer)
REGISTER_SUBMENU(MODELCHANGER_ANIMAL,              sub::ModelChangerAnimal)
REGISTER_SUBMENU(MODELCHANGER_AMBFEMALES,          sub::ModelChangerAmbientFemale)
REGISTER_SUBMENU(MODELCHANGER_AMBMALES,            sub::ModelChangerAmbientMale)
REGISTER_SUBMENU(MODELCHANGER_CS,                  sub::ModelChangerCutscene)
REGISTER_SUBMENU(MODELCHANGER_GANGFEMALES,         sub::ModelChangerGangFemale)
REGISTER_SUBMENU(MODELCHANGER_GANGMALES,           sub::ModelChangerGangMale)
REGISTER_SUBMENU(MODELCHANGER_STORY,               sub::ModelChangerStory)
REGISTER_SUBMENU(MODELCHANGER_MP,                  sub::ModelChangerMultiplayer)
REGISTER_SUBMENU(MODELCHANGER_SCENARIOFEMALES,     sub::ModelChangerScenarioFemale)
REGISTER_SUBMENU(MODELCHANGER_SCENARIOMALES,       sub::ModelChangerScenarioMale)
REGISTER_SUBMENU(MODELCHANGER_ST_SCENARIOFEMALES,  sub::ModelChangerStoryScenarioFemale)
REGISTER_SUBMENU(MODELCHANGER_ST_SCENARIOMALES,    sub::ModelChangerStoryScenarioMale)
REGISTER_SUBMENU(MODELCHANGER_OTHERS,              sub::ModelChangerOthers)
REGISTER_SUBMENU(MODELCHANGER_SEARCH,              sub::ModelChangerSearchMenu)