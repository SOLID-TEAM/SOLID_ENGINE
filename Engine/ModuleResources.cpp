#include "ModuleResources.h"
#include "Application.h"
#include "Event.h"

#include "R_Model.h"
#include "R_Mesh.h"
#include "R_Material.h"

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

	switch (type)
	{
	case Resource::Type::MODEL:
		import_ok = R_Model::Import(new_file_in_assets, ret);
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
		Resource* r = CreateNewResource(type, ret);
		r->GetName().assign(new_file_in_assets);
		//r->GetExportedName().assign(written_file.c_str());
		ret = r->GetUID();
		LOG("");
	}
	else
	{
		LOG("[Error:%s] Importing %s", name.c_str() ,new_file_in_assets);
	}

	return ret;
}

Resource* ModuleResources::CreateNewResource(Resource::Type type, UID force_uid)
{
	Resource* ret = nullptr;
	UID uid = 0;

	if (force_uid > 0)
		uid = force_uid;
	else
		uid = GenerateNewUID();

	switch (type)
	{
	case Resource::Type::MODEL:
		ret = (Resource*) new R_Model(uid);
		break;
	case Resource::Type::MESH:
		ret = (Resource*) new R_Mesh(uid);
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

void ModuleResources::ReceiveEvent(const Event& e)
{

	switch (e.type)
	{
	case Event::file_dropped:
		LOG("[Info:%s] New file dropped %s", name.c_str() ,e.string.ptr);
		ImportFileDropped(e.string.ptr);
		break;
	}

}

// TODO: add functionality to import directories
void ModuleResources::ImportFileDropped(const char* file)
{
	std::string full_path = App->file_sys->NormalizePath(file);
	std::string extension;
	std::string filename;
	
	App->file_sys->SplitFilePath(full_path.c_str(), nullptr, &filename, &extension);

	filename += "." + extension;

	Resource::Type type = GetResourceTypeFromFileExtension(extension);

	// if dropped file is recognized
	if (type != Resource::Type::NO_TYPE)
	{
		LOG("[Info:%s] Recognized file extension .%s", name.c_str() ,extension.c_str());

		// duplicate file to our assets folder
		// TODO: if file exists this overwrites, maybe we don't need this
		std::string final_path = GetRelativePathToWriteFromType(type, full_path);

		if (App->file_sys->DuplicateFile(file, final_path.c_str() , std::string("/")))
		{
			LOG("[Info:%s] Dropped file duplicated to %s", name.c_str(), (final_path + filename).c_str() );

			UID uid = ImportFile((final_path + filename).c_str(), type);
			
			// TODO: testing creating gameobjects from model
			App->scene->CreateGameObjectFromModel(uid);

		}
		else
		{
			LOG("[Error:%s] Dropped file cannot be duplicated to %s", name.c_str(), final_path.c_str());
		}

	}
	else
	{
		LOG("[Error:%s] Dropped file can't be imported", name.c_str());
		LOG("[Error:%s] Path: %s", name.c_str() ,file);
		LOG("[Error:%s] Extension: %s", name.c_str() ,extension.c_str());
	}

}

// TODO: get every specific folder
std::string ModuleResources::GetRelativePathToWriteFromType(Resource::Type type, std::string filename) const
{
	std::string ret;

	switch (type)
	{
	case Resource::Type::MODEL:
		ret.assign(ASSETS_FOLDER);
		break;
	case Resource::Type::MESH:
		ret.assign(ASSETS_FOLDER);
		break;
	case Resource::Type::MATERIAL:
		ret.assign(ASSETS_FOLDER);
		break;
	case Resource::Type::TEXTURE:
		ret.assign(ASSETS_FOLDER);
		break;
	case Resource::Type::NO_TYPE:
		break;
	default:
		break;
	}

	return ret;
}

Resource::Type ModuleResources::GetResourceTypeFromFileExtension(std::string extension)
{

	if (extension.empty()) return Resource::Type::NO_TYPE;

	Resource::Type ret = Resource::Type::NO_TYPE;

	for (uint i = 0; i < strlen(extension.c_str()); ++i)
		extension[i] = std::tolower(extension[i]);

	if (extension == "fbx" ||
		extension == "obj" )
	{
		ret = Resource::Type::MODEL;
	}
	if (extension == "png" ||
		extension == "jpg" ||
		extension == "dds" ||
		extension == "tif")
	{
		ret = Resource::Type::TEXTURE;
	}

	return ret;
}