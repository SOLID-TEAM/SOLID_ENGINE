#include "Globals.h"
#include "Config.h"

Config::Config()
{
	root_value = json_value_init_object();
	root_object = json_value_get_object(root_value);
}

Config::Config(JSON_Object* section) : root_object(section) {}

Config::Config(const char* path)
{
	if (path != nullptr)
	{
		if ((root_value = json_parse_file(path)) == NULL)
		{
			LOG("[Error] Configuration file %s not found", path);
		}
		else
		{
			LOG("[Init] Loaded configuration file %s", path);
			root_object = json_value_get_object(root_value);
		}
	}
}

bool Config::CreateNewRootObject(const char* filename)
{
	root_value = json_value_init_object();
	root_object = json_value_get_object(root_value);

	return root_object == nullptr ? false : true;
}

bool Config::IsOk() const
{
	return root_object != nullptr;
}

Config Config::GetSection(const char* section) const
{
	return Config(json_object_get_object(root_object, section));
}

Config Config::AddSection(const char* section)
{
	json_object_set_value(root_object, section, json_value_init_object());
	return GetSection(section);
}

void Config::SaveConfigToFile(const char* path)
{
	// write data to file
	json_serialize_to_file_pretty(root_value, path);
}

// utils
// ADD ----------------------

bool Config::AddBool(const char* name, bool value)
{
	return !json_object_set_boolean(root_object, name, value);
}

bool Config::AddString(const char* name, const char* string)
{
	return !json_object_set_string(root_object, name, string);
}

bool Config::AddInt(const char* name, int value)
{
	return !json_object_set_number(root_object, name, (double)value);
}

// GET -------------------------

bool Config::GetBool(const char* name) const
{
	return json_object_get_boolean(root_object, name);
}

const char* Config::GetString(const char* name) const
{
	return json_object_get_string(root_object, name);
}

int Config::GetInt(const char* name) const
{
	return (int)json_object_get_number(root_object, name);
}