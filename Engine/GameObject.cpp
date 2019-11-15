#include "GameObject.h"
#include "Component.h"
#include "Application.h"
#include "W_Inspector.h"
#include "ModuleScene.h"

#include "D_Mesh.h"
#include "KDTree.h"

GameObject::GameObject(std::string name, GameObject* parent) : name(name), parent(parent)
{
	uid = App->random->Int();

	// Adds a component transform ---------------------
	transform = CreateComponent<C_Transform>();

	// Add to its parent childs -----------------------
	if (parent != nullptr)
		parent->childs.push_back(this);

	// Standard Bounding Box --------------------------

	bounding_box.SetFromCenterAndSize( float3::zero , math::float3(10.f, 10.f, 10.f));
	obb = bounding_box.ToOBB();
}

GameObject::~GameObject()
{
}

// User functions ------------------------------------

void GameObject::CleanUp(){}

void GameObject::Start(){}

void GameObject::Update(float dt){}

void GameObject::Render(){}

// Fixed functions ------------------------------------

void GameObject::DoStart()
{
	Start();
}

void GameObject::DoUpdate(float dt)
{
	Update(dt);

	std::vector<Component*>::iterator component = components.begin();

	for (; component != components.end(); ++component)
	{
		(*component)->Update(dt);
	}
}

void GameObject::DoRender()
{
	Render();

	std::vector<Component*>::iterator component = components.begin();

	for (; component != components.end(); ++component)
	{
		(*component)->Render();
	}
}

void GameObject::DoCleanUp()
{
	CleanUp();

	CleanUpRecursive(this);
}

void GameObject::CleanUpRecursive(GameObject* go)
{
	// clean all components of this pointing go

	std::vector<Component*>::iterator component = go->components.begin();

	for (; component != go->components.end(); ++component)
	{
		(*component)->CleanUp();
	}


	// for each children

	std::vector<GameObject*>::iterator child = go->childs.begin();

	for (; child != go->childs.end(); ++child)
	{
		CleanUpRecursive((*child));
	}
}

void GameObject::SetActive(bool active)
{
	this->active = active;
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

AABB GameObject::GetHierarchyAABB()
{
	AABB hierarchy_aabb;
	hierarchy_aabb.SetNegativeInfinity();

	std::stack<GameObject*> go_stack;

	go_stack.push(this);

	while (!go_stack.empty())
	{
		GameObject* go = go_stack.top();

		hierarchy_aabb.Enclose(go->obb);

		go_stack.pop();

		for (GameObject* child : go->childs)
		{
			go_stack.push(child);
		}
	}

	return hierarchy_aabb;

}

bool GameObject::SearchParentRecursive(GameObject* parent, GameObject* parent_match)
{
	bool match = false;

	if (parent == parent_match)
	{
		match = true;
	}
	else if(parent->parent != nullptr)
		match = SearchParentRecursive(parent->parent, parent_match);

	return match;
}


void GameObject::AddChild(GameObject* new_child)
{
	if (new_child->parent == this)
		return;

	// search all
	bool match = SearchParentRecursive(this, new_child);

	if (match)
	{
		//LOG("NEW CHILD IS ON PARENT TREE");
		// if new child is on parent tree, we must to relink all new_child->childs->parent to new_child->parent
		std::vector<GameObject*>::iterator it = new_child->childs.begin();
		for (; it != new_child->childs.end();)
		{
			LOG(" ------------------------------- ");
			LOG("[Info] Re-linked %s to %s", (*it)->GetName(), new_child->parent->GetName());
			(*it)->parent = new_child->parent;
			// adds to parent childs
			new_child->parent->childs.push_back((*it));
			// and remove all new_child->childs
			it = new_child->childs.erase(it);
		}
	}
	/*else
		LOG("NEW CHILD NOT ON PARENT TREE");*/

	// remove new_child from new_child->parent
	new_child->parent->RemoveChild(new_child);
	new_child->parent = this;

	// adds new child to this gameobject childs
	childs.push_back(new_child);

	LOG(" ------------------------------- ");
	LOG("[Info] Moved %s to %s", new_child->GetName(), this->GetName());
	LOG(" ------------------------------- ");

}


// removes child but not deletes
void GameObject::RemoveChild(GameObject* child)
{
	std::vector<GameObject*>::iterator it = std::find(childs.begin(), childs.end(), child);

	if (it != childs.end())
	{
		LOG("[Info] I %s i lost treat with my child %s", this->GetName(), child->GetName());
		childs.erase(it);
	}
	else
	{
		LOG("[Info] I never known about %s child");
		//return childs.end();
	}

}

bool GameObject::Save(Config& config)
{
	Config myConfig;
	// save my needed values and my components

	myConfig.AddInt("UID", uid);
	if (parent) myConfig.AddInt("Parent UID", parent->uid);
	myConfig.AddString("name", name.c_str());
	myConfig.AddBool("Active", active);

	// components
	myConfig.AddArray("Components");
	for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); ++it)
	{
		Config component;
		component.AddInt("Type", (int)(*it)->type);
		component.AddString("name", (*it)->name.c_str());
		(*it)->Save(component);
		myConfig.AddArrayEntry(component);
	}


	config.AddArrayEntry(myConfig);

	return true;
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