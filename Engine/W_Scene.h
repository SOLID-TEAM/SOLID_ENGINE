#ifndef _W_SCENE_H__
#define _W_SCENE_H__

#include "Window.h"

struct ImVec2;

class W_Scene: public Window
{
public:

	W_Scene(std::string name, bool active) : Window(name, active) {};

	void Draw();

private:

	void GridMenu();

	void DebugMenu();

	void CameraMenu();

	void ModesMenu();

private:

	ImVec2 viewport_size = { 0.f, 0.f };

	

};

#endif // !_W_SCENE_H__

