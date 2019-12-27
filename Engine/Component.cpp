#include "Component.h"
#include "GameObject.h"
#include "ModuleImporter.h"
#include "ModuleScene.h"

Component::Component(GameObject* go, ComponentType type) : linked_go(go), type(type)
{
	header_flags |= ImGuiTreeNodeFlags_DefaultOpen;
	header_flags |= ImGuiTreeNodeFlags_AllowItemOverlap;
}

void Component::Reset()
{
}

void Component::Enable()
{
	if (!enable)
		enable = !enable;
}

void Component::Disable()
{
	if (enable)
		enable = !enable;
}


bool Component::Update()
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