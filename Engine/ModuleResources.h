#ifndef __MODULERESOURCES_H__
#define __MODULERESOURCES_H__

#include "Globals.h"
#include "Module.h"
#include "Resource.h"

#include <map>
#include <string>

class ModuleResources : public Module
{
public:

	ModuleResources();
	~ModuleResources();

	bool Init(Config& config);
	bool Start(Config& config);
	bool CleanUp();

	bool Save(Config& config);
	void Load(Config& config);

	void ReceiveEvent(const Event& e);

	void ImportFileDropped(const char* file);
	Resource::Type GetResourceTypeFromFileExtension(std::string extension);


public:
	UID Find(const char* file_in_assets) const;
	UID ImportFile(const char* new_file_in_assets, Resource::Type type ,bool force = false);
	UID GenerateNewUID();
	const Resource* Get(UID uid) const;
	Resource* Get(UID uid);
	Resource* CreateNewResource(Resource::Type type, UID force_uid = 0);
private:

	std::map<UID, Resource*> resources;

	UID last_uid = 0;
};

#endif // !__MODULERESOURCES_H__