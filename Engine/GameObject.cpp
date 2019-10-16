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

bool GameObject::Draw()
{
	bool ret = true;

	std::vector<Component*>::iterator component = components.begin();

	for (; component != components.end(); ++component)
	{
		ret = (*component)->Draw();
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
	// delete components
	std::vector<Component*>::iterator compo = components.begin();

	for (; compo != components.end(); ++compo)
	{
		(*compo)->CleanUp();
	}
	
	/*std::vector<GameObject*>::iterator it = childs.begin();
	for (; it != childs.end(); ++it)
	{
		(*it)->CleanUp();
	}*/

	return true;
}

const char* GameObject::GetName() const
{
	return name.c_str();
}