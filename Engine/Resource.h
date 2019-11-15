#ifndef __RESOURCE_H__
#define __RESOURCE_H__

#include "ModuleImporter.h"
#include <string>

class Resource
{
public:

	friend ModuleImporter;

	enum class ResourceType
	{
		MODEL,
		MESH,
		MATERIAL,
		TEXTURE,
		NO_TYPE
	};

public:

	Resource(ResourceType type) : type(type){}

	virtual ~Resource() {}

	std::string & GetName() { return name; }

	ResourceType GetType() const { return type; }

	UID GetUID() const { return uid; }

	std::string GetOriginalFilePath() const { return original_file; }

	std::string GetExportedName() const { return exported_file;	}

	virtual void Save(Config& config) {};
	virtual void Load(const Config& config) {};
	
	// release memory for loaded data, not resource abstract data
	virtual void Unload() {};

protected:

	// Data Info -------------------------

	UID				uid = 0; // we use id as exported resource file too
	ResourceType		type = ResourceType::NO_TYPE; // and with type, we know how to online load this resource

	std::string		original_file; // Assets/..bla bla bla
	std::string		name; // custom name based on import names from model
	std::string		exported_file; // /Library/<custom type>/bla bla

	//uint loaded_times = 0; // stores attempts to load
	
};

#endif // !__DATA_H__