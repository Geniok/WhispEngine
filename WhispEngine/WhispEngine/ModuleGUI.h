#pragma once
#include "Module.h"

class ModuleGUI :public Module {
public:
	ModuleGUI(Application* app, bool enable_true = true);
	~ModuleGUI();

public:
	bool Init();

	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();

	bool CleanUp();

private:
	bool show_demo_window = false;
	bool show_example_window = false;
	bool show_example2_window = false;
};