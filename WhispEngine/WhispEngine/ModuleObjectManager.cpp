#include "ModuleObjectManager.h"
#include "ComponentTransform.h"
#include "Application.h"
#include "Brofiler/Brofiler.h"
#include "Imgui/ImGuizmo.h"
#include "PanelScene.h"

ModuleObjectManager::ModuleObjectManager()
{
	name.assign("ObjectManager");
}


ModuleObjectManager::~ModuleObjectManager()
{
}

bool ModuleObjectManager::Start()
{
	root = new GameObject(nullptr);
	App->importer->ImportTexture("Assets/Textures/Checker.dds");

	return true;
}

update_status ModuleObjectManager::Update()
{
	BROFILER_CATEGORY("GameObject Manager", Profiler::Color::MediumSpringGreen);

	UpdateGameObject(root);

	//Camera
	App->camera->GetGameCamera()->DrawInsideFrustum();

	return UPDATE_CONTINUE;
}

void ModuleObjectManager::UpdateGameObject(GameObject* &obj)
{
	if (obj->IsActive()) {
		
		glPushMatrix();		
		glMultMatrixf(((ComponentTransform*)obj->GetComponent(ComponentType::TRANSFORM))->GetGlobalMatrix().Transposed().ptr());

		obj->Update();

		glPopMatrix();
		if (!obj->children.empty()) {
			for (auto i = obj->children.begin(); i != obj->children.end(); ++i) {
				UpdateGameObject(*i);
			}
		}
	}
}

bool ModuleObjectManager::CleanUp()
{
	delete root;

	for (auto t = textures.begin(); t != textures.end(); t++) {
		glDeleteTextures(1, &(*t)->id);
		delete *t;
	}
	textures.clear();

	return true;
}

GameObject * ModuleObjectManager::CreateGameObject(GameObject * parent)
{
	if (parent == nullptr)
		parent = root;

	GameObject* ret = new GameObject(parent);

	return ret;
}

void ModuleObjectManager::DestroyGameObject(GameObject * obj)
{
	obj->parent->children.erase(std::find(obj->parent->children.begin(), obj->parent->children.end(), obj));
	delete obj;
}

GameObject * ModuleObjectManager::GetRoot() const
{
	return root;
}
void ModuleObjectManager::GetChildsFrom(GameObject* &obj, std::vector<GameObject*> &vector)
{

	if (!obj->children.empty()) {

		for (auto i = obj->children.begin(); i != obj->children.end(); ++i) {
			vector.push_back(*i);

			GetChildsFrom(*i, vector);

		}
	}

}

GameObject * ModuleObjectManager::GetSelected() const
{
	return selected;
}

void ModuleObjectManager::SetSelected(GameObject * select)
{

	if (selected != select)
	{
		if (selected != nullptr)
			selected->Deselect();
		
		if (select != nullptr)
			select->Select();
		selected = select;
	}
}

void ModuleObjectManager::MousePick()
{
	static ImVec2 scene_position;
	static ImVec2 scene_size;

	scene_position = App->gui->scene->GetPanelPos();
	scene_size = App->gui->scene->GetPanelSize();
	scene_size = { scene_position.x + scene_size.x, scene_position.y + scene_size.y };



	Rect window = { (int)scene_position.x, (int)scene_position.y, (int)scene_size.x, (int)scene_size.y };
	float2 mouse_pos = {(float)App->input->GetMouseX(), (float)App->input->GetMouseY()};

	//Point Rect Formula
	if (mouse_pos.x >= window.left && mouse_pos.x <= window.right && mouse_pos.y > window.top && mouse_pos.y < window.bottom)
	{
		//The point (1, 1) corresponds to the top-right corner of the near plane
		//(-1, -1) is bottom-left

		float first_normalized_x = (mouse_pos.x - window.left) / (window.right - window.left);
		float first_normalized_y = (mouse_pos.y - window.top) / (window.bottom - window.top);

		float normalized_x = (first_normalized_x * 2) - 1;
		float normalized_y = 1 - (first_normalized_y * 2);

		LineSegment picking = App->camera->GetSceneCamera()->GetFrustum().UnProjectLineSegment(normalized_x, normalized_y);

		float distance = 99999999999;
		GameObject* closest = nullptr;

		std::vector<GameObject*> game_objects;
		GetChildsFrom(root, game_objects);

		//kdtree->GetElementsToTest(picking, 0, App->camera->GetCurrentCamera()->GetFarPlaneDistance(), gos);   //Need to be finished Octree

		for (std::vector<GameObject*>::iterator it = game_objects.begin(); it != game_objects.end(); it++)
		{
			bool hit;
			float dist;
			//(*it)->TestRay(picking, hit, dist); //Need to be finihed AABB

			if (hit)
			{
				if (dist < distance)
				{
					distance = dist;
					closest = (*it);
				}
			}
		}

		if (closest != nullptr)
		{
			std::vector<GameObject*> selected_and_childs;
			if (selected != nullptr)
			{
				selected_and_childs.push_back(selected);
				GetChildsFrom(selected, selected_and_childs);
			}

			for (std::vector<GameObject*>::iterator it = selected_and_childs.begin(); it != selected_and_childs.end();)
			{
				(*it)->Deselect();
				it = selected_and_childs.erase(it);
			}

			//TODO solve this
			GetChildsFrom(closest, selected_and_childs);
		}
	}
}

std::vector<Texture*>* ModuleObjectManager::GetTextures()
{
	return &textures;
}

const char * ModuleObjectManager::PrimitivesToString(const Primitives prim)
{
	const char* name = nullptr;
	switch (prim)
	{
	case Primitives::CUBE:
		name = "Cube";
		break;
	case Primitives::TETRAHEDRON:
		name = "Tetrahedron";
		break;
	case Primitives::OCTAHEDRON:
		name = "Octahedron";
		break;
	case Primitives::DODECAHEDRON:
		name = "Dodecahedron";
		break;
	case Primitives::ICOSAHEDRON:
		name = "Icosahedron";
		break;
	case Primitives::SPHERE:
		name = "Sphere";
		break;
	case Primitives::HEMISPHERE:
		name = "Hemisphere";
		break;
	case Primitives::TORUS:
		name = "Torus";
		break;
	case Primitives::CONE:
		name = "Cone";
		break;
	case Primitives::CYLINDER:
		name = "Cylinder";
		break;
	default:
		LOG("Added more primitives than expected, add the missing primitives to the for");
		break;
	}

	return name;
}

void ModuleObjectManager::UpdateGuizmo()
{
	ImGuizmo::MODE guizmoApply = guizmoMode;

	if (gizmoOperation == ImGuizmo::OPERATION::SCALE)
		guizmoApply = ImGuizmo::MODE::WORLD;

	ChangeGuizmoOperation(gizmoOperation);

	float4x4 global_transform = ((ComponentTransform*)GetSelected()->GetComponent(ComponentType::TRANSFORM))->GetGlobalMatrix().Transposed();
	float4x4 moved_transformation;
	float matrix_properties[16];

	ImGuizmo::Manipulate(App->camera->GetSceneCamera()->GetViewMatrix().ptr(), App->camera->GetSceneCamera()->GetProjectionMatrix().ptr(), gizmoOperation, guizmoApply, global_transform.ptr(), matrix_properties);
	FillMatrix(moved_transformation, matrix_properties);
	
	std::vector<GameObject*> selected_and_childs;
	if (selected != nullptr)
	{
		selected_and_childs.push_back(selected);
		GetChildsFrom(selected, selected_and_childs);
	}

	for (std::vector<GameObject*>::iterator i = selected_and_childs.begin(); i != selected_and_childs.end(); ++i)
	{
		if (ImGuizmo::IsUsing() && !App->camera->is_moving_camera)
		{
			float4x4 position_matrix = float4x4::identity;
			float4x4 rotation_matrix = float4x4::identity;
			float4x4 scale_matrix = float4x4::identity;

			switch (gizmoOperation)
			{
			case ImGuizmo::OPERATION::TRANSLATE:
			{
				position_matrix = moved_transformation * ((ComponentTransform*)(*i)->GetComponent(ComponentType::TRANSFORM))->GetGlobalMatrix();
				((ComponentTransform*)(*i)->GetComponent(ComponentType::TRANSFORM))->SetGlobalMatrix(position_matrix);
			}
			break;

			case ImGuizmo::OPERATION::ROTATE:
			{
				rotation_matrix = moved_transformation * ((ComponentTransform*)(*i)->GetComponent(ComponentType::TRANSFORM))->GetGlobalMatrix();
				((ComponentTransform*)(*i)->GetComponent(ComponentType::TRANSFORM))->SetGlobalMatrix(rotation_matrix);
			}
			break;
			case ImGuizmo::OPERATION::SCALE:
			{
				float4x4 save_trans = moved_transformation;
				moved_transformation = moved_transformation * last_moved_transformation.Inverted();

				scale_matrix = moved_transformation * ((ComponentTransform*)(*i)->GetComponent(ComponentType::TRANSFORM))->GetGlobalMatrix();
				((ComponentTransform*)(*i)->GetComponent(ComponentType::TRANSFORM))->SetGlobalMatrix(scale_matrix);

				last_moved_transformation = save_trans;
			}
			break;
			}
		}
		else
		{
			last_moved_transformation = float4x4::identity;
		}
	}
}

void ModuleObjectManager::FillMatrix(float4x4 & matrix, float o[])
{
	matrix = float4x4(
		o[0], o[4], o[8], o[12],
		o[1], o[5], o[9], o[13],
		o[2], o[6], o[10], o[14],
		o[3], o[7], o[11], o[15]);
}

void ModuleObjectManager::ChangeGuizmoOperation(ImGuizmo::OPERATION &gizmoOperation)
{
	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN) {
		gizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
	}
	else if (App->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN) {
		gizmoOperation = ImGuizmo::OPERATION::ROTATE;
	}
	else if (App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN) {
		gizmoOperation = ImGuizmo::OPERATION::SCALE;
	}

}

void ModuleObjectManager::ChangeGuizmoMode(ImGuizmo::MODE & gizmoMode)
{
	this->guizmoMode = gizmoMode;
}

void ModuleObjectManager::AddTexture(Texture * tex)
{
	textures.push_back(tex);
}

Mesh_info * ModuleObjectManager::CreateMesh(const uint & n_vertex, const float * vertex, const uint & n_index, const uint * index, const float * normals, const float* texCoords)
{
	Mesh_info *mesh = new Mesh_info();

	FillVertex(mesh, n_vertex, vertex);

	FillIndex(mesh, n_index*3, index);

	FillNormals(mesh, normals);

	if (texCoords != nullptr)
		FillTextureCoords(mesh, texCoords);

	mesh->SetGLBuffers();

	return mesh;
}

Mesh_info * ModuleObjectManager::CreateMesh(const aiMesh * mesh)
{
	Mesh_info *ret = new Mesh_info();

	FillVertex(ret, mesh->mNumVertices, (float*)mesh->mVertices);

	if (mesh->HasFaces())
	{
		FillIndex(ret, mesh->mNumFaces, mesh->mFaces);
	}

	if (mesh->HasNormals()) {

		FillNormals(ret, (float*)mesh->mNormals);
	}

	if (mesh->HasTextureCoords(0)) {
		FillTextureCoords(ret, (float*)mesh->mTextureCoords[0]);
	}

	// Generate AABB
	ret->local_box.SetNegativeInfinity();
	ret->local_box.Enclose((float3*)ret->vertex.data, ret->vertex.size);

	ret->obb = ret->local_box;
	

	ret->aabb.SetNegativeInfinity();
	ret->aabb.Enclose(ret->obb);


	ret->SetGLBuffers();

	return ret;
}

void ModuleObjectManager::FillNormals(Mesh_info * ret, const float * normals)
{
	float magnitude = 0.3f; // To multiply normalized vectors

	if (normals != nullptr) {
		// Vertex Normals --------------------------------------------------
		ret->vertex_normals.size = ret->vertex.size * 3;
		ret->vertex_normals.data = new float[ret->vertex_normals.size];
		memcpy(ret->vertex_normals.data, normals, sizeof(float) * ret->vertex_normals.size);
	}

	// Face Normals ----------------------------------------------------
	ret->face_normals.size = ret->index.size * 2;
	ret->face_normals.data = new float[ret->face_normals.size];

	for (int k = 0; k < ret->index.size/3; k += 3) {
		vec3 p1(ret->vertex.data[ret->index.data[k] * 3],     ret->vertex.data[ret->index.data[k] * 3 + 1],     ret->vertex.data[ret->index.data[k] * 3 + 2]);
		vec3 p2(ret->vertex.data[ret->index.data[k + 1] * 3], ret->vertex.data[ret->index.data[k + 1] * 3 + 1], ret->vertex.data[ret->index.data[k + 1] * 3 + 2]);
		vec3 p3(ret->vertex.data[ret->index.data[k + 2] * 3], ret->vertex.data[ret->index.data[k + 2] * 3 + 1], ret->vertex.data[ret->index.data[k + 2] * 3 + 2]);

		ret->face_normals.data[k * 2]	  = (p1.x + p2.x + p3.x) / 3.f;
		ret->face_normals.data[k * 2 + 1] = (p1.y + p2.y + p3.y) / 3.f;
		ret->face_normals.data[k * 2 + 2] = (p1.z + p2.z + p3.z) / 3.f;

		vec3 v1 = p2 - p1;
		vec3 v2 = p3 - p1;

		vec3 v_norm = cross(v1, v2);
		v_norm = normalize(v_norm);

		ret->face_normals.data[k * 2 + 3] = ret->face_normals.data[k * 2]     + v_norm.x * magnitude;
		ret->face_normals.data[k * 2 + 4] = ret->face_normals.data[k * 2 + 1] + v_norm.y * magnitude;
		ret->face_normals.data[k * 2 + 5] = ret->face_normals.data[k * 2 + 2] + v_norm.z * magnitude;
	}
}

void ModuleObjectManager::FillIndex(Mesh_info * ret, const uint & n_index, const aiFace* faces)
{
	ret->index.size = n_index * 3 * 3;
	ret->index.data = new uint[ret->index.size];

	for (uint j = 0; j < n_index; ++j)
	{
		if (faces[j].mNumIndices != 3)
		{
			LOG("WARNING, geometry face with != 3 indices!");
			ret->index.data[j * 3] = 0;
			ret->index.data[j * 3 + 1] = 0;
			ret->index.data[j * 3 + 2] = 0;
		}
		else
		{
			memcpy(&ret->index.data[j * 3], faces[j].mIndices, sizeof(uint) * 3);
		}
	}
	LOG("New mesh with %i faces", ret->index.size / 6);
}

void ModuleObjectManager::FillIndex(Mesh_info * ret, const uint & n_index, const uint * index)
{
	ret->index.size = n_index * 3;
	ret->index.data = new uint[ret->index.size];
	memcpy(ret->index.data, index, sizeof(uint) * n_index);
	LOG("New mesh with %i faces", ret->index.size / 3);
}

void ModuleObjectManager::FillVertex(Mesh_info * ret, const uint & n_vertex, const float* vertex)
{
	ret->vertex.size = n_vertex;
	ret->vertex.data = new float[ret->vertex.size * 3];
	memcpy(ret->vertex.data, vertex, sizeof(float) * ret->vertex.size * 3);
	LOG("New mesh with %d vertex", ret->vertex.size);
}

void ModuleObjectManager::FillTextureCoords(Mesh_info * mesh, const float * textureCoords)
{
	mesh->tex_coords.size = mesh->vertex.size;
	mesh->tex_coords.data = new float[mesh->tex_coords.size * 3];
	memcpy(mesh->tex_coords.data, textureCoords, sizeof(float) * mesh->tex_coords.size * 3);
}



Mesh_info * ModuleObjectManager::CreateMeshPrimitive(const Primitives & type)
{
	Object_data data = Object_data();
	par_shapes_mesh* prim = nullptr;

	switch (type)
	{
	case Primitives::CUBE:
		prim = par_shapes_create_cube();
		break;
	case Primitives::TETRAHEDRON:
		prim = par_shapes_create_tetrahedron();
		break;
	case Primitives::OCTAHEDRON:
		prim = par_shapes_create_octahedron();
		break;
	case Primitives::DODECAHEDRON:
		prim = par_shapes_create_dodecahedron();
		break;
	case Primitives::ICOSAHEDRON:
		prim = par_shapes_create_icosahedron();
		break;
	case Primitives::SPHERE:
		//TODO Create a sphere with radious, rings and sectors, not by subdivisions https://stackoverflow.com/questions/5988686/creating-a-3d-sphere-in-opengl-using-visual-c/5989676#5989676
		prim = par_shapes_create_subdivided_sphere(data.slices);
		break;
	case Primitives::HEMISPHERE:
		prim = par_shapes_create_hemisphere(data.slices, data.rings);
		break;
	case Primitives::TORUS:
		prim = par_shapes_create_torus(data.slices, data.rings, data.radius);
		break;
	case Primitives::CONE:
		prim = par_shapes_create_cone(data.slices, data.rings);
		break;
	case Primitives::CYLINDER:
		prim = par_shapes_create_cylinder(data.slices, data.rings);
		break;
	default:
		LOG("Primitive not found to create. id: %i", (int)type);
		break;
	}

	Mesh_info* mesh = CreateMesh(prim->npoints, prim->points, prim->ntriangles, prim->triangles, prim->normals, prim->tcoords);
	par_shapes_free_mesh(prim);

	return mesh;
}

bool ModuleObjectManager::CreatePrimitive(const Primitives & type, const Object_data &data)
{
	bool ret = true;

	par_shapes_mesh* prim = nullptr;

	switch (type)
	{
	case Primitives::CUBE:
		prim = par_shapes_create_cube();
		break;
	case Primitives::TETRAHEDRON:
		prim = par_shapes_create_tetrahedron();
		break;
	case Primitives::OCTAHEDRON:
		prim = par_shapes_create_octahedron();
		break;
	case Primitives::DODECAHEDRON:
		prim = par_shapes_create_dodecahedron();
		break;
	case Primitives::ICOSAHEDRON:
		prim = par_shapes_create_icosahedron();
		break;
	case Primitives::SPHERE:
		//TODO Create a sphere with radious, rings and sectors, not by subdivisions https://stackoverflow.com/questions/5988686/creating-a-3d-sphere-in-opengl-using-visual-c/5989676#5989676
		prim = par_shapes_create_subdivided_sphere(data.slices);
		break;
	case Primitives::HEMISPHERE:
		prim = par_shapes_create_hemisphere(data.slices, data.rings);
		break;
	case Primitives::TORUS:
		prim = par_shapes_create_torus(data.slices, data.rings, data.radius);
		break;
	case Primitives::CONE:
		prim = par_shapes_create_cone(data.slices, data.rings);
		break;
	case Primitives::CYLINDER:
		prim = par_shapes_create_cylinder(data.slices, data.rings);
		break;
	default:
		LOG("Primitive not found to create. id: %i", (int)type);
		break;
	}

	par_shapes_translate(prim, data.pos.x, data.pos.y, data.pos.z);

	if (data.rotate.axis[0] != 0 || data.rotate.axis[1] != 0 || data.rotate.axis[2] != 0) {
		float mgn = std::sqrt(data.rotate.axis[0] * data.rotate.axis[0] + data.rotate.axis[1] * data.rotate.axis[1] + data.rotate.axis[2] * data.rotate.axis[2]); // normalize rotation axis
		float rot[3] = { data.rotate.axis[0] / mgn,data.rotate.axis[1] / mgn ,data.rotate.axis[2] / mgn };
		par_shapes_rotate(prim, data.rotate.angle, rot);
	}

	par_shapes_scale(prim, data.scale.x, data.scale.y, data.scale.z);

	GameObject* obj = CreateGameObject(nullptr);
	obj->SetName(PrimitivesToString(type));

	ComponentMesh* mesh = (ComponentMesh*)obj->CreateComponent(ComponentType::MESH);
	mesh->mesh = CreateMesh(prim->npoints, prim->points, prim->ntriangles, prim->triangles, prim->normals, prim->tcoords);
	
	ComponentMaterial* mat = (ComponentMaterial*)obj->GetComponent(ComponentType::MATERIAL);
	mat->SetFaceColor(data.face_color[0],data.face_color[1],data.face_color[2],1.f); 
	mat->SetWireColor(data.wire_color[0], data.wire_color[1], data.wire_color[2], 1.f);

	par_shapes_free_mesh(prim);

	return ret;
}

void ModuleObjectManager::Demo()
{
	std::vector<par_shapes_mesh*> prim;
	prim.resize((int)Primitives::MAX);
	prim[0] = par_shapes_create_cube();
	prim[1] = par_shapes_create_tetrahedron();
	prim[2] = par_shapes_create_octahedron();
	prim[3] = par_shapes_create_dodecahedron();
	prim[4] = par_shapes_create_icosahedron();
	prim[5] = par_shapes_create_subdivided_sphere(3);
	prim[6] = par_shapes_create_hemisphere(10, 10);
	prim[7] = par_shapes_create_torus(10, 10, 0.5f);
	prim[8] = par_shapes_create_cone(10, 10);
	prim[9] = par_shapes_create_cylinder(10, 10);

	
	int posx = -10;
	for (auto i = prim.begin(); i != prim.end(); ++i) {
		par_shapes_translate(*i, posx, 0.f, 3);

		GameObject* obj = CreateGameObject(nullptr);
		obj->SetName(PrimitivesToString((Primitives)std::distance(prim.begin(), i)));

		ComponentMesh* mesh = (ComponentMesh*)obj->CreateComponent(ComponentType::MESH);
		mesh->mesh = CreateMesh((*i)->npoints, (*i)->points, (*i)->ntriangles, (*i)->triangles, (*i)->normals, (*i)->tcoords);

		ComponentMaterial* mat = (ComponentMaterial*)obj->GetComponent(ComponentType::MATERIAL);

		float rnd_color[4] = { App->random->Randomf(0.f,1.f), App->random->Randomf(0.f,1.f), App->random->Randomf(0.f,1.f), 1.f };

		mat->SetFaceColor(&rnd_color[0]);

		par_shapes_free_mesh((*i));
		posx += 3;
	}

	prim.clear();	
}
