#pragma once
#include <vector>
#include "Panel.h"
#include "Globals.h"
#include "GameObject.h"
class PanelConfiguration :
	public Panel
{
public:
	PanelConfiguration(const bool &start_active = true, const SDL_Scancode &shortcut1 = SDL_SCANCODE_UNKNOWN, const SDL_Scancode &shortcut2 = SDL_SCANCODE_UNKNOWN, const SDL_Scancode &shortcut3 = SDL_SCANCODE_UNKNOWN);
	~PanelConfiguration();

	void Update() override;

private:
	void Hardware();
	void Input();
	void FileSystem();
	void Options();
	void Window();
	void Render();
	void Application();
	void Viewport();
	void Frustum();
	void zBuffer();
	void Skybox();

private:
	void PushBackVectorAsQueue(std::vector<float> &vector, const float &value);

private:
	bool checkbox_fullscreen_window = false;
	bool checkbox_borderless_window = false;
	bool checkbox_fulldesktop_window = false;
	bool checkbox_resizable_window = false;
	bool checkbox_zbuffer = false;
	bool checkbox_skybox = true;

	bool select_tex = false;

	std::vector<float> ms_reg;
	std::vector<float> fps_reg;
	std::vector<float> mem_reg;

public:
	bool active_skybox = true;
	
	
};

