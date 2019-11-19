#ifndef _C_MESH_H__
#define _C_MESH_H__

#include "Component.h"

class R_Mesh;

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

	// Resource functions ---------------------

	bool SetMeshResource(UID resource);

	void DeleteMeshResource();

	bool Save(Config& config);

	bool Load(Config& config);

public:

	UID resource = 0;
	AABB	bounding_box;

};

#endif // !_C_MESH_H__
