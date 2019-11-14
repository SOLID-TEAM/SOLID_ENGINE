#ifndef __DATA_H__
#define __DATA_H__

#include "ModuleImporter.h"
#include <string>

class Data
{
public:

	friend ModuleImporter;

	enum class DataType
	{
		MESH,
		MATERIAL,
		TEXTURE,
		NO_TYPE
	};

public:

	Data(DataType type) : type(type){}

	virtual ~Data() {}

	std::string & GetName() { return name; }

	DataType GetType() const { return type; }

	UID GetUID() const { return uid; }

	std::string GetOriginalFilePath() const { return original_file; }

	std::string GetExportedName() const { return exported_file;	}

	virtual void Save(Config& config) {};
	virtual void Load(const Config& config) {};
	
	// release memory for loaded data, not resource abstract data
	virtual void Unload() {};

protected:

	// Data Info -------------------------

	UID				uid = 0;
	DataType		type = DataType::NO_TYPE;

	std::string		original_file; // Assets/..bla bla bla
	std::string		name; // custom name based on import names from model
	std::string		exported_file; // /Library/<custom type>/bla bla

	//uint loaded_times = 0; // stores attempts to load
	
};

#endif // !__DATA_H__