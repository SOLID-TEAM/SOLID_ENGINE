#include "GameObject.h"
#include "Component.h"

#include "C_Transform.h"
#include "C_Mesh.h"

//GameObject::GameObject() 
//{
//	// adds a component transform by default
//	components.push_back(CreateComponent(ComponentType::TRANSFORM));
//}

GameObject::GameObject(std::string name, GameObject* parent) : name(name), parent(parent)
{
	// adds a component transform by default
	CreateComponent(ComponentType::TRANSFORM);

	if (parent != nullptr)
		parent->childs.push_back(this);
}

GameObject::~GameObject()
{
}

bool GameObject::Enable()
{
	std::vector<Component*>::iterator component = components.begin();

	for (; component != components.end(); ++component)
	{
		(*component)->Enable();
	}

	return true;
}

bool GameObject::Disable()
{
	std::vector<Component*>::iterator component = components.begin();

	for (; component != components.end(); ++component)
	{
		(*component)->Disable();
	}

	return true;
}

bool GameObject::Update(float dt)
{
	bool ret = true;

	std::vector<Component*>::iterator component = components.begin();

	for (; component != components.end(); ++component)
	{
		ret = (*component)->Update(dt);
	}

	return ret;
}

bool GameObject::PostUpdate(float dt)
{
	bool ret = true;

	std::vector<Component*>::iterator component = components.begin();

	for (; component != components.end(); ++component)
	{
		ret = (*component)->PostUpdate(dt);
	}

	return ret;
}

Component* GameObject::CreateComponent(ComponentType type)
{
	Component* new_component = nullptr;

	switch (type)
	{
	case ComponentType::TRANSFORM:
	{
		components.push_back(new_component = new ComponentTransform(this));
		break;
	}
	case ComponentType::MESH:
	{
		components.push_back(new_component = new ComponentMesh(this));
		break;
	}
	case ComponentType::MATERIAL:
		break;
	case ComponentType::LIGHT:
		break;
	default:
		break;
	}


	return new_component;
}

bool GameObject::CleanUp()
{
	return true;
}