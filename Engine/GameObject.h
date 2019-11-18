#ifndef _GAMEOBJECT_H__
#define _GAMEOBJECT_H__

#include <vector>
#include <string>

#include "external/MathGeoLib/include/Geometry/AABB.h"
#include "external/MathGeoLib/include/Geometry/OBB.h"

// Components ------------------------------ // Needed to dynamic cast functions

#include "Component.h"
#include "C_Transform.h"
#include "C_Mesh.h"
#include "C_Material.h"
#include "C_MeshRenderer.h"
#include "C_Camera.h"

#include <typeinfo>
#include <list>

class ModuleScene;
class ModuleEditor;
class W_Inspector;
class D_Mesh; // Remove
class KDTree;
class DynTree;
class DynTreeNode;

class GameObject;

class GameObject
{
public:

	friend W_Inspector;
	friend ModuleScene;
	friend ModuleEditor;
	friend KDTree;
	friend DynTree;

	friend	C_Transform;
	friend 	C_Mesh;
	friend 	C_Material;
	friend 	C_MeshRenderer;
	friend 	C_Camera;

public:

	GameObject(std::string name = "Unknown", GameObject* parent = nullptr);

	~GameObject();

	// Common functions ---------------------------------------------

	void SetActive(bool active);

	void SetIsStatic(bool value);

	const char* GetName() const;

	// Virtual functions -------------------------------------------

	virtual void  Start();
		    
	virtual void  Update(float dt);
		     
	virtual void  Render();
		     
	virtual void  CleanUp();

	// SERIALIZE ME
	bool Save(Config& config);

	// Components Funtions -------------------------------------------

	template <class T>
	T* CreateComponent();
	template <class T>
	T* GetComponent() const;

	const std::vector<Component*>& GetComponents() const;

	D_Mesh* GetMeshes();

	// Bounding Box Functions --------------------------------------

	AABB GetHierarchyAABB();

private:

	void  DoStart();

	void  DoUpdate(float dt);

	void  DoRender();

	void  DoCleanUp();

	// Hierarchy functions -----------------------------------------

	void AddChild(GameObject* child); // Addchild currently is used to hierarchical changes

	void RemoveChild(GameObject* child);

	bool SearchParentRecursive(GameObject* parent, GameObject* parent_match);

	void CleanUpRecursive(GameObject* go);

	// 

	UID GetUID() const { return uid;  };

public:

	std::vector<GameObject*> childs;

	GameObject* parent = nullptr;

	C_Transform* transform = nullptr;

private:

	std::string name;

	UID uid = 0;

	bool is_static = false;

	bool active = true;

	bool ignore = false;

	std::vector<Component*> components;

	math::AABB bounding_box;

	OBB obb;

	DynTreeNode* dyn_node = nullptr;

};

// Template functions -----------------------------------------------------

template<class T>
T* GameObject::GetComponent() const
{
	for (auto& component : components)
	{
		if (T* c = dynamic_cast<T*>(component))
			return c;
	}

	return nullptr;
}
template<class T>
T* GameObject::CreateComponent()
{
	T* new_component = nullptr;

	if (typeid(C_Transform) == typeid(T))
		components.push_back(new_component = (T*) new C_Transform(this));
	else if (typeid(C_Mesh) == typeid(T))
		components.push_back(new_component = (T*)new C_Mesh(this));
	else if (typeid(C_MeshRenderer) == typeid(T))
		components.push_back(new_component = (T*)new C_MeshRenderer(this));
	else if (typeid(C_Material) == typeid(T))
		components.push_back(new_component = (T*)new C_Material(this));
	else if (typeid(C_Camera) == typeid(T))
		components.push_back(new_component = (T*)new C_Camera(this));

	return new_component;
}

#endif // !_GAMEOBJECT_H__