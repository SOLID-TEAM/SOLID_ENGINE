#ifndef _GAMEOBJECT_H__
#define _GAMEOBJECT_H__

#include <vector>
#include <string>

class Components;

class GameObject
{
public:
	GameObject();
	~GameObject();

private:
	std::string name;
	std::vector<Components> components;
};

#endif // !_GAMEOBJECT_H__

