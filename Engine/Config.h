#pragma once

#include "external/Parson/parson.h"
#include "external/MathGeoLib/include/Math/float3.h"

class Config 
{
public:
	Config(); // local init if needed
	Config(const char* path); // application init, needed
	Config(JSON_Object* section); // application init get sections for each module from json object but with own class functionality

public:

	bool IsOk() const;
	void SaveConfigToFile(const char* path);
	bool CreateNewRootObject(const char* filename);
	Config GetSection(const char* module_name) const;
	Config AddSection(const char* section_name);
	JSON_Value* Config::FindValue(const char* var_name, int array_index) const;
	// --------------------
	// Add -----
	bool AddBool(const char* name, bool value);
	bool AddString(const char* name, const char* string);
	bool AddInt(const char* name, int value);
	bool AddFloatArray(const char* var_name, const float* values, const int size);
	bool AddFloat(const char* var_name, const float value);
	bool AddArray(const char* array_name);
	bool AddArrayEntry(const Config& config);
	// Get -----
	bool GetBool(const char* name, const bool default, const int array_index = -1) const;
	const char* GetString(const char* name,  const char* default, const int array_index = -1) const;
	int GetInt(const char* name, const int default,const int array_index = -1) const;
	float GetFloat(const char* name, const float default, const float array_index = -1) const;
	float3 GetFloat3(const char* field, const float3& default);
	
	int GetArrayCount(const char* field) const;
	Config GetArray(const char* field, int index) const;

private:
	JSON_Value* root_value = nullptr;
	JSON_Object* root_object = nullptr;
	JSON_Array* array = nullptr;

};