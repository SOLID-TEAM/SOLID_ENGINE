#include "Globals.h"
#include "Application.h"
#include "ModuleImporter.h"


ModuleImporter::ModuleImporter(bool start_enabled) : Module(start_enabled)
{
}

ModuleImporter::~ModuleImporter()
{

}

bool ModuleImporter::Init(Config& config)
{
	return true;
}

bool ModuleImporter::Start(Config& config)
{
	return true;
}

update_status ModuleImporter::Update(float dt)
{
	return UPDATE_CONTINUE;
}

bool ModuleImporter::CleanUp()
{
	return true;
}
