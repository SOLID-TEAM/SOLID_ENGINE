#pragma once

#include "Component.h"
#include "ModelData.h"

class ComponentMesh : public Component
{
public:
	ComponentMesh(GameObject* parent);
	~ComponentMesh();

	bool Update(float dt);
	bool PostUpdate(float dt);
	bool Draw();
	bool CleanUp();

public:
	// TODO: for now only one mesh for gameObject
	ModelData* mesh = nullptr;
};