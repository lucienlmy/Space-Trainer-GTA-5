#include "MainMenu.h"
#include "Settings.h"
#include "..\Menu\SpaceTheme.h"

namespace sub
{
	void Settings()
	{
		auto& bSyncWithConfig = MenuConfig::bSaveAtIntervals;

		AddTitle("Settings");

		bool bChangeLangPressed = false;
		AddTexter("Language", 0, { Language::GetSelectedLangTitle() }, bChangeLangPressed);
		if (bChangeLangPressed)
		{
			Menu::SetSub_delayed = SUB::SETTINGS_LANGUAGE;
		}

		AddOption("Themes", null, nullFunc, SUB::SETTINGS_THEMES);
		AddOption("Menu Colours", null, nullFunc, SUB::SETTINGS_COLOURS);
		AddOption("Menu Fonts", null, nullFunc, SUB::SETTINGS_FONTS);
		AddOption("Menu Position", null, nullFunc, SUB::SETTINGS_MENUPOS);
		AddOption("Appearance", null, nullFunc, SUB::APPEARANCE_HUB);
		AddOption("PC Graphics (Weak/High)", null, nullFunc, SUB::GRAPHICSQUALITY);
		AddToggle("Mouse Support", Menu::bit_mouse);
		AddToggle("Gradients", Menu::gradients);
		AddToggle("Titlebox Glare", Menu::bit_glare_test);
		AddToggle("Centre Title", Menu::bit_centre_title);
		AddToggle("Centre Options", Menu::bit_centre_options);
		AddToggle("Centre Breaks", Menu::bit_centre_breaks);
		AddToggle("Reset Player Model Upon Death (SP)", checkSelfDeathModel);
		AddToggle("Sync Space With Config File", bSyncWithConfig, MenuConfig::SaveConfig, MenuConfig::SaveConfig);
		AddOption("Reset Toggles (Most Of Them)", null, MenuConfig::ConfigResetHaxValues);

	}

	void SettingsMenuPos()
	{
		bool settingsXPosPlus = false;
		bool settingsXPosMinus = false;
		bool settingsYPosPlus = false;
		bool settingsYPosMinus = false;

		AddTitle("Menu Position");
		AddNumber("Offset X", menuPos.x * 100 + 6.0, 2, null, settingsXPosPlus, settingsXPosMinus);
		AddNumber("Offset Y", menuPos.y * 100 + 7.4, 2, null, settingsYPosPlus, settingsYPosMinus);

		const float maxX = SpaceTheme::spaceUiEnabled ? 0.10f : 0.7397f;
		const float minX = SpaceTheme::spaceUiEnabled ? -0.02f : -0.0598f;
		const float maxY = SpaceTheme::spaceUiEnabled ? 0.28f : 0.85f;
		const float minY = SpaceTheme::spaceUiEnabled ? -0.04f : -0.074f;

		if (settingsXPosPlus)
		{
			if (menuPos.x < maxX)
				menuPos.x += 0.002f;
			if (menuPos.x > maxX)
				menuPos.x = maxX;
			return;
		}
		else if (settingsXPosMinus)
		{
			if (menuPos.x > minX)
				menuPos.x -= 0.002f;
			if (menuPos.x < minX)
				menuPos.x = minX;
			return;
		}

		if (settingsYPosPlus)
		{
			if (menuPos.y < maxY)
				menuPos.y += 0.002f;
			if (menuPos.y > maxY)
				menuPos.y = maxY;
			return;
		}
		else if (settingsYPosMinus)
		{
			if (menuPos.y > minY)
				menuPos.y -= 0.002f;
			if (menuPos.y < minY)
				menuPos.y = minY;
			return;
		}


	}

	RGBA* g_settingsRGBA;
	int*g_settingsRGBA2;
	INT8* g_settingsFont;
	UINT8 settingsHUDColor = 0ui8;

	void AddsettingscolOption(const std::string& text, RGBA& feature)
	{
		auto& settings_rgba = g_settingsRGBA;
		
		bool pressed = false;
		AddOption(text, pressed, nullFunc, SUB::SETTINGS_COLOURS2);

		if (*Menu::currentopATM == Menu::printingop)
		{
			AddPresetColourOptionsPreviews(feature);
		}

		if (pressed)
		{
			settings_rgba = &feature;
		}
	}

	void SettingsColours()
	{
		AddTitle("Menu Colours");
		AddsettingscolOption("Title Box", titlebox);
		AddsettingscolOption("Background", BG);
		AddsettingscolOption("Title Text", titletext);
		AddsettingscolOption("Option Text", optiontext);
		AddsettingscolOption("Selected Text", selectedtext);
		AddsettingscolOption("Option Breaks", optionbreaks);
		AddsettingscolOption("Option Count", optioncount);
		AddsettingscolOption("Selection Box", selectionhi);
		AddsettingscolOption("Ped Trackers", _globalPedTrackers_Col);
		AddToggle("Rainbow", rainbowBoxes);
	}
	void SettingsColours2()
	{
		bool settingsRInput = false;
		bool settingsRPlus = false;
		bool settingsRMinus = false; 
		bool settingsHUDColourApply = false; 
		bool settingsHUDColourPlus = false;
		bool settingsHUDColourMinus = false;

		auto& settingsRGBA = g_settingsRGBA;
		auto& settingsRGBA2 = g_settingsRGBA2;

		AddTitle("Set Colour");
		AddNumber("Red", settingsRGBA->R, 0, settingsRInput, settingsRPlus, settingsRMinus);

		switch (*Menu::currentopATM)
		{
		case 1:
		case 2:
		case 3:
			AddPresetColourOptionsPreviews(settingsRGBA->R, settingsRGBA->G, settingsRGBA->B);
			break;
		}

		AddNumber("Green", settingsRGBA->G, 0, settingsRInput, settingsRPlus, settingsRMinus);
		AddNumber("Blue", settingsRGBA->B, 0, settingsRInput, settingsRPlus, settingsRMinus);
		AddNumber("Opacity", settingsRGBA->A, 0, settingsRInput, settingsRPlus, settingsRMinus);
		AddTexter("HUD Colour", settingsHUDColor, HudColour::vHudColours, settingsHUDColourApply, settingsHUDColourPlus, settingsHUDColourMinus);
		AddBreak("---Presets---");
		AddPresetColourOptions(settingsRGBA->R, settingsRGBA->G, settingsRGBA->B);

		switch (*Menu::currentopATM)
		{
			case 1: 
				settingsRGBA2 = &settingsRGBA->R;
				break;
			case 2: 
				settingsRGBA2 = &settingsRGBA->G;
				break;
			case 3: 
				settingsRGBA2 = &settingsRGBA->B;
				break;
			case 4: 
				settingsRGBA2 = &settingsRGBA->A;
				break;
		}

		if (settingsRInput) 
		{
			int tempHash = *settingsRGBA2;
			try 
			{
				tempHash = abs(std::stoi(Game::InputBox(std::to_string(*settingsRGBA2), 4U, "", std::to_string(*settingsRGBA2))));
			}
			catch (...)
			{
				Game::Print::PrintErrorInvalidInput(std::to_string(tempHash)); 
			}

			if (!(tempHash >= 0 && tempHash <= 255)) 
			{
				Game::Print::PrintErrorInvalidInput(std::to_string(tempHash));
			}
			else 
			{
				*settingsRGBA2 = tempHash;
			}
			return;
		}

		if (settingsRPlus) 
		{
			if (*settingsRGBA2 < 255)
			{
				(*settingsRGBA2)++;
			}
			else 
			{
				*settingsRGBA2 = 0;
			}
			return;
		}
		else if (settingsRMinus) 
		{
			if (*settingsRGBA2 > 0)
			{
				(*settingsRGBA2)--;
			}
			else
			{
				*settingsRGBA2 = 255;
			}
			return;
		}

		if (settingsHUDColourPlus) 
		{ 
			if (settingsHUDColor < HudColour::vHudColours.size() - 1) 
			{
				settingsHUDColor++;
			}
			else
			{
				settingsHUDColor = 0;
			}
			return; 
		}
		if (settingsHUDColourMinus) 
		{ 
			if (settingsHUDColor > 0) 
			{
				settingsHUDColor--;
			}
			else
			{
				settingsHUDColor = 180;
			}
			return; 
		}
		if (settingsHUDColourApply)
		{
			GET_HUD_COLOUR(settingsHUDColor, &settingsRGBA->R, &settingsRGBA->G, &settingsRGBA->B, &inull);
			return; 
		}
	}

	void AddsettingsfonOption(const std::string& text, int fontIndex, INT8& feature)
	{
		auto& settingsFont = g_settingsFont;

		bool bitChangeFont = false;
		bool bitSetFeature = false;

		if (fontIndex == -1)
		{
			AddOption(text, bitSetFeature, nullFunc, SUB::SETTINGS_FONTS2);
		}
		else
		{
			AddOption(text, bitChangeFont);
		}

		if (bitSetFeature)
		{
			settingsFont = &feature;
		}
		else if (bitChangeFont)
		{
			*settingsFont = fontIndex;
		}
	}
	void SettingsFonts()
	{
		AddTitle("Menu Fonts");
		AddsettingsfonOption("Title", -1, font_title);
		AddsettingsfonOption("Options", -1, font_options);
		AddsettingsfonOption("Selected Option", -1, font_selection);
		AddsettingsfonOption("Option Breaks", -1, font_breaks);
		AddsettingsfonOption("HUD Font", -1, font_hud);
		AddsettingsfonOption("Speedo Text", -1, font_speedo);
	}
	
	void SettingsFonts2()
	{
		auto& settingsFont = g_settingsFont;
		bool fonts2Input = false;
		INT8 dummyFeature;

		AddTitle("Set Font");
		AddsettingsfonOption("Normalish", GTAfont::Arial, dummyFeature);
		AddsettingsfonOption("Impactish", GTAfont::Impact, dummyFeature);
		AddsettingsfonOption("Italic", GTAfont::Italic, dummyFeature);
		AddsettingsfonOption("Pricedown", GTAfont::Pricedown, dummyFeature);
		AddsettingsfonOption("Caps", GTAfont::Caps, dummyFeature);
		AddOption("Input Index", fonts2Input);

		if (fonts2Input)
		{
			std::string inputStr = Game::InputBox(std::to_string(*settingsFont), 7U, "", std::to_string(*settingsFont));
			auto& settings_font = g_settingsFont;
			int tempHash = *settings_font;
			try 
			{
				tempHash = abs(stoi(inputStr)); 
			}
			catch (...) 
			{
				Game::Print::PrintErrorInvalidInput(inputStr); 
			}
			*settings_font = tempHash;
		}
	}

	void SettingsLanguage()
	{
		AddTitle("Language");
		AddTickol("English", Language::selectedLang == nullptr, Language::ResetSelectedLang, Language::ResetSelectedLang);

		for (auto& l : Language::allLangs)
		{
			bool bPressed = false;
			const std::string displayName = (l.GetName() == "Russian") ? "Russian" : l.GetName();
			AddTickol(displayName, Language::selectedLang == &l, bPressed, bPressed); if (bPressed)
			{
				Language::SetSelectedLang(&l);
			}
		}

		AddTickol("Reload Language Files", true, *reinterpret_cast<void(*)()>(&Language::Init), *reinterpret_cast<void(*)()>(&Language::Init), TICKOL::CROSS);
	}
	class BuiltinTheme
	{
	private:
		bool grads;
		bool rainbow;
		bool thinLineOverFooter;

		RGBA ttbox;
		RGBA bgbox;
		RGBA tttext;
		RGBA optext;
		RGBA seltext;
		RGBA opbreak;
		RGBA opcount;
		RGBA selhi;
		RGBA pedtrackers;

		INT8 f_title;
		INT8 f_options;
		INT8 f_selection;
		INT8 f_breaks;
		INT8 f_hud;
		INT8 f_speedo;
	public:
		BuiltinTheme()
		{
		}
		BuiltinTheme(bool _grads, bool _rainbow, bool _thinLineOverFooter,
			RGBA _ttbox, RGBA _bgbox, RGBA _tttext, RGBA _optext, RGBA _seltext, RGBA _opbreak, RGBA _opcount, RGBA _selhi, RGBA _pedtrackers,
			INT8 _f_title, INT8 _f_options, INT8 _f_selection, INT8 _f_breaks, INT8 _f_hud, INT8 _f_speedo)
		{
			grads = _grads;
			rainbow = _rainbow;
			thinLineOverFooter = _thinLineOverFooter;

			ttbox = _ttbox;
			bgbox = _bgbox;
			tttext = _tttext;
			optext = _optext;
			seltext = _seltext;
			opbreak = _opbreak;
			opcount = _opcount;
			selhi = _selhi;
			pedtrackers = _pedtrackers;

			f_title = _f_title;
			f_options = _f_options;
			f_selection = _f_selection;
			f_breaks = _f_breaks;
			f_hud = _f_hud;
			f_speedo = _f_speedo;
		}

		void SetActive()
		{
			Menu::gradients = grads;
			rainbowBoxes = rainbow;
			Menu::thinLineOverScrect = thinLineOverFooter;

			titlebox = ttbox;
			BG = bgbox;
			titletext = tttext;
			optiontext = optext;
			selectedtext = seltext;
			optionbreaks = opbreak;
			optioncount = opcount;
			selectionhi = selhi;
			_globalPedTrackers_Col = pedtrackers;

			font_title = f_title;
			font_options = f_options;
			font_selection = f_selection;
			font_breaks = f_breaks;
			font_hud = f_hud;
			font_speedo = f_speedo;
		}
		bool IsActive()
		{
			return
				Menu::gradients == grads &&
				rainbowBoxes == rainbow &&
				Menu::thinLineOverScrect == thinLineOverFooter &&

				titlebox == ttbox &&
				BG == bgbox &&
				titletext == tttext &&
				optiontext == optext &&
				selectedtext == seltext &&
				optionbreaks == opbreak &&
				optioncount == opcount &&
				selectionhi == selhi &&
				_globalPedTrackers_Col == pedtrackers &&

				font_title == f_title &&
				font_options == f_options &&
				font_selection == f_selection &&
				font_breaks == f_breaks &&
				font_hud == f_hud &&
				font_speedo == f_speedo;
		}

		static BuiltinTheme CurrentlyActiveTheme()
		{
			BuiltinTheme curr;

			curr.grads = Menu::gradients;
			curr.rainbow = rainbowBoxes;
			curr.thinLineOverFooter = Menu::thinLineOverScrect;

			curr.ttbox = titlebox;
			curr.bgbox = BG;
			curr.tttext = titletext;
			curr.optext = optiontext;
			curr.seltext = selectedtext;
			curr.opbreak = optionbreaks;
			curr.opcount = optioncount;
			curr.selhi = selectionhi;
			curr.pedtrackers = _globalPedTrackers_Col;

			curr.f_title = font_title;
			curr.f_options = font_options;
			curr.f_selection = font_selection;
			curr.f_breaks = font_breaks;
			curr.f_hud = font_hud;
			curr.f_speedo = font_speedo;

			return curr;
		}

		bool operator ==(const BuiltinTheme& value2) const
		{
			const BuiltinTheme& value1 = *this;

			return
				value1.grads == value2.grads &&
				value1.rainbow == value2.rainbow &&
				value1.thinLineOverFooter == value2.thinLineOverFooter &&

				value1.ttbox == value2.ttbox &&
				value1.bgbox == value2.bgbox &&
				value1.tttext == value2.tttext &&
				value1.optext == value2.optext &&
				value1.seltext == value2.seltext &&
				value1.opbreak == value2.opbreak &&
				value1.opcount == value2.opcount &&
				value1.selhi == value2.selhi &&
				value1.pedtrackers == value2.pedtrackers &&

				value1.f_title == value2.f_title &&
				value1.f_options == value2.f_options &&
				value1.f_selection == value2.f_selection &&
				value1.f_breaks == value2.f_breaks &&
				value1.f_hud == value2.f_hud &&
				value1.f_speedo == value2.f_speedo;
		}
		bool Equals(BuiltinTheme const& value2)
		{
			return (this->operator==(value2));
		}

	};
	class BuiltinThemeNamed
	{
	public:
		std::string name;
		BuiltinTheme theme;

		BuiltinThemeNamed(const std::string& newName, const BuiltinTheme& newTheme)
		{
			this->name = newName;
			this->theme = newTheme;
		}
	};

	std::vector<BuiltinThemeNamed> vValues_BuiltinThemes
	{
		{ "Modern Dark", BuiltinTheme(true, false, true, RGBA(20, 184, 166, 245), RGBA(12, 14, 18, 230), RGBA(255, 255, 255, 255), RGBA(220, 225, 230, 255), RGBA(12, 14, 18, 255), RGBA(20, 184, 166, 240), RGBA(160, 170, 180, 255), RGBA(236, 242, 248, 230), RGBA(20, 184, 166, 205), GTAfont::Arial, GTAfont::Arial, GTAfont::Arial, GTAfont::Arial, GTAfont::Arial, GTAfont::Arial) },
		{ "Modern Teal", BuiltinTheme(true, false, true, RGBA(15, 118, 110, 245), RGBA(8, 12, 16, 235), RGBA(255, 255, 255, 255), RGBA(210, 230, 228, 255), RGBA(8, 20, 18, 255), RGBA(45, 212, 191, 240), RGBA(148, 163, 184, 255), RGBA(45, 212, 191, 200), RGBA(45, 212, 191, 205), GTAfont::Arial, GTAfont::Arial, GTAfont::Arial, GTAfont::Arial, GTAfont::Arial, GTAfont::Arial) },
		{ "Black & Green", BuiltinTheme(true, false, false, RGBA(0, 0, 0, 255), RGBA(0, 0, 0, 100), RGBA(0, 255, 100, 255), RGBA(0, 255, 100, 255), RGBA(255, 255, 255, 255), RGBA(0, 255, 100, 255), RGBA(0, 255, 100, 255), RGBA(0, 255, 100, 255), RGBA(0, 255, 100, 205), GTAfont::Italic, GTAfont::Impact, GTAfont::Impact, GTAfont::Italic, GTAfont::Arial, GTAfont::Pricedown) },
		{ "Black & Teal", BuiltinTheme(true, false, false, RGBA(0, 0, 0, 255), RGBA(0, 0, 0, 150), RGBA(0, 255, 255, 255), RGBA(0, 255, 255, 255), RGBA(255, 255, 255, 255), RGBA(0, 255, 255, 255), RGBA(0, 255, 255, 255), RGBA(0, 255, 255, 150), RGBA(0, 255, 255, 205), GTAfont::Italic, GTAfont::Impact, GTAfont::Impact, GTAfont::Italic, GTAfont::Arial, GTAfont::Pricedown) },
		{ "Black & Red", BuiltinTheme(true, false, false, RGBA(0, 0, 0, 255), RGBA(0, 0, 0, 175), RGBA(255, 0, 0, 255), RGBA(255, 0, 0, 255), RGBA(255, 255, 255, 255), RGBA(255, 0, 0, 255), RGBA(255, 0, 0, 255), RGBA(255, 0, 0, 100), RGBA(255, 0, 0, 205), GTAfont::Italic, GTAfont::Impact, GTAfont::Impact, GTAfont::Italic, GTAfont::Arial, GTAfont::Pricedown) },
		{ "Black & White", BuiltinTheme(true, false, false, RGBA(0, 0, 0, 255), RGBA(0, 0, 0, 50), RGBA(255, 255, 255, 255), RGBA(255, 255, 255, 255), RGBA(255, 255, 255, 255), RGBA(255, 255, 255, 255), RGBA(255, 255, 255, 255), RGBA(255, 255, 255, 150), RGBA(255, 255, 255, 205), GTAfont::Italic, GTAfont::Impact, GTAfont::Impact, GTAfont::Italic, GTAfont::Arial, GTAfont::Pricedown) },
		{ "Black & Yellow", BuiltinTheme(true, false, false, RGBA(0, 0, 0, 255), RGBA(0, 0, 0, 100), RGBA(255, 255, 100, 255), RGBA(255, 255, 100, 255), RGBA(255, 255, 255, 255), RGBA(255, 255, 100, 255), RGBA(255, 255, 100, 255), RGBA(255, 255, 100, 125), RGBA(255, 255, 100, 205), GTAfont::Italic, GTAfont::Impact, GTAfont::Impact, GTAfont::Italic, GTAfont::Arial, GTAfont::Pricedown) },
		{ "White & Green", BuiltinTheme(true, false, false, RGBA(255, 255, 255, 255), RGBA(0, 0, 0, 100), RGBA(0, 255, 100, 255), RGBA(0, 255, 100, 255), RGBA(255, 255, 255, 255), RGBA(0, 255, 100, 255), RGBA(0, 255, 100, 255), RGBA(0, 255, 125, 150), RGBA(0, 255, 100, 255), GTAfont::Italic, GTAfont::Impact, GTAfont::Impact, GTAfont::Italic, GTAfont::Arial, GTAfont::Pricedown) },
		{ "White & Teal", BuiltinTheme(true, false, false, RGBA(255, 255, 255, 255), RGBA(0, 0, 0, 100), RGBA(0, 255, 255, 255), RGBA(0, 255, 255, 255), RGBA(255, 255, 255, 255), RGBA(0, 255, 255, 255), RGBA(0, 255, 255, 255), RGBA(0, 255, 255, 150), RGBA(0, 255, 255, 205), GTAfont::Italic, GTAfont::Impact, GTAfont::Impact, GTAfont::Italic, GTAfont::Arial, GTAfont::Pricedown) },
		{ "White & Red", BuiltinTheme(true, false, false, RGBA(255, 255, 255, 255), RGBA(0, 0, 0, 175), RGBA(255, 0, 0, 255), RGBA(255, 255, 255, 255), RGBA(255, 255, 255, 255), RGBA(255, 0, 0, 255), RGBA(255, 0, 0, 255), RGBA(255, 0, 0, 150), RGBA(255, 0, 0, 205), GTAfont::Italic, GTAfont::Impact, GTAfont::Impact, GTAfont::Italic, GTAfont::Arial, GTAfont::Pricedown) },
		{ "White & Black", BuiltinTheme(true, false, false, RGBA(255, 255, 255, 255), RGBA(0, 0, 0, 150), RGBA(0, 0, 0, 255), RGBA(255, 255, 255, 255), RGBA(0, 0, 0, 255), RGBA(0, 0, 0, 255), RGBA(0, 0, 0, 255), RGBA(255, 255, 255, 150), RGBA(255, 255, 255, 205), GTAfont::Italic, GTAfont::Impact, GTAfont::Impact, GTAfont::Italic, GTAfont::Arial, GTAfont::Pricedown) },
		{ "White & Yellow", BuiltinTheme(true, false, false, RGBA(255, 255, 255, 255), RGBA(0, 0, 0, 100), RGBA(255, 255, 100, 255), RGBA(255, 255, 100, 255), RGBA(255, 255, 100, 255), RGBA(255, 255, 100, 255), RGBA(255, 255, 100, 255), RGBA(255, 255, 100, 125), RGBA(255, 255, 100, 205), GTAfont::Italic, GTAfont::Impact, GTAfont::Impact, GTAfont::Italic, GTAfont::Arial, GTAfont::Pricedown) },
		{ "Halloween", BuiltinTheme(true, false, false, RGBA(22, 161, 18, 255), RGBA(96, 62, 148, 170), RGBA(255, 51, 0, 255), RGBA(255, 255, 255, 255), RGBA(0, 0, 0, 255), RGBA(255, 255, 255, 255), RGBA(255, 51, 0, 255), RGBA(255, 51, 0, 150), RGBA(22, 161, 18, 205), GTAfont::Pricedown, GTAfont::Impact, GTAfont::Impact, GTAfont::Italic, GTAfont::Arial, GTAfont::Pricedown) },
		{ "Elegant Purple", BuiltinTheme(true, false, true, RGBA(102, 0, 204, 255), RGBA(10, 10, 10, 255), RGBA(255, 255, 255, 255), RGBA(255, 255, 255, 255), RGBA(0, 0, 0, 255), RGBA(255, 255, 255, 240), RGBA(255, 255, 255, 255), RGBA(255, 255, 255, 211), RGBA(153, 51, 255, 205), GTAfont::Pricedown, GTAfont::Impact, GTAfont::Impact, GTAfont::Italic, GTAfont::Arial, GTAfont::Pricedown) },
	};

	void SettingsThemesMain()
	{
		AddTitle("Themes");

		AddOption("Custom Themes (Shareable)", null, nullFunc, SUB::CUSTOM_THEMES);
		AddOption("SPACE UI Settings", null, nullFunc, SUB::SPACE_UISETTINGS);

		AddBreak("---Built-in---");
		bool pressed = false;
		for (auto& them : vValues_BuiltinThemes)
		{
			pressed = false;
			BOOL isActive = (them.theme == BuiltinTheme::CurrentlyActiveTheme()) ? TRUE : FALSE;
			AddTickol(them.name, isActive, pressed, pressed, TICKOL::MAKEUPTHING, TICKOL::NONE, false);
			if (pressed)
			{
				them.theme.SetActive();
			}
		}
	}
}


#include "..\Menu\submenu_switch.h"
#include "..\Menu\submenu_enum.h"
REGISTER_SUBMENU(SETTINGS,            sub::Settings)
REGISTER_SUBMENU(SETTINGS_MENUPOS,    sub::SettingsMenuPos)
REGISTER_SUBMENU(SETTINGS_THEMES,     sub::SettingsThemesMain)
REGISTER_SUBMENU(SETTINGS_COLOURS,    sub::SettingsColours)
REGISTER_SUBMENU(SETTINGS_COLOURS2,   sub::SettingsColours2)
REGISTER_SUBMENU(SETTINGS_FONTS,      sub::SettingsFonts)
REGISTER_SUBMENU(SETTINGS_FONTS2,     sub::SettingsFonts2)
REGISTER_SUBMENU(SETTINGS_LANGUAGE,   sub::SettingsLanguage)



