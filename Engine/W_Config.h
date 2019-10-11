#ifndef __PANEL_CONFIG_H__
#define __PANEL_CONFIG_H__

#include <string>
#include <vector>

#include "Globals.h"
#include "SDL/include/SDL_scancode.h"
#include "Window.h"

class W_Config : public Window
{
public:

	W_Config(std::string name, bool active);

	virtual ~W_Config();

	void Draw();

	void AddFpsLog(const float fps, const float ms);

	void AddMemoryLog(const float mem);

	void AddInputLog(uint key, KEY_STATE state, int flag);

private:

	ImGuiTextBuffer input_buffer;
	std::vector<float> stored_fps;
	std::vector<float> stored_ms;
	std::vector<float> stored_mem;
};

#endif // __PANEL_CONFIG_H__