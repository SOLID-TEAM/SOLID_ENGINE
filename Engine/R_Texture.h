#ifndef __R_TEXTURE_H__
#define __R_TEXTURE_H__

#include "Resource.h"

class R_Texture : public Resource
{
public:

	enum Format {
		color_index,
		rgb,
		rgba,
		bgr,
		bgra,
		luminance,
		unknown
	};

public:

	R_Texture(UID id);
	~R_Texture();

	bool LoadInMemory() override;
	void ReleaseFromMemory();
	void Save(Config& config) const override;
	void Load(const Config& config) override;


public:
	uint buffer_id = 0;

	uint width = 0;
	uint height = 0;
	uint depth = 0;
	bool has_mips = false;
	bool linear = false;
	uint bytes = 0;

	Format format = unknown;
};
#endif //__R_TEXTURE_H__