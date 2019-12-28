#ifndef _GAMEOBJECT_H__
#define _GAMEOBJECT_H__

#include <vector>
#include <string>
#include <map>

#include "external/MathGeoLib/include/Geometry/AABB.h"
#include "external/MathGeoLib/include/Geometry/OBB.h"

// Components ------------------------------ // Needed to dynamic cast functions

#include "Component.h"
#include "C_Transform.h"
#include "C_Mesh.h"
#include "C_Material.h"
#include "C_MeshRenderer.h"
#include "C_Camera.h"
#include "C_Collider.h"
#include "C_BoxCollider.h"
#include "C_SphereCollider.h"
#include "C_CapsuleCollider.h"
#include "C_ConvexHullCollider.h"
#include "C_RigidBody.h"
#include "C_JointP2P.h"
#include "C_CharacterController.h"

#include <typeinfo>
#include <list>

class ModuleScene;
class ModuleEditor;
class W_Inspector;
class R_Mesh; // Remove
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
	friend	C_Collider;
	friend  C_RigidBody;
	friend  C_JointP2P;

public:

	GameObject(std::string name, GameObject* parent , bool is_static);

	~GameObject();

	// Common functions ---------------------------------------------

	void SetActive(bool active);

	void SetIsStatic(bool value);

	void SetIsStaticToHierarchy(bool value);

	const char* GetName() const;

	// Virtual functions -------------------------------------------

	virtual void  Start();
		    
	virtual void  Update();
		     
	virtual void  Render();

	virtual void OnCollisionEnter() {}

	virtual void OnCollision() {}

	virtual void OnCollisionExit() {}

	virtual void  CleanUp();

	// SERIALIZE ME
	bool Save(Config& config);
	bool Load(Config& config, std::map<GameObject*, uint>& relations);

	// Components Funtions -------------------------------------------

	template <class T>
	T* AddComponent();
	template <class T>
	T* GetComponent() const;

	const std::vector<Component*>& GetComponents() const;

	// Bounding Box Functions --------------------------------------

	AABB GetHierarchyAABB();

	AABB GetAABB();

	OBB GetOBB();

	void RemoveChild(GameObject* child);

	bool IsActive() const { return active; };

private:

	void  DoStart();

	void  DoUpdate();

	void  DoRender();

	void  DoCleanUp();

	// Hierarchy functions -----------------------------------------

	void AddChild(GameObject* child); // Addchild currently is used to hierarchical changes

	void SetNewParent(GameObject* parent);

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

	bool ignore_culling = false;

	std::vector<Component*> components;

	math::AABB bounding_box;

	math::OBB obb;

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
T* GameObject::AddComponent()
{
	T* new_component = nullptr;

	if (typeid(C_Transform) == typeid(T))
		new_component = (T*) new C_Transform(this);
	else if (typeid(C_Mesh) == typeid(T))
		new_component = (T*)new C_Mesh(this);
	else if (typeid(C_MeshRenderer) == typeid(T))
		new_component = (T*)new C_MeshRenderer(this);
	else if (typeid(C_Material) == typeid(T))
		new_component = (T*)new C_Material(this);
	else if (typeid(C_Camera) == typeid(T))
		new_component = (T*)new C_Camera(this);
	else if (typeid(C_BoxCollider) == typeid(T))
		new_component = (T*)new C_BoxCollider(this);
	else if (typeid(C_SphereCollider) == typeid(T))
		new_component = (T*)new C_SphereCollider(this);
	else if (typeid(C_CapsuleCollider) == typeid(T))
		new_component = (T*)new C_CapsuleCollider(this);
	else if (typeid(C_ConvexHullCollider) == typeid(T))
		new_component = (T*)new C_ConvexHullCollider(this);
	else if (typeid(C_RigidBody) == typeid(T))
		new_component = (T*)new C_RigidBody(this);
	else if (typeid(C_JointP2P) == typeid(T))
		new_component = (T*)new C_JointP2P(this);
	else if (typeid(C_CharacterController) == typeid(T))
		new_component = (T*)new C_CharacterController(this);


	if (new_component != nullptr)
		components.push_back(new_component);

	return new_component;
}

#endif // !_GAMEOBJECT_H__