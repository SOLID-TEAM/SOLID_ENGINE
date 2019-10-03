#pragma once

#include "external/Parson/parson.h"

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

	// --------------------
	// Add -----
	bool AddBool(const char* name, bool value);
	bool AddString(const char* name, const char* string);
	bool AddInt(const char* name, int value);
	// Get -----
	bool GetBool(const char* name) const;
	const char* GetString(const char* name) const;
	int GetInt(const char* name) const;


private:
	JSON_Value* root_value = nullptr;
	JSON_Object* root_object = nullptr;

};