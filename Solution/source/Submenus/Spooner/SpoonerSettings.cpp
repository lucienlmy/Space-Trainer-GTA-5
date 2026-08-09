#include "SpoonerSettings.h"

#include "..\..\macros.h"

//#include "..\..\Menu\Menu.h"
//#include "..\..\Menu\Routine.h"

//#include "..\..\Natives\natives2.h"

#include <string>
#include <vector>

namespace sub::Spooner
{
	//enum class eSpoonerModeMode : UINT8 { GroundEase, Precision };
	const std::vector<std::string> spoonerModeModeNames{ "Surface Ease", "Precision" };

	namespace Settings
	{
//#define GTA_MAX_ENTITIES 2048

		float cameraMovementSensitivityKeyboard = 0.33f;
		float cameraRotationSensitivityMouse = 6.5f;
		float cameraMovementSensitivityGamepad = 0.9f;
		float cameraRotationSensitivityGamepad = 1.4f;

		eSpoonerModeMode spoonerModeMode = eSpoonerModeMode::GroundEase;

		bool bShowModelPreviews = true;
		bool bDisplaySpoonerInfo = true;
		bool bShowBoxAroundSelectedEntity = false;
		bool bSpawnDynamicProps = false;
		bool bSpawnDynamicPeds = true;
		bool bSpawnDynamicVehicles = true;
		bool bFreezeEntityWhenMovingIt = false;
		bool bSpawnInvincibleEntities = false;
		bool bSpawnStillPeds = true;
		bool bAddToDbAsMissionEntities = true;
		bool bKeepPositionWhenAttaching = false;
		
		bool bTeleportToReferenceWhenLoadingFile = true;

		bool bGridSnapEnabled = false;
		float gridSnapSize = 1.0f;
		float rotationSnapDegrees = 0.0f;
	}

}



