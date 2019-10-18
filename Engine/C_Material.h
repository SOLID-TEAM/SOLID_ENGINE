#ifndef _C_MATERIAL_H__
#define _C_MATERIAL_H__

#include "Component.h"

class GameObject;

class ComponentMaterial : public Component
{
public:

	ComponentMaterial(GameObject* go);

	//bool PreUpdate(float dt);

	//bool Update(float dt);

	//bool PostUpdate(float dt);

	//bool Draw(); // draw something on viewport (components) called from gameobjects draw (last game loop)

	//bool DrawPanelInfo(); // draw something on the editor panels loop (components) called from editor

};

#endif // !_C_MATERIAL_H__
