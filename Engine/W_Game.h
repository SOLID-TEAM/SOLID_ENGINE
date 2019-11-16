#ifndef _W_GAME_H__
#define _W_GAME_H__

#include "Window.h"

class W_Game : public Window
{
public:

	W_Game(std::string name, bool active) : Window(name, active) {};

	void Draw();

};

#endif // !_W_GAME_H__

