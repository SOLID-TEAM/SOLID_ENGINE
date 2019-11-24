#ifndef _W_PROJECT_H__
#define _W_PROJECT_H__

#include "Window.h"
#include <vector>
#include "ModuleResources.h"

class Resource;

class W_Project : public Window
{
public:

	W_Project(std::string name, bool active);

	void Draw();

private:

	void DrawTree();
	void DrawFolder();

private:

	Resource::Type resource_type = Resource::Type::NO_TYPE;
	std::vector<Resource*> visible_resources;
	float folder_column_width = 0.f;

};

#endif // !_W_PROJECT_H__

