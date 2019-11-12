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

	bool DrawPanelInfo();

	// Degrees 4.0 to 179.0

	void SetAspectRatio( float width, float height);

	void SetFov(float fov);

	void SetClippingNearPlane(float distance);

	void SetClippingFarPlane(float distance);

	float GetFov();

	float GetClippingNearPlane();

	float GetClippingFarPlane();

	math::float4x4 GetProjectionMatrix();

	math::float4x4 GetViewMatrix();

private:

	void UpdateFov();

	void UpdateProjectionMatrix();

	void UpdateViewMatrix();

private:

	math::Frustum frustum;

	math::float4x4 projection_matrix;
	math::float4x4 view_matrix;

	float aspect_ratio = 1.f;
	bool cullling = true;

};

#endif // !_C_CAMERA_H__

