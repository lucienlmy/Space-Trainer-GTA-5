/* Space UI */
#pragma once

#include "..\Natives\types.h"
#include "..\Util\GTAmath.h"
#include <string>

namespace SpaceTheme
{
	enum class AccentPreset : int
	{
		Blue = 0,
		Purple,
		Red,
		Green,
		White,
		Cyan
	};

	enum class AnimQuality : int
	{
		Off = 0,
		Low,
		Medium,
		High
	};

	struct Layout
	{
		float panelCenterX;
		float panelWidth;
		float panelLeft;
		float panelRight;
		float headerTopY;
		float headerHeight;
		float titleBarY;
		float titleBarH;
		float optionStartY;
		float optionRowH;
		float footerPad;
		float scale;
	};

	extern AccentPreset accentPreset;
	extern float uiOpacity;
	extern float uiScale;
	extern float uiBlur;
	extern float textScale;
	extern AnimQuality animQuality;
	extern bool spaceUiEnabled;
	extern bool livePulse;
	extern bool showTips;
	extern bool showRowDividers;
	extern std::string currentSubtitle;

	RGBA Accent();
	RGBA PanelBg();
	RGBA TitleBarBg();
	RGBA SelectedBg();
	RGBA TextPrimary();
	RGBA TextSecondary();
	RGBA Divider();

	void BeginFrame();
	const Layout& GetLayout();
	void ApplyDefaultsToLegacyColors();
	void TickOpenAnimation();
	void NotifySubChanged();
	float OpenAlpha();
	float OpenSlideY();
	float OpenScale();
	float Pulse();
	float SelectionEase();
	float SubtitleAlpha();

	void DrawRoundedPanelApprox(float cx, float cy, float w, float h, const RGBA& col);
	void DrawHeaderShell(const Layout& L);
	void DrawHeaderTitle(const Layout& L, const std::string& subTitle);
	void DrawListChrome(const Layout& L, int visibleRows);
	void DrawFooter(const Layout& L, int current, int total, const std::string& description);
	void DrawSelection(const Layout& L, float optionY);

	const char* AccentName(AccentPreset p);
	void CycleAccent(int dir);
	void SetNamedTheme(AccentPreset p);
}
