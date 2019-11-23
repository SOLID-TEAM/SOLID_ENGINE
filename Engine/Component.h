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
	NO_TYPE
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
	int				flags = 0; 			// for imgui draw

protected:

	bool			active = true;
	ComponentType   type = ComponentType::NO_TYPE;
	GameObject*		linked_go = nullptr;
};


#endif // !_COMPONENT_H__
