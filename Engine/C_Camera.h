#ifndef _C_CAMERA_H__
#define  _C_CAMERA_H__

#include "Component.h"
#include "external/MathGeoLib/include/Geometry/GeometryAll.h"
#include "external/MathGeoLib/include/Math/MathAll.h"

class GameObject;

class C_Camera : public Component
{

public:

	C_Camera(GameObject* go);

	bool CleanUp();

	void UpdateTransform();

	bool Render();

	void LookAt(float3 reference);

	bool DrawPanelInfo();

	// Degrees 4.0 to 179.0

	void SetAspectRatio( float width, float height);

	void SetFov(float fov);

	void SetClippingNearPlane(float distance);

	void SetClippingFarPlane(float distance);

	void SetFrustumType(math::FrustumType type);

	float GetFov();

	float GetClippingNearPlane();

	float GetClippingFarPlane();

	math::FrustumType GetFrustumType();

	math::float4x4 GetProjectionMatrix();

	math::float4x4 GetViewMatrix();

	bool CheckCollisionAABB(AABB& aabb);

private:

	void UpdateFov();

	void UpdateProjectionMatrix();

	void UpdateViewMatrix();


public:

	bool cullling = true;

private:

	math::Frustum frustum;

	math::float4x4 projection_matrix;
	math::float4x4 view_matrix;

	float aspect_ratio = 1.f;
};

#endif // !_C_CAMERA_H__

