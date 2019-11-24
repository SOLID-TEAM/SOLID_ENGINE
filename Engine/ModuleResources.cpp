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
	// TODO: load last_uid;
	//LoadAllMetaResources();
	std::string path(LIBRARY_SETTINGS_FOLDER + std::string("lastuid"));
	Config uid(path.c_str());
	last_uid = uid.GetInt("Last_UID", 1);

	LoadAllMetaResources();
	
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

UID ModuleResources::ImportFile(const char* new_file_in_assets, Resource::Type type, std::string from_path, bool force)
{
	UID ret = 0;
	bool import_ok = false;

	if (force)
	{
		ret = Find(new_file_in_assets);
		if (ret > 0)
			return ret;
	}

	switch (type)
	{
	case Resource::Type::MODEL:
		import_ok = R_Model::Import(new_file_in_assets, ret, from_path);
		break;
	case Resource::Type::TEXTURE:
		import_ok = App->textures->Import(new_file_in_assets, ret, from_path);//, ret);
		break;
	case Resource::Type::NO_TYPE:
		break;
	default:
		break;
	}

	if (import_ok)
	{
		Resource* r = CreateNewResource(type, ret);
		std::string p, f, e;
		App->file_sys->SplitFilePath(new_file_in_assets, &p, &f, &e);

		ret = r->GetUID();
		r->GetOriginalFile().assign(new_file_in_assets);
		r->GetName().assign(f + "." +e);
		r->GetExportedFile().assign(GetRelativePathToWriteFromType(type) + std::to_string(ret));
		
		// create metadata
		CreateNewMetaData(new_file_in_assets, ret);
	}
	else
	{
		LOG("[Error:%s] Importing %s", name.c_str() ,new_file_in_assets);
	}

	return ret;
}

bool ModuleResources::CreateNewMetaData(const char* file_in_assets, UID uid)
{
	bool ret = true;

	std::string path, file, extension;
	App->file_sys->SplitFilePath(file_in_assets, &path, &file, &extension);
	
	// TODO: binary?
	Config new_meta;

	new_meta.AddInt("guid", uid);
	new_meta.AddInt("Type", (int)GetResourceTypeFromFileExtension(extension));

	std::string final_path(path + file + "." + extension + ".meta");
	new_meta.SaveConfigToFile(final_path.c_str());

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
		ret = (Resource*) new R_Material(uid);
		break;
	case Resource::Type::TEXTURE:
		ret = (Resource*) new R_Texture(uid);
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
	last_uid++;
	SaveLastUID();
	return last_uid;
}

UID ModuleResources::Find(const char* file_in_assets) const
{
	std::string file = App->file_sys->NormalizePath(file_in_assets);

	for (std::map<UID, Resource*>::const_iterator it = resources.begin(); it != resources.end(); ++it)
	{
		if (it->second->GetOriginalFile().compare(file) == 0)
		{
			LOG("[Info] Resource duplicated, returning loaded resource %s", it->second->GetExportedFile().c_str());
			return it->first;
		}
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

std::vector<Resource*> ModuleResources::GetAllResourcesByType(Resource::Type type)
{
	std::vector<Resource*> to_return;

	for (auto const& [key, resource] : resources)
	{
		if (resource->GetType() == type)  to_return.push_back(resource);
	}

	return to_return;
}

// TODO: add functionality to import directories
void ModuleResources::ImportFileDropped(const char* file)
{
	std::string full_path = App->file_sys->NormalizePath(file);
	std::string extension;
	std::string filename;
	std::string path;
	
	App->file_sys->SplitFilePath(full_path.c_str(), &path, &filename, &extension);

	filename += "." + extension;

	Resource::Type type = GetResourceTypeFromFileExtension(extension);

	// if dropped file is recognized
	if (type != Resource::Type::NO_TYPE)
	{
		LOG("[Info:%s] Recognized file extension .%s", name.c_str() ,extension.c_str());

		// duplicate file to our assets folder
		// TODO: if file exists this overwrites, maybe we don't need this
		std::string final_path = GetRelativePathToWriteFromType(type);

		// TODO: new function to duplicate files, currently if the destination is the same of source, names conflict and kabum
		if (App->file_sys->DuplicateFile(file, final_path.c_str() , std::string("/")))
		{
			LOG("[Info:%s] Dropped file duplicated to %s", name.c_str(), (final_path + filename).c_str() );

			bool force_find = false;
			if (type == Resource::Type::TEXTURE || type == Resource::Type::MODEL) // TODO: temporal force find, todo re-import asset
				force_find = true;

			UID uid = ImportFile((final_path + filename).c_str(), type, path, force_find);
			
			// TODO: testing creating gameobjects from model
			if(type == Resource::Type::MODEL)
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
std::string ModuleResources::GetRelativePathToWriteFromType(Resource::Type type) const
{
	std::string ret;

	switch (type)
	{
	case Resource::Type::MODEL:
		ret.assign(ASSETS_MODEL_FOLDER);
		break;
	case Resource::Type::MESH:
		ret.assign(ASSETS_FOLDER);
		break;
	case Resource::Type::MATERIAL:
		ret.assign(ASSETS_FOLDER);
		break;
	case Resource::Type::TEXTURE:
		ret.assign(ASSETS_TEXTURES_FOLDER);
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
		extension == "tif" ||
		extension == "tga")
	{
		ret = Resource::Type::TEXTURE;
	}

	return ret;
}

void ModuleResources::LoadAllMetaResources()
{
	std::vector<std::string> filter_ext;
	filter_ext.push_back(std::string("meta"));
	PathNode all_nodes = App->file_sys->GetAllFiles(ASSETS_FOLDER, &filter_ext);

	std::vector<std::string> all_metas;
	GetMetasFromNodes(all_nodes, all_metas);

	for (uint i = 0; i < all_metas.size(); ++i)
	{
		Config meta(all_metas[i].c_str());

		UID resource_uid = meta.GetInt("guid", 0);
		Resource::Type type = (Resource::Type)meta.GetInt("Type", (int)Resource::Type::NO_TYPE);
		
		// TODO: check if the resource still exists on library folders

		//
		Resource* r = CreateNewResource(type, resource_uid);

		if (r != nullptr)
		{
			std::string path, file;
			App->file_sys->SplitFilePath(all_metas[i].c_str(), &path, &file);
			r->GetOriginalFile().assign(path + file);
			r->GetName().assign(file);
			r->GetExportedFile().assign(GetRelativePathToWriteFromType(type) + std::to_string(resource_uid));

			LoadDependencies(r);
		}

	}
	
}

void ModuleResources::GetMetasFromNodes(PathNode node, std::vector<std::string>& meta_vector)
{
	if (node.file)
	{
		std::string p, f, e;
		App->file_sys->SplitFilePath(node.path.c_str(), &p, &f, &e);
		
		std::string output;
		// scan all path to find double /
		// TODO: improve this
		for (uint i = 0; i < p.length(); ++i)
		{
			if (p[i] != '/')
				output += p[i];
			else
			{
				if (i < p.length())
				{
					if (p[i + 1] == '/')
					{
						output += '/';
					}
				}
			}
		}
		// check if the last character has or not /
		if (output.back() != '/')
			output.push_back('/');

		std::string clean_path = output + f + "." + e;
		meta_vector.push_back(clean_path);
	}
		

	for (uint i = 0; i < node.children.size(); ++i)
	{
		GetMetasFromNodes(node.children[i], meta_vector);
	}

}

// TODO: only resource model has dependencies for now
// UPDATE when needed
void ModuleResources::LoadDependencies(Resource* resource)
{
	if (resource->GetType() == Resource::Type::MODEL)
	{
		R_Model* model = static_cast<R_Model*>(resource);
		model->LoadDependencies();
	}
}

// TODO: adapt with filesystem buffer save
void ModuleResources::SaveLastUID()
{
	Config uid;

	uid.AddInt("Last_UID", last_uid);

	std::string final_path(LIBRARY_SETTINGS_FOLDER + std::string("lastuid"));
	uid.SaveConfigToFile(final_path.c_str());

}