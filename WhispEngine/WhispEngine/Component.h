#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include "Globals.h"

#pragma once

class GameObject;

enum ComponentType
{
	NONE = -1,
	TRANSFORM,
	MESH,
	MATERIAL,
	LIGHT
};

class Component
{
public:
	Component(GameObject* parent, ComponentType type);
	virtual ~Component();

	virtual void Update();
	virtual void Init();

	virtual void Enable();
	virtual void Disable();

	virtual void OnInspector() = 0;

	const bool IsActive() const;
	const ComponentType GetType() const;


public:
	GameObject* parent = nullptr;

private:
	ComponentType type = ComponentType::NONE;

protected:
	bool is_active = false;
};
#endif // __COMPONENT_H__
