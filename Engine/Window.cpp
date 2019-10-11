#include "Application.h"
#include "Window.h"
#include "ModuleEditor.h"
#include "ImGui/imgui.h"

Window::Window(std::string name, bool active) : name(name), active(active)
{
	App->editor->AddWindow(this);
}

Window::~Window()
{}

void Window::SwitchActive()
{
	active = !active;
}

bool Window::IsActive() const
{
	return active;
}

void Window::Destroy()
{
	App->editor->DestroyWindow(this);
}

std::vector<SDL_Scancode> Window::GetShortCut() const
{
	return shortcut;
}
