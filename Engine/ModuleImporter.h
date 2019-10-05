#ifndef __MODULE_IMPORTER_H__
#define __MODULE_IMPORTER_H__

#include "Globals.h"
#include "Module.h"

#include "external/Assimp/include/cimport.h"
#include "external/Assimp/include/scene.h"
#include "external/Assimp/include/postprocess.h"
#include "external/Assimp/include/cfileio.h"

#pragma comment (lib, "external/Assimp/libx86/assimp.lib")

#include "ModelData.h"

class ModuleImporter : public Module
{
public:
	ModuleImporter(bool start_enabled = true);
	~ModuleImporter();

	bool Init(Config& config);
	bool Start(Config& config);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	bool LoadFileMesh(const char* path);

private:
	struct aiLogStream stream;

	// TODO: for testing here, but we need to search a better place, maybe new module coming
	std::vector<ModelData*> meshes;
	//std::vector<ModelData*> startup_meshes;
};

#endif // !__MODULE_IMPORTER_H__
