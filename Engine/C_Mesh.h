#ifndef _C_MESH_H__
#define _C_MESH_H__

#include "Component.h"

class D_Mesh;

class C_Mesh : public Component
{
public:

	friend ModuleImporter;

public:

	C_Mesh(GameObject* parent);
	~C_Mesh();

	bool Update(float dt);
	bool DrawPanelInfo();
	bool CleanUp();

public:

	D_Mesh* data = nullptr;
	AABB	bounding_box;

};

#endif // !_C_MESH_H__
