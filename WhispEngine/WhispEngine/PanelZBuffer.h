#pragma once
#ifndef _PANEL_ZBUFFER__
#define _PANEL_ZBUFFER__

#include "Panel.h"
#include "Globals.h"
#include "Imgui/imgui.h"

class PanelZBuffer : public Panel
{
public:
	PanelZBuffer(const bool &start_active = true,
		const SDL_Scancode &shortcut1 = SDL_SCANCODE_UNKNOWN, const SDL_Scancode &shortcut2 = SDL_SCANCODE_UNKNOWN, const SDL_Scancode &shortcut3 = SDL_SCANCODE_UNKNOWN);
	~PanelZBuffer();

	void Update() override;

	const ImVec2 GetPanelSize();
	const ImVec2 GetPanelPos();

	bool focused = false;

private:

	ImVec2 panel_size = { 0.f, 0.f };
	ImVec2 panel_pos = { 0.f, 0.f };
};

#endif // !_PANEL_GAME__