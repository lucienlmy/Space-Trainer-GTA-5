#pragma once

#include "..\..\Scripting\GTAentity.h"
#include "..\..\Scripting\Model.h"

#include <utility>
#include <set>

typedef unsigned char UINT8, BYTE;
typedef unsigned short UINT16;

class Camera;

namespace sub::Spooner
{
	class SpoonerEntity;

	namespace SpoonerMode
	{
		extern BYTE bindsKeyboard;
		extern std::pair<UINT16, UINT16> bindsGamepad;

		extern bool bEnabled;
		extern bool bIsSomethingHeld;
		extern bool bHeldEntityHasCollision;

		enum class eEntityEditMode : UINT8 { Disabled, Keyboard, Gizmo };
		extern eEntityEditMode entityEditMode;
		enum class eGizmoMode : UINT8 { Translate, Rotate, Scale };
		extern eGizmoMode gizmoMode;
		extern bool bGizmoCameraLocked;
		extern bool bGizmoLocalSpace; // false = world-aligned, true = entity-local axes
		extern Camera spoonerModeCamera;
		extern float spoonerModeCameraCamDistance;

		struct SpoonerStats {
			int totalNumEntities;
			int totalNumProps;
			int totalNumPeds;
			int totalNumVehicles;
		};
		SpoonerStats GetSpoonerStats();

		bool IsHotkeyPressed();

		struct ModelPreviewInfoStructure
		{
			EntityType entityType;
			Model previousModel,
				model;
			GTAentity entity;
			std::set<GTAentity> previousEntities;
		};
		extern ModelPreviewInfoStructure modelPreviewInfo;
		extern float previewYawOffset;
		void SpawnModelPreview();

		void ResetSelectedEntity();
		bool GetEntityPtr(GTAentity& inEntity, SpoonerEntity*& outEntity);
		SpoonerEntity GetEntityPtrValue(GTAentity& entity);
		inline void SetAsSelectedEntity(GTAentity& entity);
		Vector3 SnapPos(Vector3 pos);
		Vector3 SnapRotation(Vector3 rot);

		inline void CamTick();
		void Tick();

		void TurnOn();
		void TurnOff();
		void Toggle();
	}

}
