#pragma once
#include "Color.h"
#include "MathGeoLib/include/Math/float3.h"
#include "glew-2.1.0/include/GL/glew.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#pragma comment (lib, "glew-2.1.0/libx86/glew32.lib")

struct Light
{
	Light();

	void Init();
	void SetPos(float x, float y, float z);
	void Active(bool active);
	void Render();

	Color ambient;
	Color diffuse;
	math::float3 position;

	int ref;
	bool on;
};