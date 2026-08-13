#include "Gta2Cam.h"

#include "..\macros.h"

#include "..\Util\GTAmath.h"
#include "..\Natives\natives2.h"
#include "..\Scripting\Camera.h"
#include "..\Scripting\GTAentity.h"
#include "..\Scripting\GTAvehicle.h"
#include "..\Scripting\GTAped.h"
#include "..\Scripting\World.h"
#include "..\Scripting\GameplayCamera.h"

namespace GTA2Cam
{
	// Not using these as class members because I feel like it
	Camera gmCam2;
	Camera mainCam;
	Vector3 mainCamRelativePos;

	const Camera& Gta2Cam::MainCam()
	{
		return mainCam;
	}

	void Gta2Cam::CreateMainCam(GTAentity myPed)
	{
		if (myPed.Exists())
		{
			if (mainCam.Exists())
			{
				mainCam.SetActive(false);
				mainCam.Destroy();
			}
			Vector3 myPos = myPed.GetPosition();
			mainCamRelativePos = Vector3(0.0f, -0.5f, 19.0f);
			Vector3 mainCamPos = myPos + mainCamRelativePos;
			Vector3 mainCamRot = Vector3(-89.5, 0.0f, 0.0f);
			mainCam = World::CreateCamera(mainCamPos, mainCamRot, 45.0f);
		}
	}

	void Gta2Cam::TurnOn()
	{
		GenericLoopedMode::TurnOn();

		if (gmCam2.Exists())
		{
			gmCam2.SetActive(false);
			gmCam2.Destroy();
		}

		GTAentity myPed = PLAYER_PED_ID();

		Vector3 gmCamPos = GameplayCamera::GetPosition();
		Vector3 gmCamRot = GameplayCamera::GetRotation();
		float gmCamFov = GameplayCamera::GetFieldOfView();
		gmCam2 = World::CreateCamera(gmCamPos, gmCamRot, gmCamFov);

		CreateMainCam(myPed);

		gmCam2.InterpTo(mainCam, 2500, true, true);
		Camera::RenderScriptCams(true);
		mainCam.SetActive(true);
		// Keep gmCam2 until interp finishes; DoGta2CamTick cleans it when done.
	}
	void Gta2Cam::TurnOff()
	{
		GenericLoopedMode::TurnOff();

		if (gmCam2.Exists())
		{
			gmCam2.SetActive(false);
			gmCam2.Destroy();
		}
		if (mainCam.Exists())
		{
			mainCam.SetActive(false);
			mainCam.Destroy();
		}
		Camera::RenderScriptCams(false);
	}

	void Gta2Cam::Tick()
	{
		if (bEnabled)
		{
			DoGta2CamTick();
		}
	}
	inline void Gta2Cam::DoGta2CamTick()
	{
		GTAped myPed = PLAYER_PED_ID();

		if (mainCam.Exists())
		{
			if (gmCam2.Exists() && mainCam.IsActive())
			{
				gmCam2.Destroy();
				gmCam2 = Camera();
			}

			if (mainCam.IsActive())
			{
				Camera::RenderScriptCams(true);
				Vector3 myPos = myPed.GetPosition();

				mainCam.SetPosition(myPos + mainCamRelativePos);
				SET_THIRD_PERSON_CAM_RELATIVE_PITCH_LIMITS_THIS_UPDATE(0.0f, 0.0f);
				SET_THIRD_PERSON_CAM_RELATIVE_HEADING_LIMITS_THIS_UPDATE(0.0f, 0.0f);

				GTAvehicle myVeh = myPed.CurrentVehicle();

				if (myPed.IsOnFoot())
				{
					bool bSprinting = myPed.IsSprinting();
					if (bSprinting)
					{
						if (mainCamRelativePos.z < 22.0f)
							mainCamRelativePos.z += 0.14f;
					}
					else
					{
						if (mainCamRelativePos.z > 19.0f)
							mainCamRelativePos.z -= 0.14f;
					}
				}
				else if (myVeh.Exists())
				{
					float mySpeed = myVeh.GetSpeed() * 3.6f;
					if (mySpeed > 30.0f)
					{
						if (mainCamRelativePos.z < 26.0f)
							mainCamRelativePos.z += 0.14f;
					}
					else
					{
						if (mainCamRelativePos.z > 19.0f)
							mainCamRelativePos.z -= 0.14f;
					}
				}
			}
			else // Cam isn't active
			{
				mainCam.SetActive(true);
				Camera::RenderScriptCams(true);
			}
		}
		else // Cam doesn't exist
		{
			CreateMainCam(myPed);
		}
	}


	Gta2Cam g_gta2Cam;

	void ToggleOnOff()
	{
		g_gta2Cam.Toggle();
	}
}