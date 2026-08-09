#include "CustomHelpText.h"

#include "..\macros.h"

#include "..\Natives\natives2.h"
#include "..\Menu\Language.h"

#include <Windows.h> // PCHAR, GetTickCount
#include <string>
#include <sstream>

namespace Game
{
	DWORD CustomHelpText::_timer = 0;
	std::string CustomHelpText::_tag = "Space Help:~n~";
	std::string CustomHelpText::_text;

	bool CustomHelpText::Drawing()
	{
		return IS_HELP_MESSAGE_BEING_DISPLAYED() != 0;
	}

	void CustomHelpText::SetTag(const std::string& newTag)
	{
		_tag = newTag;
	}

	void CustomHelpText::SetText(const std::string& newText)
	{
		_text = _tag + Language::TranslateToSelected(newText);
	}

	void CustomHelpText::ShowThisFrame(const std::string& textToShow)
	{
		if (DOES_TEXT_LABEL_EXIST(textToShow.c_str()))
		{
			DISPLAY_HELP_TEXT_THIS_FRAME(textToShow.c_str(), 0);
		}
		else
		{
			if (textToShow.length() < 100)
			{
				BEGIN_TEXT_COMMAND_DISPLAY_HELP("STRING");
				ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(textToShow.c_str());
			}
			else
			{
				BEGIN_TEXT_COMMAND_DISPLAY_HELP("jamyfafi");
				add_text_component_long_string(textToShow);
			}
			END_TEXT_COMMAND_DISPLAY_HELP(0, false, false, -1);
			//DISPLAY_HELP_TEXT_THIS_FRAME("STRING", 0);
		}

	}
	void CustomHelpText::ShowThisFrame(std::ostream& s)
	{
		ShowThisFrame((dynamic_cast<std::ostringstream&>(s).str()));
	}
	void CustomHelpText::ShowThisFrame(std::wostream& s)
	{
		std::wstring wtext = (dynamic_cast<std::wostringstream&>(s).str());
		ShowThisFrame(std::string(wtext.begin(), wtext.end()));
	}
	void CustomHelpText::ShowThisFrame()
	{
		ShowThisFrame(_text);
	}

	void CustomHelpText::End()
	{
		/*if (Drawing())
		{
			HIDE_HELP_TEXT_THIS_FRAME();
		}*/
		_timer = 0;
	}

	void CustomHelpText::Tick()
	{
		if (_timer)
		{
			DWORD time_now = GetTickCount();
			if (time_now <= _timer)
			{
				ShowThisFrame();
			}
			else
			{
				End();
			}
		}
	}

	void CustomHelpText::ShowTimedText(const std::string& text, DWORD how_many_ms)
	{
		if (!_timer) PLAY_SOUND_FRONTEND(-1, "WEAPON_SELECT_ARMOR", "HUD_AMMO_SHOP_SOUNDSET", 0);
		SetText(text);
		_timer = GetTickCount() + how_many_ms;
	}
	void CustomHelpText::ShowTimedText(std::ostream& s, DWORD how_many_ms)
	{
		ShowTimedText((dynamic_cast<std::ostringstream&>(s).str()), how_many_ms);
	}
	void CustomHelpText::ShowTimedText(std::wostream& s, DWORD how_many_ms)
	{
		std::wstring wtext = (dynamic_cast<std::wostringstream&>(s).str());
		ShowTimedText(std::string(wtext.begin(), wtext.end()), how_many_ms);
	}

}


