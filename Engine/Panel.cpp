#include "Panel.h"
#include "ModuleEditor.h"

Panel::Panel(std::string name) : name(name)
{}

Panel::~Panel()
{}

void Panel::SwitchActive()
{
	active = !active;
}

bool Panel::IsActive() const
{
	return active;
}

std::vector<SDL_Scancode> Panel::GetShortCut() const
{
	return shortcut;
}
