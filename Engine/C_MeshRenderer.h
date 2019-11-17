#ifndef _C_MESHRENDERER_H__
#define _C_MESHRENDERER_H__

#include "Component.h"

class R_Mesh;
class R_Material;

class C_MeshRenderer : public Component
{
public:

	friend ModuleImporter;

public:

	C_MeshRenderer(GameObject* parent);

	~C_MeshRenderer();

	bool Render();

private:

	void RenderMesh(float* color, const uint custom_tex = 0, bool textured = true);

	void RenderWireframe(float width, float* color);

	void RenderOutline(float width, float* color);

	void RenderBoundingBox(math::AABB& aabb, float width, float* color);

private:

	R_Mesh* r_mesh = nullptr;
	/*UID mesh_uid;
	UID material_uid;*/

};

#endif // !_C_MESHRENDERER_H__
