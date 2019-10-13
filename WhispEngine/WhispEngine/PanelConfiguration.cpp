#include "PanelConfiguration.h"
#include "Application.h"
//#include "Imgui/imgui.h"


PanelConfiguration::PanelConfiguration(const bool &start_active, const SDL_Scancode &shortcut1, const SDL_Scancode &shortcut2, const SDL_Scancode &shortcut3)
	: Panel("Configuration", start_active, shortcut1, shortcut2, shortcut3)
{
	fps_reg.resize(100);
	ms_reg.resize(100);
	mem_reg.resize(100);
}


PanelConfiguration::~PanelConfiguration()
{
}

void PanelConfiguration::Update()
{
	if (ImGui::Begin(name.data(), &active, ImGuiWindowFlags_MenuBar))
	{
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Options")) {
				Options();
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}
		if (ImGui::CollapsingHeader("Application"))
		{
			Application();
		}
		if (ImGui::CollapsingHeader("Window"))
		{
			Window();
		}
		if (ImGui::CollapsingHeader("Render"))
		{
			Render();
		}
		if (ImGui::CollapsingHeader("Textures"))
		{
			Textures();
		}
		if (ImGui::CollapsingHeader("File System"))
		{
			FileSystem();
		}
		if (ImGui::CollapsingHeader("Input"))
		{
			Input();
		}
		if (ImGui::CollapsingHeader("Hardware"))
		{
			Hardware();
		}
	}
	ImGui::End();
}

void PanelConfiguration::Hardware()
{
	ImGui::Text("SDL Version: "); ImGui::SameLine(); ImGui::TextColored(IMGUI_COLOR_YELLOW, App->hardware->GetSDLVersion());
	ImGui::Text("OpenGL Version: "); ImGui::SameLine(); ImGui::TextColored(IMGUI_COLOR_YELLOW, App->hardware->GetOpenGLVersion());
	ImGui::Separator();

	std::string sdl_cpu_string = std::to_string(SDL_GetCPUCount()) + " (Cache: " + std::to_string(SDL_GetCPUCacheLineSize()) + "Kb)";
	ImGui::Text("CPUs: "); ImGui::SameLine(); ImGui::TextColored(IMGUI_COLOR_YELLOW, (sdl_cpu_string.c_str()));

	std::string sdl_ram_string = std::to_string(SDL_GetSystemRAM()) + "Mb";
	ImGui::Text("System RAM: "); ImGui::SameLine(); ImGui::TextColored(IMGUI_COLOR_YELLOW, (sdl_ram_string.c_str()));

	ImGui::Text("Caps: "); ImGui::SameLine(); ImGui::TextColored(IMGUI_COLOR_YELLOW, App->hardware->GetCapsHardware());
	ImGui::Separator();

	App->hardware->SetVRAMHardware();
	ImGui::Text("GPU: "); ImGui::SameLine(); ImGui::TextColored(IMGUI_COLOR_YELLOW, "vendor %u device %u", App->hardware->hardware.gpu_vendor, App->hardware->hardware.gpu_device);
	ImGui::Text("Brand: "); ImGui::SameLine(); ImGui::TextColored(IMGUI_COLOR_YELLOW, (App->hardware->hardware.gpu_brand));
	ImGui::Text("VRAM Budget: "); ImGui::SameLine(); ImGui::TextColored(IMGUI_COLOR_YELLOW, "%.1f Mb", App->hardware->hardware.vram_mb_budget);
	ImGui::Text("VRAM Usage: "); ImGui::SameLine(); ImGui::TextColored(IMGUI_COLOR_YELLOW, "%.1f Mb", App->hardware->hardware.vram_mb_usage);
	ImGui::Text("VRAM Available: "); ImGui::SameLine(); ImGui::TextColored(IMGUI_COLOR_YELLOW, "%.1f Mb", App->hardware->hardware.vram_mb_available);
	ImGui::Text("VRAM Reserved: "); ImGui::SameLine(); ImGui::TextColored(IMGUI_COLOR_YELLOW, "%.1f Mb", App->hardware->hardware.vram_mb_reserved);
}

void PanelConfiguration::Input()
{
	ImGui::Text("Mouse Position: "); ImGui::SameLine(); ImGui::TextColored(IMGUI_COLOR_YELLOW, "%i, %i", App->input->GetMouseX(), App->input->GetMouseY());
	ImGui::Text("Mouse Motion: "); ImGui::SameLine(); ImGui::TextColored(IMGUI_COLOR_YELLOW, "%i,%i", App->input->GetMouseXMotion(), App->input->GetMouseYMotion());
	ImGui::Text("Mouse Wheel: "); ImGui::SameLine(); ImGui::TextColored(IMGUI_COLOR_YELLOW, "%i", App->input->GetMouseWheel());
	ImGui::Separator();

	ImGui::BeginChild("Buffer");
	ImGui::TextUnformatted(App->input->text_buffer.begin());
	if (App->input->auto_scroll)
	{
		ImGui::SetScrollHereY(1.0f);
		App->input->auto_scroll = false;
	}
	ImGui::EndChild();
}

void PanelConfiguration::FileSystem()
{
	// TODO: Wait for the next class indications
	char path[100];
	GetModuleFileNameA(NULL, path, 100);
	ImGui::Text("Base Path:");
	ImGui::TextColored(IMGUI_COLOR_YELLOW, path);
}

void PanelConfiguration::Options()
{
	if (ImGui::MenuItem("Set Defaults"))
	{
		App->LoadDefaultConfiguration();
	}
	if (ImGui::MenuItem("Load"))
	{
		App->LoadConfiguration();
	}
	if (ImGui::MenuItem("Save"))
	{
		App->SaveConfiguration();
	}
}

void PanelConfiguration::Window()
{
	ImGui::Text("Icon:"); ImGui::SameLine(); ImGui::Text("*null*");

	if (ImGui::SliderFloat("Brightness", &App->window->bright, 0.0f, 1.0f)) {
		SDL_SetWindowBrightness(App->window->window, App->window->bright);
	}

	if (ImGui::SliderInt("Width", (int*)&App->window->screen_width, 640, 1920))
	{
		App->window->SetWindowSize(App->window->screen_width, App->window->screen_height);
	}
	if (ImGui::SliderInt("Height", (int*)&App->window->screen_height, 480, 1080))
	{
		App->window->SetWindowSize(App->window->screen_width, App->window->screen_height);
	}


	SDL_DisplayMode display;
	SDL_GetCurrentDisplayMode(NULL, &display);
	ImGui::Text("Refresh rate:"); ImGui::SameLine(); ImGui::TextColored(IMGUI_COLOR_YELLOW, std::to_string(display.refresh_rate).data());

	if (ImGui::Checkbox("Fullscreen", &checkbox_fullscreen_window))
	{
		if (checkbox_fulldesktop_window)
			checkbox_fulldesktop_window = false;

		SDL_SetWindowFullscreen(App->window->window, SDL_WINDOW_FULLSCREEN);
	}
	if (ImGui::Checkbox("Bordeless", &checkbox_borderless_window))
	{
		if (!checkbox_fullscreen_window && !checkbox_fulldesktop_window)
			SDL_SetWindowBordered(App->window->window, (SDL_bool)!checkbox_borderless_window);
		else
			checkbox_borderless_window = false;
	}
	if (ImGui::Checkbox("Full Desktop", &checkbox_fulldesktop_window))
	{
		if (checkbox_fullscreen_window)
			checkbox_fullscreen_window = false;

		SDL_SetWindowFullscreen(App->window->window, SDL_WINDOW_FULLSCREEN_DESKTOP);
	}
	if (ImGui::Checkbox("Resizable", &checkbox_resizable_window))
	{
		SDL_SetWindowFullscreen(App->window->window, SDL_WINDOW_RESIZABLE);
		checkbox_fullscreen_window = checkbox_fulldesktop_window = checkbox_borderless_window = false;
		SDL_SetWindowBordered(App->window->window, (SDL_bool)!checkbox_borderless_window);
	}
	if (!checkbox_fullscreen_window && !checkbox_fulldesktop_window)
	{
		SDL_SetWindowFullscreen(App->window->window, SDL_WINDOW_RESIZABLE);
	}
}

void PanelConfiguration::Render()
{
	ImGui::Checkbox("Wireframe", &App->renderer3D->wireframe);
	ImGui::Checkbox("Fill faces", &App->renderer3D->fill);
	ImGui::Separator();
	ImGui::Checkbox("Show grid", &App->scene_intro->show_grid);
	ImGui::ColorPicker3("Background Color", App->renderer3D->background_color);
	ImGui::Separator();

}

void PanelConfiguration::Application()
{
	int length = 30;

	char name[30];
	char org[30];
	sprintf_s(name, length, App->GetAppName());
	sprintf_s(org, length, App->GetOrganizationName());

	if (ImGui::InputText("App Name", name, length, ImGuiInputTextFlags_EnterReturnsTrue)) {
		App->SetAppName(name);
		App->window->SetTitle(name);
	}
	if (ImGui::InputText("Organitzation", org, length)) {
		App->SetOrganizationName(org);
	}

	ImGui::SliderInt("Max FPS", (int*)&App->framerate_cap, 1, 120);
	ImGui::Text("Limit Framerate: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1, 1, 0, 1), "%i", App->framerate_cap);

	//FPS graphic

	if (App->last_sec_frame_count == 1) { // At the beggining of the second
		PushBackVectorAsQueue(fps_reg, App->prev_last_sec_frame_count);
	}

	char title[25];
	sprintf_s(title, 25, "Framerate %.1f", fps_reg[fps_reg.size() - 1]);
	ImGui::PlotHistogram("framerate", &fps_reg[0], fps_reg.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));
	//MS graphic
	PushBackVectorAsQueue(ms_reg, 1000.0f / App->prev_last_sec_frame_count);

	sprintf_s(title, 25, "Miliseconds %0.1f", ms_reg[ms_reg.size() - 1]);
	ImGui::PlotHistogram("##miliseconds", &ms_reg[0], ms_reg.size(), 0, title, 0.0f, 40.0f, ImVec2(310, 100));
	
	App->hardware->UpdateMemory();

	if (App->last_sec_frame_count == 1) { // At the beggining of the second
		PushBackVectorAsQueue(mem_reg, App->hardware->config.total_reported_mem);
	}

	ImGui::PlotHistogram("##memory", &mem_reg[0], mem_reg.size(), 0.f, "Memory Consumption", 0.0f, App->hardware->config.peak_reported_mem * 1.5f, ImVec2(310, 100));

	ImGui::Text("Total Reported Mem: "); ImGui::SameLine(); ImGui::Text("%i", App->hardware->config.total_reported_mem);
	ImGui::Text("Total Actual Mem: "); ImGui::SameLine(); ImGui::Text("%i", App->hardware->config.total_actual_mem);
	ImGui::Text("Peak Reported Mem: "); ImGui::SameLine(); ImGui::Text("%i", App->hardware->config.peak_reported_mem);
	ImGui::Text("Peak Actual Mem: "); ImGui::SameLine(); ImGui::Text("%i", App->hardware->config.peak_actual_mem);
	ImGui::Text("Accumulated Reported Mem: "); ImGui::SameLine(); ImGui::Text("%i", App->hardware->config.accumulated_reported_mem);
	ImGui::Text("Accumulated Actual Mem: "); ImGui::SameLine(); ImGui::Text("%i", App->hardware->config.accumulated_actual_mem);
	ImGui::Text("Accumulated Alloc Mem: "); ImGui::SameLine(); ImGui::Text("%i", App->hardware->config.accumulated_alloc_unit);
	ImGui::Text("Total Alloc Unit Mem: "); ImGui::SameLine(); ImGui::Text("%i", App->hardware->config.total_alloc_unity_count);
	ImGui::Text("PeakAlloc Unit Mem: "); ImGui::SameLine(); ImGui::Text("%i", App->hardware->config.peak_alloc_unit_count);
}

void PanelConfiguration::Textures()
{
	auto textures = App->object_manager->GetTextures();
	for (auto i = textures->begin(); i != textures->end(); ++i) {
		if (ImGui::ImageButton((ImTextureID)(*i).id, ImVec2((*i).width / 4, (*i).height / 4))) {
			/*App->object_manager->SelectTexture(*i);*/
		}
		ImGui::SameLine();
	}
	ImGui::NewLine();
}

void PanelConfiguration::PushBackVectorAsQueue(std::vector<float> &vector, const float &value)
{
	for (int i = 0; i < vector.size() - 1; ++i) {
		vector[i] = vector[i + 1];
	}
	vector[vector.size() - 1] = value;
}