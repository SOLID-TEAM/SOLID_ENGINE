#ifndef _C_MESHRENDERER_H__
#define _C_MESHRENDERER_H__

#include "Component.h"

class D_Mesh;
class D_Material;

class C_MeshRenderer : public Component
{
public:

	friend ModuleImporter;

public:

	C_MeshRenderer(GameObject* parent);

	~C_MeshRenderer();

	bool PostUpdate(float dt);

private:

	bool Render();

	bool RenderWireframe();

	bool RenderOutline();

private:

	D_Mesh*     d_mesh = nullptr;
	D_Material* d_mat = nullptr;

};

#endif // !_C_MESHRENDERER_H__
