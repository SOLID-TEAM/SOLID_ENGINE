#ifndef __PANEL_CONSOLE_H__
#define __PANEL_CONSOLE_H__

#include "Globals.h"
#include <string>
#include <vector>
#include "Panel.h"

#include "ImGui/imgui.h"

class PanelConsole : public Panel
{
public:

	PanelConsole(std::string name);

	virtual ~PanelConsole();

	void AddConsoleLog(const char* new_entry);

	void Draw();

private:

	ImVector<char*> console_log;
};

#endif // __PANEL_CONSOLE_H__
