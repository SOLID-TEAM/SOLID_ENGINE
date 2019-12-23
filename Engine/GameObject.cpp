#include "GameObject.h"
#include "Component.h"
#include "Application.h"
#include "W_Inspector.h"
#include "ModuleScene.h"

#include "R_Mesh.h"
#include "KDTree.h"
#include "DynTree.h"

#include "Viewport.h" //TODO, remove this when we rework how main camera links 

GameObject::GameObject(std::string name, GameObject* parent, bool is_static) : name(name), is_static(is_static) ,parent(parent)
{
	uid = App->random->Int();

	// Adds a component transform ---------------------
	transform = AddComponent<C_Transform>();

	// Add to its parent childs -----------------------
	if (parent != nullptr)
	{
		parent->childs.push_back(this);
	}

	// Standard Bounding Box --------------------------

	bounding_box.SetNegativeInfinity();
	obb.SetNegativeInfinity();
}

GameObject::~GameObject()
{
	/*if (parent != nullptr)
	{
		parent->RemoveChild(this);
	}*/
}

// User functions ------------------------------------

void GameObject::CleanUp(){}

void GameObject::Start(){}

void GameObject::Update(){}

void GameObject::Render(){}

// Fixed functions ------------------------------------

void GameObject::DoStart()
{
	Start();
}

void GameObject::DoUpdate()
{
	Update();

	for (Component* component : components)
	{
		component->Update();
	}
}

void GameObject::DoRender()
{
	Render();

	for (Component* component : components)
	{
		component->Render();
	}
}

void GameObject::DoCleanUp()
{
	CleanUpRecursive(this);
}

void GameObject::CleanUpRecursive(GameObject* go)
{
	// clean all components of this pointing go

	std::vector<Component*>::iterator component = go->components.begin();

	for (; component != go->components.end();)
	{
		(*component)->CleanUp();
		//delete_components.push((*component));
		delete *component;
		component = go->components.erase(component);
	}

	// for each children

	std::vector<GameObject*>::iterator child = go->childs.begin();
	std::queue<GameObject*> delete_forever;

	for (; child != go->childs.end();) //++child)
	{
		CleanUpRecursive((*child));
		delete_forever.push((*child));
		child = go->childs.erase(child);
	}

	while (!delete_forever.empty())
	{
		delete delete_forever.front();
		delete_forever.front() = nullptr;
		delete_forever.pop();
	}

	CleanUp();
}

void GameObject::SetActive(bool active)
{
	this->active = active;
}

void GameObject::SetIsStatic(bool value)
{
	if (is_static != value)
	{
		App->scene->PushEvent( this, (is_static == true) ? EventGoType::STATIC_TO_DYNAMIC : EventGoType::DYNAMIC_TO_STATIC );
		is_static = value;
	}
}

void GameObject::SetIsStaticToHierarchy(bool value)
{
	std::stack<GameObject*> go_stack;

	go_stack.push(this);

	while (!go_stack.empty())
	{
		GameObject* go = go_stack.top();
		go_stack.pop();

		if (go->is_static != value)
		{
			App->scene->PushEvent(go, (go->is_static == true) ? EventGoType::STATIC_TO_DYNAMIC : EventGoType::DYNAMIC_TO_STATIC);
			go->is_static = value;
		}

		for (GameObject* child : go->childs)
		{
			go_stack.push(child);
		}
	}
}

const char* GameObject::GetName() const
{
	return name.c_str();
}

const std::vector<Component*>& GameObject::GetComponents() const
{
	return components;
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

		if (go->obb.IsFinite())
		{
			hierarchy_aabb.Enclose(go->bounding_box);
		}
		else
		{
			hierarchy_aabb.Enclose(AABB::FromCenterAndSize(go->transform->position, float3::one ) );
		}

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

void GameObject::SetNewParent(GameObject* new_parent)
{
	if (new_parent == nullptr)
		return;

	if (parent != nullptr)
		parent->RemoveChild(this);
	
	parent = new_parent;
	new_parent->childs.push_back(this); // TODO: review addchild, or rename and add a simple addchild, current addchild is pointed to hierarchical moves on editor
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
		//LOG("[Info] I %s i lost treat with my child %s", this->GetName(), child->GetName());
		childs.erase(it);
	}
	else
	{
		//LOG("[Info] I never known about %s child");
		//return childs.end();
	}

}

bool GameObject::Save(Config& config)
{
	Config myConfig;
	// save my needed values and my components

	myConfig.AddInt("UID", uid);
	if (parent) myConfig.AddInt("Parent UID", parent->uid);
	myConfig.AddString("Name", name.c_str());
	myConfig.AddBool("Active", active);
	myConfig.AddBool("Static", is_static);

	// components
	myConfig.AddArray("Components");
	for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); ++it)
	{
		Config component;
		component.AddInt("Type", (int)(*it)->type);
		component.AddString("Name", (*it)->name.c_str());
		(*it)->Save(component);
		myConfig.AddArrayEntry(component);
	}

	config.AddArrayEntry(myConfig);

	return true;
}

bool GameObject::Load(Config& config, std::map<GameObject*, uint>& relationship)
{
	// set stored uuid
	uid = config.GetInt("UID", uid);
	uint parent_id = config.GetInt("Parent UID", 0);
	// store relations to next hierarchical assignment
	relationship[this] = parent_id;

	name.assign(config.GetString("Name", "noname"));
	active = config.GetBool("Active", true);
	is_static = config.GetBool("Static", true);

	// load all components
	int num_components = config.GetArrayCount("Components");

	for (uint i = 0 ; i < num_components; ++i)
	{
		Config c_config(config.GetArray("Components", i));

		ComponentType type = (ComponentType)c_config.GetInt("Type", (int)ComponentType::NO_TYPE);

		if (type != ComponentType::NO_TYPE)
		{
			Component* new_component = nullptr;
			// TODO: improve this with our template
			switch (type)
			{
			case ComponentType::TRANSFORM:
				new_component = transform;
				break;
			case ComponentType::MESH:
				new_component = AddComponent<C_Mesh>();
				break;
			case ComponentType::MATERIAL:
				new_component = AddComponent<C_Material>();
				break;
			case ComponentType::CAMERA:
			{
				// TODO: IMPROVE THIS when we get more scene cameras
				// if we are loading a new scene, the first camera found
				// always is linked to new scene camera
				if (App->scene->main_camera == nullptr)
				{
					new_component = AddComponent<C_Camera>();
					ignore_culling = true;
					App->scene->main_camera = this;
					App->scene->game_viewport->SetCamera(this);
				}
				else
				{
					new_component = AddComponent<C_Camera>();
				}
				
				break;
			}
			case ComponentType::LIGHT:
				break;
			case ComponentType::MESH_RENDERER:
				new_component = AddComponent<C_MeshRenderer>();
				break;
			case ComponentType::NO_TYPE:
				break;
			default:
				break;
			}

			if (new_component != nullptr)
			{
				new_component->Load(c_config);
			}
		}
	}

	return true;
}

