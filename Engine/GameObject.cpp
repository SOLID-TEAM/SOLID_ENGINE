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
	// recursively delete its childs and components
	CleanUpRecursive(this);

	// currently does nothing
	transform->CleanUp();

	return true;
}

void GameObject::CleanUpRecursive(GameObject* go)
{
	// clean all components of this pointing go
	
	std::vector<Component*>::iterator compoIt = go->components.begin();
	for (; compoIt != go->components.end(); ++compoIt)
	{
		(*compoIt)->CleanUp();
	}

	// currently does nothing
	transform->CleanUp();
	
	// for each children
	for (std::vector<GameObject*>::iterator it = go->childs.begin(); it != go->childs.end(); ++it)
		CleanUpRecursive((*it));

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
		OBB obb = aabb.Transform(this->transform->global_transform);
		aabb = obb.MinimalEnclosingAABB();
	}
	else
	{ 
		aabb.maxPoint = transform->GetWorldPosition();
		aabb.minPoint = transform->GetWorldPosition();
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
			AABB global_aabb = c_mesh->data->aabb;
			OBB obb = global_aabb.Transform((*child)->transform->global_transform);
			global_aabb = obb.MinimalEnclosingAABB();

			aabb->maxPoint = aabb->maxPoint.Max(global_aabb.maxPoint);
			aabb->minPoint = aabb->minPoint.Min(global_aabb.minPoint);
		}
		else
		{
			aabb->maxPoint = aabb->maxPoint.Max((*child)->transform->GetWorldPosition());
			aabb->minPoint = aabb->minPoint.Min((*child)->transform->GetWorldPosition());
		}
	}
}

void GameObject::AddChild(GameObject* child)
{
	childs.push_back(child);
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