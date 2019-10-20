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
};

#endif // !_C_MATERIAL_H__
