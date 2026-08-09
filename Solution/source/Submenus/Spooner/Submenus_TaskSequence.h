#pragma once

namespace sub::Spooner
{
	class STSTask;

	namespace Submenus
	{
		extern STSTask* _selectedSTST;

		namespace Sub_TaskSequence
		{
			void Nothing();
			void SetHealth();

			void AddBlip();
			void RemoveBlip();

			void Pause();
			void UsePhone();
			void ThrowProjectile();
			void Writhe();
			void FaceDirection();
			void FaceEntity();
			void LookAtCoord();
			void LookAtEntity();
			void LookAtCoordEyesOnly();
			void LookAtEntityEyesOnly();
			void TeleportToCoord();
			void SeekCoverAtCoord();
			void SlideToCoord();
			void GoToCoord();
			void FollowRoute();
			void FollowEntity();
			void PatrolInRange();
			void WanderFreely();
			void FleeFromCoord();
			void NearestAppropriateAction();
			void ScenarioAction();
			void ScenarioAction_list();
			void PlayAnimation();
			void PlayAnimation_settings();
			void PlayAnimation_allPedAnims();
			void PlayAnimation_allPedAnims_inDict();
			void SetActiveWeapon();
			void AimAtCoord();
			void AimAtEntity();
			void ShootAtCoord();
			void ShootAtEntity();
			void FightHatedTargets();
			void FightPed();
			void SpeakToPed();
			void PlaySpeechWithVoice();
			void PlaySpeechWithVoice_inVoice();

			void WarpIntoVehicle();
			void EnterVehicle();
			void DriveWander();
			void DriveToCoord();
			void DriveFollowEntity();
			void DriveLandPlane();

			void AchieveVehicleForwardSpeed();

			void ChangeTextureVariation();

			void AchieveVelocity();
			void AchievePushForce();
			void OscillateToPoint();
			void OscillateToEntity();
			void FreezeInPlace();
			void SetRotation();
			void ChangeOpacity();
			void TriggerFx();
		}
		void Sub_TaskSequence_TaskList();
		void Sub_TaskSequence_AddTask();
		void Sub_TaskSequence_InTask();

	}

}



