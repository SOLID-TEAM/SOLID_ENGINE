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

// utils ----------------------------------------------
JSON_Value* Config::FindValue(const char* var_name, int array_index) const
{
	JSON_Value* ret = nullptr;
	// returns the value, or the array index value
	if (array_index < 0)
		return ret = json_object_get_value(root_object, var_name);
	
	JSON_Array* arr = json_object_get_array(root_object, var_name);
	if (arr != nullptr)
		return ret = json_array_get_value(arr, array_index);
	
	return ret;
}
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

bool Config::AddFloatArray(const char* var_name, const float* values, const int size)
{
	bool ret = true;

	if (values != nullptr && size > 0)
	{
		JSON_Value* value = json_value_init_array();
		json_object_set_value(root_object, var_name, value);

		for (int i = 0; i < size; ++i)
			json_array_append_number(json_value_get_array(value), values[i]);
	}
	else
		ret = false;

	return ret;
}

bool Config::AddFloat(const char* var_name, const float value)
{
	return !json_object_set_number(root_object, var_name, (double)value);
}

// GET -------------------------

bool Config::GetBool(const char* name,const bool default, const int array_index) const
{
	JSON_Value* value = FindValue(name, array_index);

	if (value != NULL)
		return json_value_get_boolean(value);

	return default;
}

const char* Config::GetString(const char* name, const char* default, const int array_index) const
{
	JSON_Value* value = FindValue(name, array_index);

	if (value != NULL)
		return json_value_get_string(value);

	return default;
}

int Config::GetInt(const char* name, const int default, const int array_index) const
{
	JSON_Value* value = FindValue(name, array_index);

	if (value != NULL)
		return (int)json_value_get_number(value);

	return default;
}

float Config::GetFloat(const char* name, const float default, const float array_index) const
{
	JSON_Value* value = FindValue(name, array_index);

	if (value != NULL)
		return (float)json_value_get_number(value);

	return default;
}

bool Config::AddArrayEntry(const Config& config)
{
	if (array != nullptr)
		return json_array_append_value(array, json_value_deep_copy(config.root_value)) == JSONSuccess;

	return false;
}

bool Config::AddArray(const char* array_name)
{
	JSON_Value* va = json_value_init_array();
	array = json_value_get_array(va);

	return json_object_set_value(root_object, array_name, va) == JSONSuccess;
}

int Config::GetArrayCount(const char* field) const
{
	int ret = 0;
	JSON_Array* array = json_object_get_array(root_object, field);
	if (array != nullptr)
		ret = json_array_get_count(array);
	return ret;
}

Config Config::GetArray(const char* field, int index) const
{
	JSON_Array* array = json_object_get_array(root_object, field);
	if (array != nullptr)
		return Config(json_array_get_object(array, index));
	return Config((JSON_Object*) nullptr);
}

float3 Config::GetFloat3(const char* field, const float3& default)
{
	return float3(
		GetFloat(field, default.x, 0),
		GetFloat(field, default.y, 1),
		GetFloat(field, default.z, 2));
}