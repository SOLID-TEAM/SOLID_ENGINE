#ifndef _C_TRANSFORM_H__
#define _C_TRANSFORM_H__

#include "Component.h"

//#include "external/MathGeoLib/include/Math/float4x4.h"
//#include "external/MathGeoLib/include/Math/float3.h"
#include "external/MathGeoLib/include/MathGeoLib.h"

class C_Transform : public Component
{
public:
	C_Transform(GameObject* parent);
	~C_Transform();

	//bool Update(float dt);
	bool DrawPanelInfo();

private:
	float3 position = position.zero;
	float3 rotation = rotation.zero;
	float3 scale = scale.zero;
	Quat rot = rot.identity;

	float4x4 local_matrix = local_matrix.identity;
	float4x4 global_matrix = global_matrix.identity;
};

#endif // !_C_TRANSFORM_H__



