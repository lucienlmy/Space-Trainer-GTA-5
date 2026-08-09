#include "DxHookIMG.h"

#include "..\macros.h"

#include "..\Util\GTAmath.h"
#include "..\Natives\natives2.h"
#include "..\Util\FileLogger.h"
#include "..\Menu\Menu.h"
#include "..\Menu\MenuConfig.h"

#include "..\Util\ExePath.h"

#include "..\Submenus\VehicleOptions.h"
#include "..\Submenus\TimeOptions.h"

namespace DxHookIMG
{
	int DxTexture::globalDrawOrder = -9999;

	bool DxTexture::operator == (const DxTexture& right) const
	{
		return (this->id == right.id);
	}
	DxTexture& DxTexture::operator = (const DxTexture& right)
	{
		this->id = right.id;
		return *this;
	}

	DxTexture::DxTexture() : id(0)//, instanceIndex(0)
	{
	}

	DxTexture::DxTexture(int iD) : id(iD)//, instanceIndex(0)
	{
	}

	DxTexture::DxTexture(const std::string& file)
	{
		this->Load(file);
	}

	void DxTexture::Load(const std::string& file)
	{
		if (does_file_exist(file))
		{
			addlog(ige::LogType::LOG_INFO,  "[DX-HOOK] Loading texture: " + file);
			this->id = createTexture(file.c_str());

			/*auto slashPos = file.rfind("\\");
			if (slashPos == std::string::npos) slashPos = file.rfind("//");
			if (slashPos == std::string::npos) slashPos = 0;
			this->name = file.substr(slashPos, file.rfind(".") - slashPos);*/
		}
		else
		{
			addlog(ige::LogType::LOG_ERROR,  "[DX-HOOK] Unable to find: " + file);
			this->id = 0;
		}
	}

	bool DxTexture::Exists()
	{
		return this->id != 0;
	}

	int& DxTexture::GlobalDrawOrderRef()
	{
		return DxTexture::globalDrawOrder;
	}

	int& DxTexture::ID()
	{
		return this->id;
	}

	void DxTexture::Draw(UINT8 index, const Vector2& position, const Vector2& size, float rotationOutOf360, const RGBA& colour)
	{
		//DRAW_SPRITE("SpaceExtras", this->name.c_str(), position.x, position.y, size.x, size.y, rotationOutOf360, colour.R, colour.G, colour.B, colour.A);
		drawTexture(this->id, index, DxTexture::globalDrawOrder, 55, size.x, size.y, 0.5f, 0.5f, position.x, position.y, rotationOutOf360 / 360.0f, GET_ASPECT_RATIO(false), static_cast<float>(colour.R) / 255.0f, static_cast<float>(colour.G) / 255.0f, static_cast<float>(colour.B) / 255.0f, static_cast<float>(colour.A) / 255.0f);
		DxTexture::globalDrawOrder++;
	}
	void DxTexture::DrawAdvanced(UINT8 index, int level, int time, const Vector2& centrePos, const Vector2& position, const Vector2& size, float rotationOutOf360, const RGBA& colour)
	{
		//DRAW_SPRITE("SpaceExtras", this->name.c_str(), position.x, position.y, size.x, size.y, rotationOutOf360, colour.R, colour.G, colour.B, colour.A);
		drawTexture(this->id, index, level, time, size.x, size.y, centrePos.x, centrePos.y, position.x, position.y, rotationOutOf360 / 360.0f, GET_ASPECT_RATIO(false), static_cast<float>(colour.R) / 255.0f, static_cast<float>(colour.G) / 255.0f, static_cast<float>(colour.B) / 255.0f, static_cast<float>(colour.A) / 255.0f);
	}

	//DxTexture titleui_spooner;
	DxTexture teleToWpBoxIconKeyboard;
	DxTexture teleToWpBoxIconGamepad;
	void LoadAllSpaceTexturesInit()
	{
		sub::Speedo::LoadSpeedoImages();
		sub::Clock::LoadClockImages();
		//DxHookIMG::titleui_spooner.Load(GetPathffA(Pathff::Graphics, true) + "titleui_spooner.png");
		DxHookIMG::teleToWpBoxIconKeyboard.Load(GetPathffA(Pathff::Graphics, true) + "teleportToWaypointBoxKeyboard.png");
		DxHookIMG::teleToWpBoxIconGamepad.Load(GetPathffA(Pathff::Graphics, true) + "teleportToWaypointBoxGamepad.png");

	}
}



