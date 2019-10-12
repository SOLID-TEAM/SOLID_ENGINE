#ifndef _W_RENDERING_H__
#define _W_RENDERING_H__

#include "Window.h"

class W_Rendering : public Window
{
public:

	W_Rendering(std::string name, bool active) : Window(name, active) {};

	void Draw();
};

#endif // !_W_RENDERING_H__
