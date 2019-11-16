#include "Component.h"
#include "GameObject.h"
#include "ModuleImporter.h"
#include "ModuleScene.h"

Component::Component(GameObject* go, ComponentType type) : linked_go(go), type(type) {}

void Component::Reset()
{
}

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

void Component::UpdateTransform()
{
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