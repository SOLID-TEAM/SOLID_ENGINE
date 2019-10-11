#ifndef __PANEL_CONSOLE_H__
#define __PANEL_CONSOLE_H__

#include "Globals.h"
#include <string>
#include <vector>
#include "Window.h"

#include "ImGui/imgui.h"

class W_Console : public Window
{
public:

	W_Console(std::string name, bool active);

	virtual ~W_Console();

	void AddConsoleLog(const char* new_entry);

	void Draw();

private:

	ImVector<char*> console_log;
};

#endif // __PANEL_CONSOLE_H__
