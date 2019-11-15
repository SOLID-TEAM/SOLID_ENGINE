#include "ModuleResources.h"
#include "Application.h"

ModuleResources::ModuleResources()
{
	name.assign("ModuleResources");
}

ModuleResources::~ModuleResources() {}

bool ModuleResources::Init(Config& config)
{
	return true;
}

bool ModuleResources::Start(Config& config)
{
	return true;
}

bool ModuleResources::CleanUp()
{
	for (std::map<UID, Resource*>::iterator it = resources.begin(); it != resources.end(); ++it)
		RELEASE(it->second);

	resources.clear();

	return true;
}

UID ModuleResources::ImportFile(const char* new_file_in_assets, Resource::Type type, bool force)
{
	UID ret = 0;
	bool import_ok = false;
	std::string written_file;

	switch (type)
	{
	case Resource::Type::MODEL:
		//import_ok = ResourceModel::Import(new_file_in_assets, written_file);
		break;
	case Resource::Type::TEXTURE:
		break;
	case Resource::Type::NO_TYPE:
		break;
	default:
		break;
	}

	if (import_ok)
	{
		Resource* r = CreateNewResource(type);
		r->GetName().assign(new_file_in_assets);
		r->GetExportedName().assign(written_file.c_str());
		ret = r->GetUID();
	}

	return ret;
}

Resource* ModuleResources::CreateNewResource(Resource::Type type, UID force_uid)
{
	Resource* ret = nullptr;
	UID uid = GenerateNewUID();

	switch (type)
	{
	case Resource::Type::MODEL:
		break;
	case Resource::Type::MESH:
		break;
	case Resource::Type::MATERIAL:
		break;
	case Resource::Type::TEXTURE:
		break;
	case Resource::Type::NO_TYPE:
		break;
	default:
		break;
	}

	if (ret != nullptr)
		resources[uid] = ret;

	return ret;
}

const Resource* ModuleResources::Get(UID uid) const
{
	std::map<UID, Resource*>::const_iterator it = resources.find(uid);
	if (it != resources.end())
		return it->second;

	return nullptr;
}

Resource* ModuleResources::Get(UID uid)
{
	std::map<UID, Resource*>::iterator it = resources.find(uid);
	if (it != resources.end())
		return it->second;

	return nullptr;
}

UID ModuleResources::GenerateNewUID()
{
	// TODO: save here and not with editor_config
	return last_uid++;
}

UID ModuleResources::Find(const char* file_in_assets) const
{
	std::string file = App->file_sys->NormalizePath(file_in_assets);

	for (std::map<UID, Resource*>::const_iterator it = resources.begin(); it != resources.end(); ++it)
	{
		if (it->second->GetOriginalFilePath().compare(file) == 0)
			return it->first;
	}
	return 0;
}

// TODO: save and load last uid from another place, not editor_configuration
bool ModuleResources::Save(Config& config)
{
	config.AddInt("Last_UID", last_uid);

	return true;
}

void ModuleResources::Load(Config& config)
{
	last_uid = config.GetInt("Last_UID", last_uid);
}