#ifndef _W_HIERARCHY_H__
#define _W_HIERARCHY_H__

#include "Window.h"

class GameObject;

class W_Hierarchy : public Window
{
public:
	W_Hierarchy(std::string name, bool active);

	void Draw();
	void DrawAll(GameObject* go);

private:

	GameObject* selected_go = nullptr;

};

#endif // !_W_HIERARCHY_H__

