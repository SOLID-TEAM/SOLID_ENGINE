#include "Resource.h"

void Resource::Save(Config& config) const
{
	config.AddInt("UID", uid);
	config.AddInt("Type", (int)type);
	//config.AddString("custom_name", name.c_str());
	//config.AddString("")
}

void Resource::Load(const Config& config)
{
	uid = config.GetInt("UID", uid);
	type = (Type)config.GetInt("Type", (int)type);
}

uint Resource::CountReferences() const
{
	return loaded;
}

bool Resource::IsLoadedToMemory() const
{
	return loaded > 0;
}

bool Resource::LoadToMemory()
{
	if (loaded > 0)
		loaded++;
	else
		loaded = LoadInMemory();

	return loaded > 0;
}