#ifndef __DATA_H__
#define __DATA_H__

#include "ModuleImporter.h"
#include <string>

typedef unsigned long long ulong;

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

	std::string & GetName()
	{
		return name;
	}

	DataType GetType() const
	{
		return type;
	}

	ulong GetID() const
	{
		return ID;
	}

	std::string GetOriginalFilePath() const
	{
		return original_file_path;
	}

	std::string GetLibraryFilePath() const
	{
		return library_file_path;
	}

	std::string GetExportedName() const
	{
		return exported_name;
	}

	uint GetFileTexture() const
	{
		return file_texture;
	}

	virtual void GenerateFileTexture() {}

	virtual void Load() {};
	virtual void Unload() {};

protected:

	// Data Info -------------------------

	ulong			ID = 0;
	DataType		type = DataType::NO_TYPE;
	std::string		name = "";
	std::string		library_file_path = "";
	std::string		original_file_path = "";
	std::string		exported_name;
	uint			file_texture = 0;
};

#endif // !__DATA_H__