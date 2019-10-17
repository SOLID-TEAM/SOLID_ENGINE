#ifndef _COMPONENT_H__
#define _COMPONENT_H__

#include <string>

enum class ComponentType
{
	TRANSFORM,
	MESH,
	MATERIAL,
	LIGHT,
};

class GameObject;

class Component
{
public:
	Component(GameObject* go);
	virtual void Enable();
	virtual void Disable();

	virtual bool PreUpdate(float dt);
	virtual bool Update(float dt);
	virtual bool PostUpdate(float dt);

	virtual bool Draw(); // draw something on viewport (components) called from gameobjects draw (last game loop)
	virtual bool DrawPanelInfo(); // draw something on the editor panels loop (components) called from editor

	virtual bool CleanUp();
public:
	std::string name;
	// for imgui draw
	int flags = 0;
	bool collapsed = false; // save individual for each gameobject its prev state
private:
	bool active = true;
	GameObject* linked_go = nullptr;
};


#endif // !_COMPONENT_H__
