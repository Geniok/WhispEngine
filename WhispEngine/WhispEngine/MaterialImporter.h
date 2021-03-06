#pragma once
#include "Importer.h"
#include "SDL/include/SDL_config.h"
#include "Assimp/include/material.h"
#include "Globals.h"

struct Texture;

class MaterialImporter :
	public Importer
{
public:
	MaterialImporter();
	~MaterialImporter();


	uint64 Import(const char* path, const aiMaterial* material = nullptr, const uint64 &force_uid = 0u);
};

