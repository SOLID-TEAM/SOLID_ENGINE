#ifndef _W_SCENE_H__
#define _W_SCENE_H__

#include "Window.h"

class W_Scene: public Window
{
public:

	W_Scene(std::string name, bool active) : Window(name, active) {};

	void Draw();

};

#endif // !_W_SCENE_H__

