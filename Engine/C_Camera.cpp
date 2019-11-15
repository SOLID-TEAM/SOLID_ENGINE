#include "C_Camera.h"
#include "ModuleRenderer3D.h"
#include "GL/glew.h"

#include "GameObject.h"
#include "C_Transform.h"

C_Camera::C_Camera(GameObject* go): Component(go, ComponentType::CAMERA)
{
	name.assign("Camera");

	frustum.type = math::FrustumType::PerspectiveFrustum;

	frustum.pos = math::float3::zero;
	frustum.front = math::float3::unitZ;
	frustum.up = math::float3::unitY;

	SetAspectRatio(16.f, 9.f );
	SetClippingNearPlane(0.03f);
	SetClippingFarPlane(30.f);
	SetFov(60.f);

}

bool C_Camera::CleanUp()
{
	return true;
}

void C_Camera::UpdateTransform()
{
	math::float4x4 global_transform = linked_go->transform->GetGlobalTransform();
	frustum.pos = global_transform.TranslatePart();
	frustum.front = global_transform.WorldZ();
	frustum.up = global_transform.WorldY();
	UpdateViewMatrix();
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
	frustum.horizontalFov = 2.0f * atanf(tanf(frustum.verticalFov * 0.5f) * aspect_ratio);
	UpdateProjectionMatrix();
}

void C_Camera::SetClippingNearPlane(float distance)
{
	if (distance < 0.f) distance = 0.f;

	frustum.nearPlaneDistance = distance;
	UpdateProjectionMatrix();
}

void C_Camera::SetClippingFarPlane(float distance)
{
	if (distance > 1000.f) distance = 1000.f;

	frustum.farPlaneDistance = distance;
	UpdateProjectionMatrix();
}

void C_Camera::SetFrustumType(math::FrustumType type)
{
	frustum.type = type;
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

math::FrustumType  C_Camera::GetFrustumType()
{
	return frustum.type;
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
	math::FrustumType type = GetFrustumType();
	float fov = GetFov();
	float near_plane = GetClippingNearPlane();
	float far_plane = GetClippingFarPlane();
	math::FrustumType last_type = type;
	float last_fov = fov;
	float last_near_plane = near_plane;
	float last_far_plane = far_plane;

	static const char* items[] = { "Perspective", "Orthographic" };
	static const char* current_item = items[0];

	ImGui::Spacing();

	ImGui::Title("Projection", 1);

	if (ImGui::BeginComboEx(std::string("##frustum_type").c_str(), std::string(" " + std::string(current_item)).c_str(), 200, ImGuiComboFlags_NoArrowButton))
	{
		for (int n = 0; n < IM_ARRAYSIZE(items); ++n)
		{
			bool is_selected = (current_item == items[n]);

			if (ImGui::Selectable(std::string("   " + std::string(items[n])).c_str(), is_selected))
			{
				current_item = items[n];

				if (current_item == "Perspective")			frustum.type = math::FrustumType::PerspectiveFrustum;
				else if (current_item == "Orthographic")	frustum.type = math::FrustumType::OrthographicFrustum;
			}

			if (is_selected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	ImGui::Title("Field of View", 1);	ImGui::SliderFloat("##fov", &fov, 4.f, 179.f, "%.1f");
	ImGui::Title("Clipping", 1);  ImGui::Text("");

	ImGui::Spacing();
	ImGui::Title("Near Plane", 3);	ImGui::DragFloat("##near_plane", &near_plane, 0.1f,  0.00f, 0.f, "%.1f");
	ImGui::Title("Far Plane", 3);	ImGui::DragFloat("##far_plane", &far_plane, 0.1f, 0.00f, 0.f, "%.1f");
	ImGui::Spacing();

	if (last_type != type)
	{
		SetFrustumType(type);
	}

	if (last_fov != fov )
	{
		SetFov(fov);
	}

	if (last_near_plane != near_plane)
	{
		SetClippingNearPlane(near_plane);
	}

	if (last_far_plane != far_plane)
	{
		SetClippingFarPlane(far_plane);
	}

	return true;
}