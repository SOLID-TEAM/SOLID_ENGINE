#ifndef _W_RENDERING_H__
#define _W_RENDERING_H__

#include "Window.h"

class W_Rendering : public Window
{
public:

	W_Rendering(std::string name, bool active) : Window(name, active) {};

	void Draw();

public:
	// TODO: read TODO of moduleinput from where we listen dropable events
	uint checker_tex_gl_id = 0;
};

#endif // !_W_RENDERING_H__
