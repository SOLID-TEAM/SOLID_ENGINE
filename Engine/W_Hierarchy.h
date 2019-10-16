#ifndef _W_HIERARCHY_H__
#define _W_HIERARCHY_H__

#include "Window.h"

class W_Hierarchy : public Window
{
public:
	W_Hierarchy(std::string name, bool active);
	void Draw();

	void DrawAll(GameObject* go);

private:

	// test
	GameObject* selected_go = nullptr;
	GameObject* hovered_go = nullptr;
};

#endif // !_W_HIERARCHY_H__

