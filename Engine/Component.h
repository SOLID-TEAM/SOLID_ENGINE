#ifndef _COMPONENT_H__
#define _COMPONENT_H__


enum class ComponentType
{
	TRANSFORM,
	MESH,
	MATERIAL,
	LIGHT,
};

class GameObject;

class Component
{
public:
	Component(GameObject* go);
	virtual void Enable();
	virtual void Disable();

	virtual bool PreUpdate(float dt);
	virtual bool Update(float dt);
	virtual bool PostUpdate(float dt);
	virtual bool Draw();
	virtual bool InspectorDraw() { return true; };

	virtual bool CleanUp();

private:
	bool active = true;
	GameObject* linked_go = nullptr;
};


#endif // !_COMPONENT_H__
