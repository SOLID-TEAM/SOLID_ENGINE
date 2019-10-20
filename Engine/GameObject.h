#ifndef _GAMEOBJECT_H__
#define _GAMEOBJECT_H__

#include <vector>
#include <string>

#include "Component.h"

class W_Inspector;
class D_Mesh;

class GameObject
{
public:

	GameObject(std::string name = "no_name", GameObject* parent = nullptr);

	~GameObject();

	bool Enable();

	bool Disable();

	bool Update(float dt);

	bool PostUpdate(float dt);

	const char* GetName() const;

	bool CleanUp();

	// Components Funtions -------------------------------------------

	Component* CreateComponent(ComponentType type);

	Component* GetComponentsByType(ComponentType type);

	const std::vector<Component*>& GetComponents() const;

	D_Mesh* GetMeshes();

public:

	std::vector<GameObject*> childs;

	GameObject* parent = nullptr;

private:

	bool active = true;

	std::string name;

	std::vector<Component*> components;

	friend W_Inspector;
};

#endif // !_GAMEOBJECT_H__

//bool Draw();