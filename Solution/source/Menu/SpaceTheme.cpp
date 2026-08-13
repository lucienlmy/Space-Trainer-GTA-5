/* Space UI — smooth SPACE TRAINER shell */
#include "SpaceTheme.h"
#include "Menu.h"
#include "..\macros.h"
#include "..\Natives\natives2.h"
#include "..\Scripting\Game.h"

#include <Windows.h>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <string>

namespace SpaceTheme
{
	AccentPreset accentPreset = AccentPreset::Cyan;
	float uiOpacity = 0.82f;
	float uiScale = 1.0f;
	float uiBlur = 0.28f;
	float textScale = 1.0f;
	AnimQuality animQuality = AnimQuality::High;
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
	static float s_selSmoothY = -1.0f;
	static float s_selSmoothW = 0.96f;

	static int ClampByte(int v) { return v < 0 ? 0 : (v > 255 ? 255 : v); }
	static RGBA WithAlpha(const RGBA& c, int a) { return RGBA(c.R, c.G, c.B, ClampByte(a)); }

	static float Smoothstep(float t)
	{
		t = t < 0.0f ? 0.0f : (t > 1.0f ? 1.0f : t);
		return t * t * (3.0f - 2.0f * t);
	}

	static float EaseOutCubic(float t)
	{
		t = t < 0.0f ? 0.0f : (t > 1.0f ? 1.0f : t);
		const float u = 1.0f - t;
		return 1.0f - u * u * u;
	}

	RGBA Accent()
	{
		switch (accentPreset)
		{
		case AccentPreset::Purple: return RGBA(168, 85, 247, 255);
		case AccentPreset::Red: return RGBA(239, 68, 68, 255);
		case AccentPreset::Green: return RGBA(34, 197, 94, 255);
		case AccentPreset::White: return RGBA(241, 245, 249, 255);
		case AccentPreset::Blue: return RGBA(59, 130, 246, 255);
		default: return RGBA(56, 189, 248, 255);
		}
	}

	RGBA PanelBg()
	{
		return RGBA(5, 7, 12, ClampByte((int)(uiOpacity * 255.0f)));
	}

	RGBA TitleBarBg()
	{
		return RGBA(14, 18, 26, ClampByte((int)((uiOpacity + 0.10f) * 255.0f)));
	}

	RGBA SelectedBg()
	{
		const auto a = Accent();
		return RGBA(
			(int)(a.R * 0.28f + 18),
			(int)(a.G * 0.30f + 22),
			(int)(a.B * 0.34f + 30),
			ClampByte((int)(0.62f * uiOpacity * 255.0f)));
	}

	RGBA TextPrimary() { return RGBA(255, 255, 255, 255); }
	RGBA TextSecondary() { return RGBA(168, 176, 190, 255); }
	RGBA Divider() { return RGBA(255, 255, 255, 18); }

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
		L.scale = uiScale * OpenScale();
		L.panelWidth = 0.248f * uiScale;

		float offsetX = menuPos.x;
		if (offsetX < -0.02f) offsetX = -0.02f;
		if (offsetX > 0.10f) offsetX = 0.10f;

		float offsetY = menuPos.y;
		if (offsetY < -0.04f) offsetY = -0.04f;
		if (offsetY > 0.28f) offsetY = 0.28f;

		L.panelLeft = 0.026f + offsetX;
		if (L.panelLeft < 0.010f) L.panelLeft = 0.010f;
		if (L.panelLeft + L.panelWidth > 0.988f)
			L.panelLeft = 0.988f - L.panelWidth;
		L.panelRight = L.panelLeft + L.panelWidth;
		L.panelCenterX = L.panelLeft + L.panelWidth * 0.5f;

		L.headerTopY = 0.066f + offsetY + OpenSlideY();
		L.headerHeight = 0.052f * uiScale;
		L.titleBarY = L.headerTopY + L.headerHeight;
		L.titleBarH = 0.022f * uiScale;
		L.optionStartY = L.titleBarY + L.titleBarH + 0.004f;
		L.optionRowH = 0.0272f * uiScale;
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
			s_selSmoothY = -1.0f;
		}
		if (!open)
			s_selSmoothY = -1.0f;
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
		const DWORD dur = (animQuality == AnimQuality::High) ? 280u
			: (animQuality == AnimQuality::Medium ? 200u : 110u);
		const DWORD dt = GetTickCount() - s_openTick;
		if (dt >= dur) return 1.0f;
		return EaseOutCubic((float)dt / (float)dur);
	}

	float OpenSlideY()
	{
		if (animQuality == AnimQuality::Off)
			return 0.0f;
		const float t = OpenAlpha();
		return (1.0f - t) * (-0.028f);
	}

	float OpenScale()
	{
		if (animQuality == AnimQuality::Off || animQuality == AnimQuality::Low)
			return 1.0f;
		const float t = OpenAlpha();
		return 0.965f + 0.035f * t;
	}

	float Pulse()
	{
		if (!livePulse || animQuality == AnimQuality::Off)
			return 1.0f;
		const float speed = (animQuality == AnimQuality::High) ? 0.0028f : 0.0038f;
		const float w = (std::sin((float)GetTickCount() * speed) + 1.0f) * 0.5f;
		return 0.86f + w * 0.14f;
	}

	float SelectionEase()
	{
		if (animQuality == AnimQuality::Off)
			return 1.0f;
		const DWORD dur = (animQuality == AnimQuality::High) ? 180u : 120u;
		const DWORD dt = GetTickCount() - s_selTick;
		if (dt >= dur) return 1.0f;
		return 0.70f + 0.30f * Smoothstep((float)dt / (float)dur);
	}

	float SubtitleAlpha()
	{
		if (animQuality == AnimQuality::Off)
			return 1.0f;
		const DWORD dur = (animQuality == AnimQuality::High) ? 220u : 140u;
		const DWORD dt = GetTickCount() - s_subTick;
		if (dt >= dur) return 1.0f;
		return EaseOutCubic((float)dt / (float)dur);
	}

	void DrawRoundedPanelApprox(float cx, float cy, float w, float h, const RGBA& col)
	{
		DRAW_RECT(cx, cy, w, h, col.R, col.G, col.B, col.A, false);
		// Soft edge strips for a softer panel look
		const int edgeA = ClampByte(col.A / 3);
		DRAW_RECT(cx, cy - h * 0.5f, w, 0.0018f, col.R, col.G, col.B, edgeA, false);
		DRAW_RECT(cx, cy + h * 0.5f, w, 0.0018f, col.R, col.G, col.B, edgeA, false);
	}

	void DrawHeaderShell(const Layout& L)
	{
		const float alpha = OpenAlpha();
		const float pulse = Pulse();
		const auto accent = Accent();

		if (uiBlur > 0.01f)
		{
			const int ba = ClampByte((int)(uiBlur * 78.0f * alpha));
			DRAW_RECT(L.panelCenterX, 0.5f, L.panelWidth + 0.075f, 1.15f, 3, 5, 9, ba, false);
		}

		auto panel = WithAlpha(PanelBg(), (int)(PanelBg().A * alpha));
		DrawRoundedPanelApprox(L.panelCenterX, L.headerTopY + L.headerHeight * 0.5f, L.panelWidth, L.headerHeight, panel);

		// Accent glow line under brand
		DRAW_RECT(L.panelCenterX, L.headerTopY + L.headerHeight - 0.0012f, L.panelWidth * 0.92f, 0.0024f,
			accent.R, accent.G, accent.B, ClampByte((int)(210 * alpha * pulse)), false);

		auto bar = WithAlpha(TitleBarBg(), (int)(TitleBarBg().A * alpha));
		DRAW_RECT(L.panelCenterX, L.titleBarY + L.titleBarH * 0.5f, L.panelWidth, L.titleBarH, bar.R, bar.G, bar.B, bar.A, false);
	}

	void DrawHeaderTitle(const Layout& L, const std::string& subTitle)
	{
		const float alpha = OpenAlpha();
		const float subA = SubtitleAlpha();
		const auto text = WithAlpha(TextPrimary(), (int)(255 * alpha));
		const auto muted = WithAlpha(TextSecondary(), (int)(235 * alpha * subA));
		const auto accent = WithAlpha(Accent(), (int)(255 * alpha * Pulse()));

		Game::Print::SetupDraw(0, Vector2(0.33f * L.scale * textScale, 0.33f * L.scale * textScale), true, false, false, text);
		Game::Print::drawstring("SPACE TRAINER", L.panelCenterX, L.headerTopY + 0.0055f);

		char ver[32];
		sprintf_s(ver, "v%s", SPACE_CURRENT_VER_);
		Game::Print::SetupDraw(0, Vector2(0.17f * L.scale * textScale, 0.17f * L.scale * textScale), true, false, false, accent);
		Game::Print::drawstring(ver, L.panelCenterX, L.headerTopY + 0.028f);

		const std::string& sub = subTitle.empty() ? currentSubtitle : subTitle;
		Game::Print::SetupDraw(0, Vector2(0.20f * L.scale * textScale, 0.20f * L.scale * textScale), true, false, false, muted);
		Game::Print::drawstring(sub, L.panelCenterX, L.titleBarY + 0.0020f);
	}

	void DrawListChrome(const Layout& L, int visibleRows)
	{
		const float alpha = OpenAlpha();
		auto panel = WithAlpha(PanelBg(), (int)(PanelBg().A * alpha));
		const float listH = (float)visibleRows * L.optionRowH + 0.010f;
		const float listCy = L.optionStartY + listH * 0.5f;
		DrawRoundedPanelApprox(L.panelCenterX, listCy, L.panelWidth, listH, panel);

		// Soft left accent rail
		const auto accent = Accent();
		DRAW_RECT(L.panelLeft + 0.0022f, listCy, 0.0020f, listH * 0.92f,
			accent.R, accent.G, accent.B, ClampByte((int)(120 * alpha * Pulse())), false);

		if (showRowDividers && visibleRows > 1)
		{
			const auto div = WithAlpha(Divider(), (int)(Divider().A * alpha * 1.5f));
			for (int i = 1; i < visibleRows; ++i)
			{
				const float y = L.optionStartY + i * L.optionRowH;
				DRAW_RECT(L.panelCenterX, y, L.panelWidth * 0.90f, 0.00085f, div.R, div.G, div.B, div.A, false);
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
		const float footerTop = listBottom + 0.007f;

		DRAW_RECT(L.panelCenterX, footerTop + 0.014f, L.panelWidth, 0.028f, panel.R, panel.G, panel.B, panel.A, false);

		std::string counter = std::to_string(current) + " / " + std::to_string(total);
		Game::Print::SetupDraw(0, Vector2(0.22f * L.scale * textScale, 0.22f * L.scale * textScale), true, false, false, muted);
		Game::Print::drawstring(counter, L.panelCenterX, footerTop + 0.004f);

		DRAW_RECT(L.panelCenterX, footerTop + 0.030f, L.panelWidth * 0.94f, 0.0018f,
			accent.R, accent.G, accent.B, (int)(235 * alpha), false);

		DRAW_RECT(L.panelCenterX, footerTop + 0.052f, L.panelWidth, 0.036f, bar.R, bar.G, bar.B, bar.A, false);

		if (!showTips)
			return;

		std::string tip = description.empty() ? "SPACE TRAINER" : description;
		// UTF-8 safe short tip
		{
			const size_t maxCp = 46u;
			size_t i = 0, cp = 0;
			while (i < tip.size() && cp < maxCp)
			{
				const unsigned char c = (unsigned char)tip[i];
				if ((c & 0x80) == 0) i += 1;
				else if ((c & 0xE0) == 0xC0) i += 2;
				else if ((c & 0xF0) == 0xE0) i += 3;
				else if ((c & 0xF8) == 0xF0) i += 4;
				else i += 1;
				++cp;
			}
			if (i < tip.size())
			{
				while (i > 0 && ((unsigned char)tip[i] & 0xC0) == 0x80) --i;
				tip = tip.substr(0, i) + "..";
			}
		}
		Game::Print::SetupDraw(0, Vector2(0.195f * L.scale * textScale, 0.195f * L.scale * textScale), false, false, false, text);
		Game::Print::drawstring(tip, L.panelLeft + 0.012f, footerTop + 0.042f);
	}

	void DrawSelection(const Layout& L, float optionY)
	{
		const float alpha = OpenAlpha();
		const float ease = SelectionEase();
		const float targetY = optionY + menuPos.y + L.optionRowH * 0.42f;

		float lerp = 1.0f;
		if (animQuality == AnimQuality::High) lerp = 0.22f;
		else if (animQuality == AnimQuality::Medium) lerp = 0.32f;
		else if (animQuality == AnimQuality::Low) lerp = 0.50f;

		if (s_selSmoothY < 0.0f)
			s_selSmoothY = targetY;
		else
			s_selSmoothY += (targetY - s_selSmoothY) * lerp;

		const float targetW = 0.985f;
		s_selSmoothW += (targetW - s_selSmoothW) * lerp;

		const auto sel = WithAlpha(SelectedBg(), (int)(SelectedBg().A * alpha * ease));
		const float rowH = L.optionRowH * (0.88f + 0.06f * ease);
		DRAW_RECT(L.panelCenterX, s_selSmoothY, L.panelWidth * s_selSmoothW, rowH, sel.R, sel.G, sel.B, sel.A, false);

		// Accent cursor bar
		const auto accent = Accent();
		DRAW_RECT(L.panelLeft + 0.0045f, s_selSmoothY, 0.0032f, rowH * 0.78f,
			accent.R, accent.G, accent.B, ClampByte((int)(230 * alpha * Pulse())), false);
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
