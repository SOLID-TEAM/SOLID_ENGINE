#ifndef _C_MESH_H__
#define _C_MESH_H__

#include "Component.h"
#include "external/MathGeoLib/include/MathGeoLib.h"


class C_MeshRenderer;
class C_Transform;
class C_Collider;
class C_BoxCollider;
class R_Mesh;


class C_Mesh : public Component
{
public:

	friend ModuleImporter;
	friend C_MeshRenderer;
	friend C_Transform;
	friend C_Collider;
	friend C_BoxCollider;

public:

	C_Mesh(GameObject* parent);

	~C_Mesh();

	bool Update();

	bool DrawPanelInfo();

	bool CleanUp();

	// Resource functions ---------------------

	bool SetMeshResource(UID resource);

	UID GetResource();

	void DeleteMeshResource();

	bool Save(Config& config);

	bool Load(Config& config);

private:

	AABB	mesh_aabb;
	UID		resource = 0;

};

#endif // !_C_MESH_H__
