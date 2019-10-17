#ifndef _W_INSPECTOR_H__
#define _W_INSPECTOR_H__

#include "Window.h"

class GameObject;

class W_Inspector : public Window
{
public:

	W_Inspector(std::string name, bool active) : Window(name, active) {};

	void Draw();

};

#endif // !_W_INSPECTOR_H__

