#include "ComponentScript.h"
#include "Imgui/imgui.h"
#include "Application.h"
#include "FileSystem.h"
#include "GameObject.h"
#include "ModuleScripting.h"
#include "Lua/LuaBridge/LuaBridge.h"

ComponentScript::ComponentScript(GameObject* parent) : Component(parent,ComponentType::SCRIPT)
{
}


ComponentScript::~ComponentScript()
{
}

void ComponentScript::Update()
{
	if (is_assigned)
	{
		luabridge::setGlobal(App->scripting->GetState(), object, "object");
		luabridge::setGlobal(App->scripting->GetState(), object->transform, "transform");
		App->scripting->ExecuteFunctionScript(script_path.c_str(), name.c_str(), "Update");
		lua_pop(App->scripting->GetState(), -1);
		lua_pop(App->scripting->GetState(), -1);
	}
}

void ComponentScript::OnInspector()
{
	if (ImGui::CollapsingHeader(title.data(), ImGuiTreeNodeFlags_DefaultOpen)) {
		if (ImGui::BeginPopupContextItem("Mesh")) {
			if (ImGui::Button("Delete")) {
				object->DeleteComponent(this);
			}
			ImGui::EndPopup();
		}
		if (is_assigned) {
			ImGui::Text("Script\t%s", name.data());
		}
		else {
			static char buffer[50];
			if (ImGui::InputText("", buffer, 50, ImGuiInputTextFlags_EnterReturnsTrue)) {
				name.assign(buffer);
				title = name + " (Script)";
				script_path.append(name + ".lua");
				App->file_system->Copy("Assets/Internal/model.lua", script_path.data());

				char* file = App->file_system->GetTextFile(script_path.data());
				std::string sfile(file);
				delete[] file;

				while (sfile.find("Model") != std::string::npos) {
					sfile.replace(sfile.find("Model"), 5, name.c_str());
				}
				while (sfile.find('\r') != std::string::npos) {
					sfile.replace(sfile.find('\r'), 1, "");
				}

				App->file_system->SaveTextFile(sfile.data(), script_path.c_str());
				is_assigned = true;
			}
		}
	}
}

void ComponentScript::SetScript(const char * path)
{
	script_path = path;
	is_assigned = true;
	name = App->file_system->GetFileNameFromPath(script_path.c_str());
	title = name + " (Script)";
}

void ComponentScript::Save(nlohmann::json & node)
{
	node["script"] = script_path.c_str();
}

void ComponentScript::Load(const nlohmann::json & node)
{
	SetScript(node.value("script", "NONE").c_str());
}
