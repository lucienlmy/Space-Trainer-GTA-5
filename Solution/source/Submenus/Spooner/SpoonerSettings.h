#pragma once

#include <string>
#include <vector>

typedef unsigned char UINT8;

namespace sub::Spooner
{
	enum class eSpoonerModeMode : UINT8 { GroundEase, Precision };
	extern const std::vector<std::string> spoonerModeModeNames;

	namespace Settings
	{
#define GTA_MAX_ENTITIES 2048

		extern float cameraMovementSensitivityKeyboard;
		extern float cameraRotationSensitivityMouse;
		extern float cameraMovementSensitivityGamepad;
		extern float cameraRotationSensitivityGamepad;

		extern eSpoonerModeMode spoonerModeMode;

		extern bool bShowModelPreviews;
		extern bool bDisplaySpoonerInfo;
		extern bool bShowBoxAroundSelectedEntity;
		extern bool bSpawnDynamicProps;
		extern bool bSpawnDynamicPeds;
		extern bool bSpawnDynamicVehicles;
		extern bool bFreezeEntityWhenMovingIt;
		extern bool bSpawnInvincibleEntities;
		extern bool bSpawnStillPeds;
		extern bool bAddToDbAsMissionEntities;
		extern bool bKeepPositionWhenAttaching;

		extern bool bTeleportToReferenceWhenLoadingFile;

		extern bool bGridSnapEnabled;
		extern float gridSnapSize;
		extern float rotationSnapDegrees;
	}

}



