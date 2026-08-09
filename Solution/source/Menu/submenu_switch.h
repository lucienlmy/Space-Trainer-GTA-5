#pragma once

#include <unordered_map>

typedef void(*SubmenuHandler)();

class SubMenuRegistry
{
public:
	static void Register(int sub, SubmenuHandler handler);
	static void Dispatch(int sub);
	static bool IsRegistered(int sub);

private:
	static std::unordered_map<int, SubmenuHandler>& GetMap();
};

// Macro for self-registration from submenu .cpp files.
// Usage: REGISTER_SUBMENU(MAINMENU, sub::MainMenu)
// Place at file scope in any .cpp that defines a submenu handler.
#define REGISTER_SUBMENU(sub_enum, func) \
	static struct _Reg_##sub_enum { \
		_Reg_##sub_enum() { SubMenuRegistry::Register(SUB::sub_enum, func); } \
	} _reg_instance_##sub_enum;
