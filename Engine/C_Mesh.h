#ifndef _C_MESH_H__
#define _C_MESH_H__

#include "Component.h"
#include "external/MathGeoLib/include/MathGeoLib.h"


class C_MeshRenderer;
class C_Transform;
class R_Mesh;


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
