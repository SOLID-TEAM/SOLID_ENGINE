#ifndef _C_MATERIAL_H__
#define _C_MATERIAL_H__

#include "Component.h"

class R_Material;

class C_Material : public Component
{
public:

	C_Material(GameObject* go);

	bool CleanUp();
	bool DrawPanelInfo();

	bool Save(Config& config);
	bool Load(Config& config);

	bool SetMaterialResource(UID uid);

public:

	UID resource = 0;
	bool textured = false;
	bool view_checker = false;
	uint checker_gl_id = 0;

};

#endif // !_C_MATERIAL_H__
