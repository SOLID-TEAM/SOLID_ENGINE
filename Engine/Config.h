#pragma once

#include "external/Parson/parson.h"

class Config 
{
public:
	Config(); // local init if needed
	Config(const char* path); // application init, needed
	Config(JSON_Object* section); // application init get sections for each module from json object but with own class functionality

public:

	Config GetSection(const char* module_name) const;

private:
	JSON_Value* root_value = nullptr;
	JSON_Object* root_object = nullptr;

};