#ifndef __MODULE_IMPORTER_H__
#define __MODULE_IMPORTER_H__

#include "Globals.h"
#include "Module.h"

class ModuleImporter : public Module
{
public:
	ModuleImporter(bool start_enabled = true);
	~ModuleImporter();

	bool Init(Config& config);
	bool Start(Config& config);
	update_status Update(float dt);
	bool CleanUp();
};

#endif // !__MODULE_IMPORTER_H__
