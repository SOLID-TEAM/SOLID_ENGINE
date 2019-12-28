#ifndef _COMPONENT_H__
#define _COMPONENT_H__


#include "W_Inspector.h"
#include "Config.h"

#include <string>

enum class ComponentType
{
	TRANSFORM,
	MESH,
	MATERIAL,
	CAMERA,
	LIGHT,
	MESH_RENDERER,
	BOX_COLLIDER,
	SPHERE_COLLIDER,
	CAPSULE_COLLIDER,
	CONVEX_HULL_COLLIDER,
	RIGID_BODY,
	CHARACTER_CONTROLLER,
	JOINTP2P,
	NO_TYPE // last
};

class ModuleScene;
class ModuleImporter;
class GameObject;

class Component
{
public :

	friend ModuleScene;
	friend ModuleImporter;
	friend W_Inspector;
	friend GameObject;

public:

	Component(GameObject* go, ComponentType type);

	virtual ~Component() {};

	virtual void Reset();

	virtual void Enable();

	virtual void Disable();

	virtual bool Update();

	virtual void UpdateTransform();

	virtual bool Render();

	virtual bool DrawPanelInfo(); // draw something on the editor panels loop (components) called from editor

	virtual bool CleanUp();

	virtual bool Save(Config& config) { return true; };
	virtual bool Load(Config& config) { return true; };

public:

	// Editor ----------------------------------------------------
	std::string		name;
	bool			collapsed = false; // save individual for each gameobject its prev state
	int				header_flags = 0; 			// for imgui draw

protected:

	bool			enable = true;
	ComponentType   type = ComponentType::NO_TYPE;
	GameObject*		linked_go = nullptr;

	bool			enable_button = false;
	bool            remove_button = false;
};


#endif // !_COMPONENT_H__
