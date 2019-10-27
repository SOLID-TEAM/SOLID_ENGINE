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

	bool Render();

private:

	bool RenderMesh(const uint custom_tex = 0, bool textured = true);

	bool RenderWireframe();

	bool RenderOutline();

private:

	D_Mesh*     d_mesh = nullptr;
	D_Material* d_material = nullptr;

};

#endif // !_C_MESHRENDERER_H__
