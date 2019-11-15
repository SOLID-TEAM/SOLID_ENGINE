#ifndef _C_MESH_H__
#define _C_MESH_H__

#include "Component.h"

class D_Mesh;
class C_MeshRenderer;
class C_Transform;

class C_Mesh : public Component
{
public:

	friend ModuleImporter;
	friend C_MeshRenderer;
	friend C_Transform;

public:

	C_Mesh(GameObject* parent);

	~C_Mesh();

	bool Update(float dt);

	bool DrawPanelInfo();

	bool CleanUp();

	// Resource functions ---------------------

	void SetMeshResource(D_Mesh* resource);

	void DeleteMeshResource();

	bool Save(Config& config);

	//bool Load(Config& config);

	D_Mesh* data = nullptr;

private:

	AABB	mesh_aabb;

};

#endif // !_C_MESH_H__
