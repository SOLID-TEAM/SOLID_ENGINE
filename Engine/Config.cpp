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

Config Config::GetSection(const char* section) const
{
	return Config(json_object_get_object(root_object, section));
}

// utils
