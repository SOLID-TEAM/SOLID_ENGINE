#ifndef _GAMEOBJECT_H__
#define _GAMEOBJECT_H__

#include <vector>
#include <string>

#include "Component.h"

class W_Inspector;

class GameObject
{
public:
	//GameObject();
	//GameObject(std::string name);
	GameObject(std::string name = "no_name", GameObject* parent = nullptr);
	~GameObject();

	bool Disable();
	bool Enable();
	bool Update(float dt);
	bool PostUpdate(float dt);
	bool Draw();

	const char* GetName() const;
	const std::vector<Component*>& GetComponents() const;

	bool CleanUp();

	Component* CreateComponent(ComponentType type);
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

