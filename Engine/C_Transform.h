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

	void SetPosition(math::float3 position);

	void SetRotation(math::float3 rotation);

	void SetScale(math::float3 scale);

	math::float3 GetPosition() const;

	math::float3 GetRotation() const;

	math::float3 GetScale() const;

	math::float3 GetWorldPosition() const;

	void UpdateTRS();

	math::float4x4 GetLocalTransform() const;

	math::float4x4 GetGlobalTransform() const;

	void UpdateLocalTransform();

	bool HasNegativeScale();

	void LookAt(math::float3 eye);

public:

	math::float3	position;
	math::float3	rotation;
	math::float3	scale;

	math::float4x4	local_transform;
	math::float4x4	global_transform;

private:

	bool		negative_scale = false;
	bool		to_update = true;
};

#endif // !_C_TRANSFORM_H__



