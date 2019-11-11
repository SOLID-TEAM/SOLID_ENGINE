#ifndef _COMPONENT_H__
#define _COMPONENT_H__

#include "ModuleImporter.h"
#include "W_Inspector.h"

#include <string>

enum class ComponentType
{
	TRANSFORM,
	MESH,
	MATERIAL,
	CAMERA,
	LIGHT,
	MESH_RENDERER,
	NO_TYPE
};

class GameObject;

class Component
{
public :

	friend ModuleImporter;
	friend W_Inspector;
	friend GameObject;

public:

	Component(GameObject* go, ComponentType type);

	virtual void Reset();

	virtual void Enable();

	virtual void Disable();

	virtual bool Update(float dt);

	virtual void UpdateTransform();

	virtual bool Render();

	virtual bool DrawPanelInfo(); // draw something on the editor panels loop (components) called from editor

	virtual bool CleanUp();

public:

	// Editor ----------------------------------------------------
	std::string		name;
	bool			collapsed = false; // save individual for each gameobject its prev state
	int				flags = 0; 			// for imgui draw

protected:

	bool			active = true;
	ComponentType   type = ComponentType::NO_TYPE;
	GameObject*		linked_go = nullptr;
};


#endif // !_COMPONENT_H__
