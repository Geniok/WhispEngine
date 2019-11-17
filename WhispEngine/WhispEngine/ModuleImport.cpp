#include "ModuleImport.h"

#include "Application.h"
#include "ModuleObjectManager.h"

#include "ComponentTransform.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"

#include "Brofiler/Brofiler.h"

#include "ModelImporter.h"
#include "MaterialImporter.h"
#include "MeshImporter.h"

#include <filesystem>

#pragma comment (lib, "Assimp/libx86/assimp.lib")


ModuleImport::ModuleImport()
{
	name.assign("Importer");
}


ModuleImport::~ModuleImport()
{
}

bool ModuleImport::Start()
{
	// Stream log messages to Debug window
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);
	
	model		= new ModelImporter();
	material	= new MaterialImporter();
	mesh		= new MeshImporter();

	CreateLibrary();

	// Charge logo texture
	//logo_txt = ImportTexture("Assets/logo.png");
	//logo_txt->visible_on_inspector = false;

	return true;
}

bool ModuleImport::CleanUp()
{
	// detach log stream
	aiDetachAllLogStreams();

	delete model;
	delete material;
	delete mesh;

	return true;
}

bool ModuleImport::Import(const char * path)
{
	bool ret = false;
	switch (App->dummy_file_system->GetFormat(path))
	{
	case FileSystem::Format::JSON:
		break;
	case FileSystem::Format::DDS:
	case FileSystem::Format::PNG:
	case FileSystem::Format::JPG:
		ret = material->Import(path);
		break;
	case FileSystem::Format::SCENE:
		ret = App->scene_intro->LoadScene();
		break;
	case FileSystem::Format::FBX:
		ret = model->Import(path);
		break;
	case FileSystem::Format::MODEL:
		ret = model->Load(path);
		break;
	case FileSystem::Format::META: {
		char* f_uid = App->dummy_file_system->GetData(path);
		if (f_uid == nullptr) {
			LOG("Failed to open meta file, trying to reload asset...");
			std::string s_path(path);
			s_path.erase(s_path.end() - 5, s_path.end());
			Import(s_path.c_str());
			f_uid = App->dummy_file_system->GetData(path);
			if (f_uid == nullptr) {
				LOG("Failed another time to load meta file, aborting loading");
				return false;
			}
		}
		uint64_t uid = 0u;
		memcpy(&uid, f_uid, sizeof(uint64_t));

		delete[] f_uid;

		if (uid != 0u) {
			std::string s_path(path);
			s_path.erase(s_path.end() - 5, s_path.end());
			switch (App->dummy_file_system->GetFormat(s_path.c_str())) {
			case FileSystem::Format::FBX:
				if (App->dummy_file_system->Exists((MODEL_L_FOLDER + std::to_string(uid) + ".whispModel").c_str()) == false) {
					LOG("Model referenced in meta does not exists, recreating from .meta...");
					model->Import(s_path.c_str());
				}
				else {
					ret = model->Load((MODEL_L_FOLDER + std::to_string(uid) + ".whispModel").c_str());
				}
				break;
			case FileSystem::Format::JPG:
			case FileSystem::Format::PNG:
			case FileSystem::Format::DDS:
				if (App->dummy_file_system->Exists((MATERIAL_L_FOLDER + std::to_string(uid) + ".dds").c_str()) == false) {
					LOG("Texture referenced in meta does not exists, recreating from .meta...");
					std::string s_path(path);
					s_path.erase(s_path.end() - 5, s_path.end());
					material->Load(s_path.c_str());
				}
				else {
					ret = material->Load((MATERIAL_L_FOLDER + std::to_string(uid) + ".dds").c_str());
				}
				break;
			}
		}
	}
		break;
	default:
		LOG("Failed to load %s. Format not setted", path);
		break;
	}
	return ret;
}

void ModuleImport::CreateLibrary()
{
	if (App->dummy_file_system->Exists(LIBRARY_FOLDER) == false) {
		LOG("Missing Library, generating...");
		App->dummy_file_system->CreateDir(LIBRARY_FOLDER);
	}

	CreateFiles(ASSETS_FOLDER);
}

void ModuleImport::CreateFiles(const char* directory)
{
	for (const auto & entry : std::experimental::filesystem::directory_iterator(directory)) {
		if (std::experimental::filesystem::is_directory(entry)) {
			CreateFiles(entry.path().u8string().c_str());
		}
		else {
			std::string path = entry.path().u8string();
			if (App->dummy_file_system->IsFileSupported(path.c_str())) {
				if (App->dummy_file_system->HasMeta(path.c_str())) {
					if (App->dummy_file_system->IsMetaVaild((path + ".meta").c_str()) == false) {
						App->importer->Import(path.c_str());
					}
				}
				else {
					App->importer->Import(path.c_str());
				}
			}
		}
	}
}
