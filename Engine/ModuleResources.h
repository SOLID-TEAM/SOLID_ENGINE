#ifndef __MODULERESOURCES_H__
#define __MODULERESOURCES_H__

#include "Globals.h"
#include "Module.h"
#include "Resource.h"
#include "PathNode.h"

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

	std::map<UID, Resource*> GetAllMapResources() const { return resources; };

	void ImportFileDropped(const char* file);
	Resource::Type GetResourceTypeFromFileExtension(std::string extension);
	std::string GetRelativePathToWriteFromType(Resource::Type type) const;

	void LoadAllMetaResources();
private:
	
	void GetMetasFromNodes(PathNode node, std::vector<std::string>& metas);
	void LoadDependencies(Resource* resource);

	void SaveLastUID();
	void LoadLastUID();


public:
	UID Find(const char* file_in_assets) const;
	UID ImportFile(const char* new_file_in_assets, Resource::Type type , std::string path, bool force = false);
	UID GenerateNewUID();
	const Resource* Get(UID uid) const;
	Resource* Get(UID uid);
	Resource* CreateNewResource(Resource::Type type, UID force_uid = 0);
	bool CreateNewMetaData(const char* new_file_in_assets, UID uid);
private:

	std::map<UID, Resource*> resources;

	UID last_uid = 1;
};

#endif // !__MODULERESOURCES_H__