#include "C_Camera.h"
#include "ModuleRenderer3D.h"
#include "GL/glew.h"

C_Camera::C_Camera(GameObject* go): Component(go, ComponentType::CAMERA)
{
	frustum.type = math::FrustumType::PerspectiveFrustum;

	frustum.pos = math::float3::zero;
	frustum.front = math::float3::unitZ;
	frustum.up = math::float3::unitY;

	frustum.nearPlaneDistance = 0.03f;
	frustum.farPlaneDistance =	1000.0f;

	frustum.ProjectionMatrix();
	frustum.ViewMatrix();
}

bool C_Camera::CleanUp()
{
	return false;
}

void C_Camera::SetAspectRatio(float width, float height)
{
	aspect_ratio = width / height;
	UpdateFov();
}

void C_Camera::SetFov(float fov)
{
	if (fov < 4.f) fov = 4.f;
	if (fov > 179.f) fov = 179.f;

	frustum.verticalFov = fov * DEGTORAD;
	frustum.horizontalFov = 2.0f * atanf(tanf(frustum.verticalFov / 2.0f) * aspect_ratio);
	UpdateProjectionMatrix();
}

void C_Camera::SetClippingNearPlane(float distance)
{
	if (distance < 0.f) distance = 0.f;
	if (distance > frustum.farPlaneDistance) distance = frustum.farPlaneDistance;

	frustum.nearPlaneDistance = distance;
	UpdateProjectionMatrix();
}

void C_Camera::SetClippingFarPlane(float distance)
{
	if (distance < frustum.nearPlaneDistance) distance = frustum.nearPlaneDistance;
	if (distance > 10000.f) distance = 10000.f;

	frustum.farPlaneDistance = distance;
	UpdateProjectionMatrix();
}

float C_Camera::GetClippingNearPlane()
{
	return frustum.nearPlaneDistance;
}

float C_Camera::GetClippingFarPlane()
{
	return frustum.farPlaneDistance;
}

math::float4x4 C_Camera::GetProjectionMatrix()
{
	return projection_matrix;
}

math::float4x4 C_Camera::GetViewMatrix()
{
	return view_matrix;
}

void C_Camera::UpdateFov()
{
	frustum.horizontalFov = 2.0f * atanf(tanf(frustum.verticalFov / 2.0f) * aspect_ratio);
	UpdateProjectionMatrix();
}

void C_Camera::UpdateProjectionMatrix()
{
	projection_matrix = frustum.ProjectionMatrix();
}

void C_Camera::UpdateViewMatrix()
{
	view_matrix = frustum.ViewMatrix();
}

float C_Camera::GetFov()
{
	return frustum.verticalFov * RADTODEG;
}

bool C_Camera::Render()
{
	ModuleRenderer3D::BeginDebugDraw();
	glLineWidth(1.f);
	glBegin(GL_LINES);


	for (int i = 0; i < frustum.NumEdges(); ++i)
	{
		math::LineSegment line_segment = frustum.Edge(i);
		glVertex3f(line_segment.a.x, line_segment.a.y, line_segment.a.z);
		glVertex3f(line_segment.b.x, line_segment.b.y, line_segment.b.z);
	}

	glEnd();
	glLineWidth(1.f);
	ModuleRenderer3D::EndDebugDraw();
	return false;
}

bool C_Camera::DrawPanelInfo()
{
	return false;
}
