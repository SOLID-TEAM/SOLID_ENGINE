#ifndef _C_TRANSFORM_H__
#define _C_TRANSFORM_H__

#include "Component.h"
#include "external/MathGeoLib/include/MathGeoLib.h"

class C_Transform : public Component
{
public:
	C_Transform(GameObject* parent);
	~C_Transform();

	bool Update(float dt);

	bool DrawPanelInfo();

	void SetPosition(math::float3 new_position);

	void SetScale(math::float3 new_scale);

	math::float3 GetWorldPosition() const;

	void SetRotation(math::Quat rotation);

	void SetRotation(math::float3 euler_angles);

	math::float4x4 GetLocalTransform() const;

	math::float3 GetPosition() const;

	math::Quat GetQuatRotation() const;

	math::float3 GetEaRotation() const;

	math::float3 GetScale() const;

	void UpdateLocalTransform();

	math::float4x4 GetGlobalTransform() const;

	bool HasNegativeScale();
	 
private:

	void UpdateTRS();


public:

	float3		position;
	Quat		rotation;
	float3		scale;

	float4x4	local_transform;
	float4x4	global_transform;

private:

	bool		negative_scale = false;
	bool		to_update = true;
};

#endif // !_C_TRANSFORM_H__



