#include "Component.h"
#include "GameObject.h"

Component::Component(GameObject* go, ComponentType type) : linked_go(go), type(type) {}

void Component::Enable()
{
	if (!active)
		active = !active;
}

void Component::Disable()
{
	if (active)
		active = !active;
}

bool Component::Update(float dt)
{
	return true;
}

bool Component::Render()
{
	return false;
}

bool Component::DrawPanelInfo()
{
	return true;
}

bool Component::CleanUp()
{
	return true;
}