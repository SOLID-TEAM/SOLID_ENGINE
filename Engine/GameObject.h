#ifndef _GAMEOBJECT_H__
#define _GAMEOBJECT_H__

#include <vector>
#include <string>

#include "Component.h"
#include "external/MathGeoLib/include/Geometry/AABB.h"

class W_Inspector;
class D_Mesh;
class C_Transform;
class ModuleScene;
class ModuleEditor;
class KDTree;

class GameObject
{
public:

	friend ModuleScene;
	friend ModuleEditor;
	friend KDTree;

public:

	GameObject(std::string name = "Unknown", GameObject* parent = nullptr);

	~GameObject();

	// Common functions ---------------------------------------------

	void SetActive(bool active);

	const char* GetName() const;

	// Virtual functions -------------------------------------------

	virtual void  Start();
		    
	virtual void  Update(float dt);
		     
	virtual void  Render();
		     
	virtual void  CleanUp();

	// Components Funtions -------------------------------------------

	Component* CreateComponent(ComponentType type);

	Component* GetComponentsByType(ComponentType type);

	const std::vector<Component*>& GetComponents() const;

	D_Mesh* GetMeshes();

	// Bounding Box Functions --------------------------------------

	void GetHierarchyAABB(math::AABB& aabb); 

private:

	void  DoStart();

	void  DoUpdate(float dt);

	void  DoRender();

	void  DoCleanUp();

	// Bounding Box Functions --------------------------------------

	void GenerateHierarchyAABB(GameObject* go, math::AABB* aabb);

	void CleanUpRecursive(GameObject* go);
	
	// Hierarchy functions -----------------------------------------

	void AddChild(GameObject* child); // Addchild currently is used to hierarchical changes

	void RemoveChild(GameObject* child);

	bool SearchParentRecursive(GameObject* parent, GameObject* parent_match);

	// 

	UID GetUID() const { return uid;  };

public:

	std::vector<GameObject*> childs;

	GameObject* parent = nullptr;

	C_Transform* transform = nullptr;

	math::AABB bounding_box;

private:

	UID uid = 0;

	bool active = true;

	std::string name;

	std::vector<Component*> components;

	friend W_Inspector;
};

#endif // !_GAMEOBJECT_H__

//bool Draw();