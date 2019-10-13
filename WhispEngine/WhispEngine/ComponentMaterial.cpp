#include "ComponentMaterial.h"
#include "GameObject.h"
#include "ComponentMesh.h"
#include "Application.h"

ComponentMaterial::ComponentMaterial(GameObject* parent) : Component(parent, ComponentType::MATERIAL)
{
	((ComponentMesh*)parent->GetComponent(ComponentType::MESH))->SetMaterial(this);
}


ComponentMaterial::~ComponentMaterial()
{
}

void ComponentMaterial::SetTexture(Texture * texture)
{
	this->texture = texture;
}

const bool ComponentMaterial::HasTexture() const
{
	return texture != nullptr;
}

const uint ComponentMaterial::GetIDTexture() const
{
	return texture->id;
}

void ComponentMaterial::OnInspector()
{
	if (ImGui::CollapsingHeader("Material")) {
		ImGui::Checkbox("Active", &is_active);
		ImGui::Text("%s", texture->path.data());
		ImGui::Button("Change Texture");

		ImGui::Text("(%d, %d)", texture->width, texture->height);
		ImGui::Image((ImTextureID)texture->id, ImVec2(128.f, 128.f));
	}
}

void ComponentMaterial::SetFaceColor(const float & r, const float & g, const float & b, const float & a)
{
	face_color[0] = r;
	face_color[1] = g;
	face_color[2] = b;
	face_color[3] = a;
}

void ComponentMaterial::SetFaceColor(const float *& c)
{
	memcpy(face_color, c, sizeof(float) * 4);
}

void ComponentMaterial::SetWireColor(const float & r, const float & g, const float & b, const float & a)
{
	wire_color[0] = r;
	wire_color[1] = g;
	wire_color[2] = b;
	wire_color[3] = a;
}

void ComponentMaterial::SetWireColor(const float *& c)
{
	memcpy(wire_color, c, sizeof(float) * 4);
}

const float * ComponentMaterial::GetFaceColor() const
{
	return &face_color[0];
}

const float * ComponentMaterial::GetWireColor() const
{
	return &wire_color[0];
}

Texture::Texture(const uint &id, const char* path, const int& width, const int& height)
	: id(id), path(path), name(App->file_system->GetFileNameFromPath(path)), width(width), height(height)
{
}
