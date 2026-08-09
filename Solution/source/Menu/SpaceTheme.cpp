/* Space UI */
#include "SpaceTheme.h"
#include "Menu.h"
#include "..\macros.h"
#include "..\Natives\natives2.h"
#include "..\Scripting\Game.h"

#include <Windows.h>
#include <algorithm>
#include <cmath>

namespace SpaceTheme
{
	AccentPreset accentPreset = AccentPreset::Cyan;
	float uiOpacity = 0.78f;
	float uiScale = 1.0f;
	float uiBlur = 0.22f;
	float textScale = 1.0f;
	AnimQuality animQuality = AnimQuality::Medium;
	bool spaceUiEnabled = true;
	bool livePulse = true;
	bool showTips = true;
	bool showRowDividers = true;
	std::string currentSubtitle = "MAIN";

	static DWORD s_openTick = 0;
	static DWORD s_subTick = 0;
	static DWORD s_selTick = 0;
	static int s_lastSel = -1;
	static bool s_wasOpen = false;
	static Layout s_layout{};
	static bool s_layoutReady = false;

	static int ClampByte(int v) { return v < 0 ? 0 : (v > 255 ? 255 : v); }
	static RGBA WithAlpha(const RGBA& c, int a) { return RGBA(c.R, c.G, c.B, ClampByte(a)); }

	RGBA Accent()
	{
		switch (accentPreset)
		{
		case AccentPreset::Purple: return RGBA(168, 85, 247, 255);
		case AccentPreset::Red: return RGBA(239, 68, 68, 255);
		case AccentPreset::Green: return RGBA(34, 197, 94, 255);
		case AccentPreset::White: return RGBA(241, 245, 249, 255);
		case AccentPreset::Blue: return RGBA(59, 130, 246, 255);
		default: return RGBA(56, 189, 248, 255); // SPACE CYAN default
		}
	}

	RGBA PanelBg()
	{
		return RGBA(6, 8, 12, ClampByte((int)(uiOpacity * 255.0f)));
	}

	RGBA TitleBarBg()
	{
		return RGBA(18, 22, 28, ClampByte((int)((uiOpacity + 0.08f) * 255.0f)));
	}

	RGBA SelectedBg()
	{
		const auto a = Accent();
		return RGBA(
			(int)(a.R * 0.22f + 24),
			(int)(a.G * 0.24f + 28),
			(int)(a.B * 0.28f + 36),
			ClampByte((int)(0.55f * uiOpacity * 255.0f)));
	}

	RGBA TextPrimary() { return RGBA(255, 255, 255, 255); }
	RGBA TextSecondary() { return RGBA(170, 178, 190, 255); }
	RGBA Divider() { return RGBA(255, 255, 255, 16); }

	void BeginFrame() { s_layoutReady = false; }

	void NotifySubChanged()
	{
		s_subTick = GetTickCount();
	}

	const Layout& GetLayout()
	{
		if (s_layoutReady)
			return s_layout;

		Layout& L = s_layout;
		L.scale = uiScale;
		L.panelWidth = 0.210f * uiScale;

		// Left-anchored panel. Clamp so a bad menuPosX (from old configs / position slider)
		// cannot push the menu off the right edge of the screen.
		float offsetX = menuPos.x;
		if (offsetX < -0.02f) offsetX = -0.02f;
		if (offsetX > 0.10f) offsetX = 0.10f;

		float offsetY = menuPos.y;
		if (offsetY < -0.04f) offsetY = -0.04f;
		if (offsetY > 0.28f) offsetY = 0.28f;

		L.panelLeft = 0.028f + offsetX;
		if (L.panelLeft < 0.012f) L.panelLeft = 0.012f;
		if (L.panelLeft + L.panelWidth > 0.988f)
			L.panelLeft = 0.988f - L.panelWidth;
		L.panelRight = L.panelLeft + L.panelWidth;
		L.panelCenterX = L.panelLeft + L.panelWidth * 0.5f;

		L.headerTopY = 0.070f + offsetY + OpenSlideY();
		L.headerHeight = 0.048f * uiScale;
		L.titleBarY = L.headerTopY + L.headerHeight;
		L.titleBarH = 0.020f * uiScale;
		L.optionStartY = L.titleBarY + L.titleBarH + 0.003f;
		L.optionRowH = 0.0265f * uiScale;
		L.footerPad = 0.016f;
		s_layoutReady = true;
		return s_layout;
	}

	void ApplyDefaultsToLegacyColors()
	{
		const auto a = Accent();
		titlebox = a;
		selectionhi = SelectedBg();
		BG = PanelBg();
		titletext = TextPrimary();
		optiontext = TextPrimary();
		selectedtext = TextPrimary();
		optionbreaks = TextSecondary();
		optioncount = TextSecondary();
		_globalPedTrackers_Col = WithAlpha(a, 205);
	}

	void TickOpenAnimation()
	{
		BeginFrame();
		const bool open = Menu::currentsub != SUB::CLOSED;
		if (open && !s_wasOpen)
		{
			s_openTick = GetTickCount();
			s_subTick = s_openTick;
		}
		s_wasOpen = open;

		if (open && Menu::currentop != s_lastSel)
		{
			s_selTick = GetTickCount();
			s_lastSel = Menu::currentop;
		}
	}

	float OpenAlpha()
	{
		if (animQuality == AnimQuality::Off || Menu::currentsub == SUB::CLOSED)
			return 1.0f;
		const DWORD dur = (animQuality == AnimQuality::High) ? 200u : (animQuality == AnimQuality::Low ? 90u : 150u);
		const DWORD dt = GetTickCount() - s_openTick;
		if (dt >= dur) return 1.0f;
		const float t = (float)dt / (float)dur;
		return t * t * (3.0f - 2.0f * t);
	}

	float OpenSlideY()
	{
		if (animQuality == AnimQuality::Off)
			return 0.0f;
		const float t = OpenAlpha();
		return (1.0f - t) * (-0.018f);
	}

	float Pulse()
	{
		if (!livePulse || animQuality == AnimQuality::Off)
			return 1.0f;
		const float w = (std::sin((float)GetTickCount() * 0.0036f) + 1.0f) * 0.5f;
		return 0.88f + w * 0.12f;
	}

	float SelectionEase()
	{
		if (animQuality == AnimQuality::Off)
			return 1.0f;
		const DWORD dur = 120u;
		const DWORD dt = GetTickCount() - s_selTick;
		if (dt >= dur) return 1.0f;
		const float t = (float)dt / (float)dur;
		return 0.72f + 0.28f * (t * t * (3.0f - 2.0f * t));
	}

	void DrawRoundedPanelApprox(float cx, float cy, float w, float h, const RGBA& col)
	{
		DRAW_RECT(cx, cy, w, h, col.R, col.G, col.B, col.A, false);
	}

	void DrawHeaderShell(const Layout& L)
	{
		const float alpha = OpenAlpha();
		if (uiBlur > 0.01f)
		{
			const int ba = ClampByte((int)(uiBlur * 70.0f * alpha));
			DRAW_RECT(L.panelCenterX, 0.5f, L.panelWidth + 0.06f, 1.1f, 4, 6, 10, ba, false);
		}

		auto panel = WithAlpha(PanelBg(), (int)(PanelBg().A * alpha));
		DrawRoundedPanelApprox(L.panelCenterX, L.headerTopY + L.headerHeight * 0.5f, L.panelWidth, L.headerHeight, panel);

		auto bar = WithAlpha(TitleBarBg(), (int)(TitleBarBg().A * alpha));
		DRAW_RECT(L.panelCenterX, L.titleBarY + L.titleBarH * 0.5f, L.panelWidth, L.titleBarH, bar.R, bar.G, bar.B, bar.A, false);
	}

	void DrawHeaderTitle(const Layout& L, const std::string& subTitle)
	{
		const float alpha = OpenAlpha();
		const auto text = WithAlpha(TextPrimary(), (int)(255 * alpha));
		const auto muted = WithAlpha(TextSecondary(), (int)(235 * alpha));

		Game::Print::SetupDraw(0, Vector2(0.48f * L.scale * textScale, 0.48f * L.scale * textScale), true, false, false, text);
		Game::Print::drawstring("SPACE", L.panelCenterX, L.headerTopY + 0.006f);

		const std::string& sub = subTitle.empty() ? currentSubtitle : subTitle;
		Game::Print::SetupDraw(0, Vector2(0.20f * L.scale * textScale, 0.20f * L.scale * textScale), true, false, false, muted);
		Game::Print::drawstring(sub, L.panelCenterX, L.titleBarY + 0.0015f);
	}

	void DrawListChrome(const Layout& L, int visibleRows)
	{
		const float alpha = OpenAlpha();
		auto panel = WithAlpha(PanelBg(), (int)(PanelBg().A * alpha));
		const float listH = (float)visibleRows * L.optionRowH + 0.008f;
		const float listCy = L.optionStartY + listH * 0.5f;
		DrawRoundedPanelApprox(L.panelCenterX, listCy, L.panelWidth, listH, panel);

		if (showRowDividers && visibleRows > 1)
		{
			const auto div = WithAlpha(Divider(), (int)(Divider().A * alpha * 1.4f));
			for (int i = 1; i < visibleRows; ++i)
			{
				const float y = L.optionStartY + i * L.optionRowH;
				DRAW_RECT(L.panelCenterX, y, L.panelWidth * 0.92f, 0.0009f, div.R, div.G, div.B, div.A, false);
			}
		}
	}

	void DrawFooter(const Layout& L, int current, int total, const std::string& description)
	{
		const float alpha = OpenAlpha();
		const float pulse = Pulse();
		const auto muted = WithAlpha(TextSecondary(), (int)(220 * alpha));
		const auto text = WithAlpha(TextPrimary(), (int)(245 * alpha));
		const auto accent = WithAlpha(Accent(), (int)(255 * alpha * pulse));
		const auto bar = WithAlpha(TitleBarBg(), (int)(TitleBarBg().A * alpha));
		const auto panel = WithAlpha(PanelBg(), (int)(PanelBg().A * alpha));

		const int rows = (total > GTA_MAXOP) ? GTA_MAXOP : (total < 1 ? 1 : total);
		const float listBottom = L.optionStartY + (float)rows * L.optionRowH;
		const float footerTop = listBottom + 0.006f;

		// Counter strip
		DRAW_RECT(L.panelCenterX, footerTop + 0.014f, L.panelWidth, 0.028f, panel.R, panel.G, panel.B, panel.A, false);

		std::string counter = std::to_string(current) + " / " + std::to_string(total);
		Game::Print::SetupDraw(0, Vector2(0.22f * L.scale * textScale, 0.22f * L.scale * textScale), true, false, false, muted);
		Game::Print::drawstring(counter, L.panelCenterX, footerTop + 0.004f);

		// Accent separator
		DRAW_RECT(L.panelCenterX, footerTop + 0.030f, L.panelWidth * 0.94f, 0.0016f,
			accent.R, accent.G, accent.B, (int)(230 * alpha), false);

		// Description box
		DRAW_RECT(L.panelCenterX, footerTop + 0.052f, L.panelWidth, 0.034f, bar.R, bar.G, bar.B, bar.A, false);

		if (!showTips)
			return;

		std::string tip = description.empty() ? "SPACE Trainer" : description;
		if (tip.size() > 48) tip = tip.substr(0, 45) + "...";
		Game::Print::SetupDraw(0, Vector2(0.20f * L.scale * textScale, 0.20f * L.scale * textScale), false, false, false, text);
		Game::Print::drawstring(tip, L.panelLeft + 0.012f, footerTop + 0.042f);
	}

	void DrawSelection(const Layout& L, float optionY)
	{
		const float alpha = OpenAlpha() * SelectionEase();
		const auto sel = WithAlpha(SelectedBg(), (int)(SelectedBg().A * alpha));
		const float rowH = L.optionRowH * 0.92f;
		const float y = optionY + menuPos.y + L.optionRowH * 0.42f;
		DRAW_RECT(L.panelCenterX, y, L.panelWidth * 0.985f, rowH, sel.R, sel.G, sel.B, sel.A, false);
	}

	const char* AccentName(AccentPreset p)
	{
		switch (p)
		{
		case AccentPreset::Purple: return "SPACE PURPLE";
		case AccentPreset::Red: return "SPACE RED";
		case AccentPreset::Green: return "SPACE GREEN";
		case AccentPreset::White: return "SPACE WHITE";
		case AccentPreset::Blue: return "SPACE BLUE";
		default: return "SPACE CYAN";
		}
	}

	void SetNamedTheme(AccentPreset p)
	{
		accentPreset = p;
		ApplyDefaultsToLegacyColors();
	}

	void CycleAccent(int dir)
	{
		int v = (int)accentPreset + dir;
		if (v < 0) v = 5;
		if (v > 5) v = 0;
		SetNamedTheme((AccentPreset)v);
	}
}
