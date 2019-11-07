#include "MeshImporter.h"
#include "Application.h"
#include "Assimp/include/scene.h"
#include "Globals.h"

MeshImporter::MeshImporter()
{
}


MeshImporter::~MeshImporter()
{
}

bool MeshImporter::Import(const uint64_t &uid, const aiMesh* mesh)
{
	/*length_name | num_vertex | num_index | num_face_normals | num_vertex_normals | num_tex_normals | �AABB?

	name | vertex | index | face_normals | vertex_normals | tex_normals*/

	std::string name(mesh->mName.C_Str());
	uint header[] = { name.length(), mesh->mNumVertices, mesh->mNumFaces * 3, mesh->mNumFaces * 2, mesh->mNumVertices, mesh->mNumVertices };

	uint size = 
		sizeof(header) +								//header
		name.length() +									//name
		sizeof(float) * mesh->mNumVertices * 3 + 		//vertex
		sizeof(uint) * mesh->mNumFaces * 9 + 			//index
		sizeof(float) * mesh->mNumFaces * 2 * 3 * 3 + 	//face_normals
		sizeof(float) * mesh->mNumVertices * 3 + 		//vertex_normals
		sizeof(float) * mesh->mNumVertices * 3;			//tex_normals

	char* data = new char[size];
	memset(data, 0, size);
	char* cursor = data;

	uint bytes = sizeof(header);
	memcpy(cursor, header, bytes);
	cursor += bytes;
	bytes = name.length();
	memcpy(cursor, name.c_str(), bytes);

	cursor += bytes;
	bytes = sizeof(float) * mesh->mNumVertices * 3;
	memcpy(cursor, (float*)mesh->mVertices, bytes);

	if (mesh->HasFaces()) {
		cursor += bytes;

		for (uint i = 0; i < mesh->mNumFaces; ++i)
		{
			if (mesh->mFaces[i].mNumIndices != 3)
			{
				LOG("WARNING, geometry face with != 3 indices!");
				cursor[i * 3]	  = 0;
				cursor[i * 3 + 1] = 0;
				cursor[i * 3 + 2] = 0;
			}
			else
			{
				memcpy(&cursor[i*3], mesh->mFaces[i].mIndices, sizeof(uint) * 3);
			}
		}
		bytes = sizeof(uint) * mesh->mNumFaces * 9;

		cursor += bytes;
		bytes = sizeof(float) * mesh->mNumFaces * 2 * 3 * 3;
		//memcpy(cursor, face_normals, bytes); TODO: Calculate face normals
		memset(cursor, NULL, bytes);
	}
	else LOG("Mesh has not faces");

	if (mesh->HasNormals()) {
		cursor += bytes;
		bytes = sizeof(float) * mesh->mNumVertices * 3;
		memcpy(cursor, (float*)mesh->mNormals, bytes);
	}
	else LOG("Mesh has not normals");

	if (mesh->HasTextureCoords(0)) {
		cursor += bytes;
		bytes = sizeof(float) * mesh->mNumVertices * 3;
		memcpy(cursor, (float*)mesh->mTextureCoords[0], bytes);
	}
	else LOG("Mesh has not texture coords");
	
	App->file_system->SaveData(data, std::string(MESH_LFOLDER + std::to_string(uid) + ".whispMesh").data(), size);
	delete[] data;

	return true;
}

