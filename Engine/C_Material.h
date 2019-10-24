#ifndef _C_MATERIAL_H__
#define _C_MATERIAL_H__

#include "Component.h"

class D_Material;

class C_Material : public Component
{
public:

	C_Material(GameObject* go);

	bool DrawPanelInfo();

public:

	D_Material* data = nullptr;
	bool textured = true;
	bool view_checker = false;
	uint checker_gl_id = 0;

};

#endif // !_C_MATERIAL_H__