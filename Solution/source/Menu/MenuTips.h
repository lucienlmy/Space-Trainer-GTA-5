#pragma once

#include <string>
#include "..\Scripting\Model.h"

namespace MenuTips
{
	extern std::string g_hoveredTip;

	void Clear();
	void SetHoveredTip(const std::string& tipEnglish);
	void SetHoveredTipRaw(const std::string& tipAlreadyTranslated);
	void ApplyBuiltinTipForOption(const std::string& optionEnglish);

	bool IsVehicleSpawnUnsafe(const GTAmodel::Model& model);
	std::string GetVehicleSpawnTip(const GTAmodel::Model& model);
	void DrawHoveredTipFooter();
}
