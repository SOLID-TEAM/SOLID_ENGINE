#include "GameObject.h"
#include "Component.h"
#include "W_Inspector.h"

#include "C_Transform.h"
#include "C_Mesh.h"
#include "C_Material.h"
#include "C_MeshRenderer.h"

#include "D_Mesh.h"

//GameObject::GameObject() 
//{
//	// adds a component transform by default
//	components.push_back(CreateComponent(ComponentType::TRANSFORM));
//}

GameObject::GameObject(std::string name, GameObject* parent) : name(name), parent(parent)
{
	// Adds a component transform ---------------------
	transform = (C_Transform*)CreateComponent(ComponentType::TRANSFORM);

	// Add to its parent childs ----------------------
	if (parent != nullptr)
		parent->childs.push_back(this);
}

GameObject::~GameObject()
{
}

bool GameObject::CleanUp()
{
	std::vector<Component*>::iterator compo = components.begin();

	for (; compo != components.end(); ++compo)
	{
		(*compo)->CleanUp();
	}

	return true;
}

bool GameObject::Enable()
{
	active = true;
	return true;
}

bool GameObject::Disable()
{
	active = false;
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


bool GameObject::Render()
{
	bool ret = true;

	std::vector<Component*>::iterator component = components.begin();

	for (; component != components.end(); ++component)
	{
		ret = (*component)->Render();
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
		components.push_back(new_component = new C_Transform(this));
		break;
	}
	case ComponentType::MESH:
	{
		components.push_back(new_component = new C_Mesh(this));
		break;
	}
	case ComponentType::MATERIAL:

		components.push_back(new_component = new C_Material(this));
		break;

	case ComponentType::MESH_RENDERER:
		components.push_back(new_component = new C_MeshRenderer(this));
		break;

	default:
		break;
	}


	return new_component;
}

Component* GameObject::GetComponentsByType(ComponentType type) 
{
	std::vector<Component*>::iterator component = components.begin();

	for (; component != components.end(); ++component)
	{
		if ((*component)->type == type) return  (*component);
	}

	return nullptr;
}

const char* GameObject::GetName() const
{
	return name.c_str();
}

const std::vector<Component*>& GameObject::GetComponents() const
{
	return components;
}

D_Mesh* GameObject::GetMeshes()
{
	std::vector<Component*>::iterator component = components.begin();

	for (; component != components.end(); ++component)
	{
		if ((*component)->type == ComponentType::MESH)
		{
			C_Mesh* cm = dynamic_cast<C_Mesh*>(*component);

			return cm->data;
		}
	}

	return nullptr;
}

void GameObject::GetBoundingBox(math::AABB& aabb)
{
	C_Mesh* c_mesh = (C_Mesh*)GetComponentsByType(ComponentType::MESH);

	if (c_mesh)
	{
		aabb = c_mesh->data->aabb;
	}
	else
	{
		aabb.maxPoint = transform->position;
		aabb.minPoint = transform->position;
	}

	GenerateGlobalBoundingBox(this, &aabb);
}


void GameObject::GenerateGlobalBoundingBox( GameObject* go, math::AABB* aabb)
{
	for (std::vector<GameObject*>::iterator child = go->childs.begin() ;  child != go->childs.end(); ++child)
	{
		GenerateGlobalBoundingBox( *child, aabb);
		
		C_Mesh* c_mesh = (C_Mesh*) (*child)->GetComponentsByType(ComponentType::MESH);

		if (c_mesh != nullptr)
		{
			aabb->maxPoint = aabb->maxPoint.Max(c_mesh->data->aabb.maxPoint);
			aabb->minPoint = aabb->minPoint.Min(c_mesh->data->aabb.minPoint);
		}
		else
		{
			aabb->maxPoint = aabb->maxPoint.Max((*child)->transform->position);
			aabb->minPoint = aabb->minPoint.Min((*child)->transform->position);
		}
	}
}

//bool GameObject::Draw()
//{
//	bool ret = true;
//
//	std::vector<Component*>::iterator component = components.begin();
//
//	for (; component != components.end(); ++component)
//	{
//		ret = (*component)->Draw();
//	}
//
//	return ret;
//}