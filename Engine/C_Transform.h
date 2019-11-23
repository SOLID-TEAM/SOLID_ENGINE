#ifndef _C_TRANSFORM_H__
#define _C_TRANSFORM_H__

#include "Component.h"
#include "external/MathGeoLib/include/MathGeoLib.h"

class C_Transform : public Component
{
public:
	C_Transform(GameObject* parent);

	~C_Transform();

	bool Update();

	void Rotate(math::float3 axis, float angle);

	bool DrawPanelInfo();

	// World -------------------------------------------


	void SetPosition(math::float3 position);

	void SetRotation(math::float3 rotation);

	void SetRotation(math::Quat q_rotation);

	void SetScale(math::float3 scale);

	void SetGlobalTransform(math::float4x4 matrix);

	math::float3 GetPosition() const;

	math::float3 GetRotation() const;

	math::float3 GetScale() const;

	math::float4x4 GetGlobalTransform() const;

	void UpdateLocalTransformFromGlobal();

	// Local -------------------------------------------

	void SetLocalPosition(math::float3 position);

	void SetLocalRotation(math::float3 rotation);

	void SetLocalScale(math::float3 scale);

	void SetLocalTransform(math::float4x4 matrix);

	math::float3 GetLocalPosition() const;

	math::float3 GetLocalRotation() const;

	math::float3 GetLocalScale() const;

	math::float4x4 GetLocalTransform() const;

	void UpdateGlobalTransformFromLocal();

	// ----------------------------------------------

	void UpdateTRS();

	bool HasNegativeScale();

	void LookAt(math::float3 eye);

	// save/load

	bool Save(Config& config);
	bool Load(Config& config);

public:

	// World ----------------------------------------

	math::float3	position;
	math::float3	rotation;
	math::float3	scale;
;
	math::float3	forward;
	math::float3	up;
	math::float3	right;

	math::float4x4	global_transform;

	// Local ----------------------------------------

	math::float3	local_position;
	math::float3	local_rotation;
	math::float3	local_scale;

	math::float4x4	local_transform;

private:

	bool		negative_scale = false;
	bool		to_update = true;
};

#endif // !_C_TRANSFORM_H__



