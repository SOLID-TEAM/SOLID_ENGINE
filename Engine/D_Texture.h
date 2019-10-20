#ifndef __D_TEXTURE_H__
#define __D_TEXTURE_H__

#include "Data.h"

class D_Texture : public Data
{
public:

	D_Texture(): Data( DataType::TEXTURE) {};
	~D_Texture() {}

public:

	uint buffer_id = 0;
};
#endif //__R_TEXTURE_H__