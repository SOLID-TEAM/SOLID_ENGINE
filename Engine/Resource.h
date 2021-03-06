#ifndef __RESOURCE_H__
#define __RESOURCE_H__

#include "ModuleImporter.h"
#include "Config.h"

#include <string>

class Resource
{
public:

	friend ModuleImporter;

	enum class Type
	{
		MODEL,
		MESH,
		MATERIAL,
		TEXTURE,
		NO_TYPE
	};

public:

	Resource(UID uid, Type type) : uid(uid), type(type) { loaded = 0u;  }

	virtual ~Resource() {}

	const char* GetCharName();

	std::string & GetName() { return name; }

	Resource::Type GetType() const { return type; }

	UID GetUID() const { return uid; }

	std::string& GetOriginalFile() { return original_file; }

	std::string& GetExportedFile() { return exported_file;	}

	std::string GetNameFromUID() const;

	bool IsLoadedToMemory() const;
	bool LoadToMemory();
	uint CountReferences() const;

	virtual void Save(Config& config) const;
	virtual void Load(const Config& config);

	void Release();
	
protected:
	// release memory for loaded data, not resource abstract data
	virtual void ReleaseFromMem() = 0;
	virtual bool LoadInMemory() = 0;

protected:

	// Data Info -------------------------
	uint loaded = 0u;
	UID				uid = 0; // we use id as exported resource file too
	Type		type = Type::NO_TYPE; // and with type, we know how to online load this resource

	// USED only for information, not needed at data level
	std::string		original_file; // Assets/..bla bla bla
	std::string		name; // custom name based on import names from model
	std::string		exported_file; // /Library/<custom type>/bla bla

	//uint loaded_times = 0; // stores attempts to load
	
};

#endif // !__DATA_H__