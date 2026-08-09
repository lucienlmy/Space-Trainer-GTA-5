/*
* Space Trainer - data-driven main hub categories
*/
#pragma once

#include "..\Menu\submenu_enum.h"
#include <string>
#include <vector>

namespace SpaceMenuData
{
	struct Category
	{
		const char* id;
		const char* title;
		const char* description;
		int submenu;
	};

	const std::vector<Category>& MainCategories();
}
