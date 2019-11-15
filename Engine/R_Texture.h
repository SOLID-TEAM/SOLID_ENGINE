#ifndef __R_TEXTURE_H__
#define __R_TEXTURE_H__

#include "Resource.h"

class R_Texture : public Resource
{
public:

	R_Texture(): Resource( ResourceType::TEXTURE) {};
	~R_Texture() {}

public:

	uint buffer_id = 0;
};
#endif //__R_TEXTURE_H__