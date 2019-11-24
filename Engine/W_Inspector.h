#ifndef _W_INSPECTOR_H__
#define _W_INSPECTOR_H__

#include "Window.h"

class GameObject;
class SelectedObject;

class W_Inspector : public Window
{
public:

	W_Inspector(std::string name, bool active) : Window(name, active) {};

	void Draw();

private:

	void DrawGameObjectInfo();

	void DrawStaticPopUp();

	void DrawResourceInfo();

};

#endif // !_W_INSPECTOR_H__

