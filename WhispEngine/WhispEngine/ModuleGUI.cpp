#include "Application.h"
#include "Globals.h"
#include "ModuleGUI.h"

#include "ModuleWindow.h"
#include "ModuleRenderer3D.h"

#include "Imgui/imgui.h"
#include "Imgui/imgui_impl_sdl.h"
#include "Imgui/imgui_impl_opengl3.h"


ModuleGUI::ModuleGUI(Application * app, bool enable_true) :Module(app, enable_true)
{
}

ModuleGUI::~ModuleGUI()
{
}

bool ModuleGUI::Init()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	
	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer3D->context);
	ImGui_ImplOpenGL3_Init((const char*)glGetString(GL_VERSION));

	ImGui::StyleColorsDark();

	return true;
}

update_status ModuleGUI::PreUpdate()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	
	ImGui::NewFrame();

	return UPDATE_CONTINUE;
}

update_status ModuleGUI::Update()
{
	update_status ret = update_status::UPDATE_CONTINUE;

	ImGui::BeginMainMenuBar();

	if (ImGui::Button("Demo")) 
	{
		show_demo_window = !show_demo_window;
	}
	if (ImGui::Button("Example"))
	{
		show_example_window = !show_example_window;
	}
	if (ImGui::Button("Close")) 
	{
		ret = update_status::UPDATE_STOP;
	}
	ImGui::EndMainMenuBar();

	//  Button
	ImGui::Begin("Mystery Windows");
	ImGui::SetWindowSize(ImVec2(150, 60));
	if (ImGui::Button("Click me"))
	{
		show_example2_window = !show_example2_window;
	}	
		
	if (show_example2_window)
	{
		ImGui::Text("I'm a text");
		ImGui::SetWindowSize(ImVec2(500, 640));
	}
	ImGui::End();



	if (show_demo_window)
	{
		ImGui::ShowDemoWindow();
	}
	if (show_example_window)
	{
		ImGui::Text("Hola Gente estoy Doramio");
	}

	return ret;
}

update_status ModuleGUI::PostUpdate()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	return UPDATE_CONTINUE;
}

bool ModuleGUI::CleanUp() {

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	return true;
}
