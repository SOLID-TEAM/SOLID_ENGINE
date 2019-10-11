#ifndef _COMPONENT_H__
#define _COMPONENT_H__

class GameObject;

class Component
{
public:
	Component();
	virtual void Enable();
	virtual void Disable();

private:

	GameObject* game_object = nullptr;
};


#endif // !_COMPONENT_H__
