#include "C_Camera.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleScene.h"
#include "GL/glew.h"

#include "GameObject.h"
#include "C_Transform.h"

C_Camera::C_Camera(GameObject* go): Component(go, ComponentType::CAMERA)
{
	name.assign("Camera");

	frustum.pos = math::float3::zero;
	frustum.front = math::float3::unitZ;
	frustum.up = math::float3::unitY;

	SetFrustumType(math::FrustumType::PerspectiveFrustum);
	SetAspectRatio(16.f, 9.f );
	SetClippingNearPlane(0.01f);
	SetClippingFarPlane(20.F);
	SetFov(60.f);

}

bool C_Camera::CleanUp()
{
	return true;
}

void C_Camera::UpdateTransform()
{
	math::float4x4 global_transform = linked_go->transform->global_transform;

	frustum.pos = global_transform.TranslatePart();
	frustum.front = global_transform.RotatePart().Mul(float3::unitZ).Normalized();
	frustum.up = global_transform.RotatePart().Mul(float3::unitY).Normalized();


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
	if (distance <= 0.01f) distance = 0.01f;

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
	
bool C_Camera::CheckCollisionAABB(AABB& aabb)
{
	float3 corners[8];
	aabb.GetCornerPoints(corners);

	for (uint i = 0; i < 6; ++i)
	{
		Plane plane = frustum.GetPlane(i);
		int positive_count = 8;

		for (uint i = 0; i < 8; ++i)
		{
			if (plane.IsOnPositiveSide(corners[i]))
				--positive_count;
		}

		if (positive_count == 0)
			return false;
	}
	return true;
}

void C_Camera::UpdateFov()
{
	frustum.horizontalFov = 2.0f * atanf(tanf(frustum.verticalFov * 0.5f) * aspect_ratio);
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
	if (App->scene->selected_go == linked_go && App->scene->editor_mode)
	{
		App->renderer3D->RenderFrustum(frustum, 1.f, float4(0.7f, 0.7f, 0.7f, 0.7f));
	}

	return true;
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

	ImGui::Title("Culling", 1);  ImGui::Checkbox("##culling", &cullling);
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

//int C_Camera::IntersectAABB(AABB& aabb)
//{
//	int    ret = 1;
//
//	Plane planes[6];
//	frustum.GetPlanes(planes);
//
//	float3  min = aabb.minPoint, max = aabb.maxPoint;
//
//	for (uint i = 0; i < 6; ++i)
//	{
//		float3 v_min, v_max;
//
//		// X axis 
//		if (planes[i].normal.x > 0)
//		{
//			v_min.x = min.x;
//			v_max.x = max.x;
//		}
//		else {
//			v_min.x = max.x;
//			v_max.x = min.x;
//		}
//		// Y axis 
//		if (planes[i].normal.y > 0)
//		{
//			v_min.y = min.y;
//			v_max.y = max.y;
//		}
//		else
//		{
//			v_min.y = max.y;
//			v_max.y = min.y;
//		}
//		// Z axis 
//		if (planes[i].normal.z > 0)
//		{
//			v_min.z = min.z;
//			v_max.z = max.z;
//		}
//		else {
//			v_min.z = max.z;
//			v_max.z = min.z;
//		}
//
//		if (planes[i].normal.Dot(v_min) + planes[i].d > 0)
//		{
//			return 2; // OUTSIDE
//		}
//
//		if (planes[i].normal.Dot(v_max) + planes[i].d >= 0)
//		{
//			ret = 0;
//		}
//	}
//
//	return 1;
//}
//
//bool C_Camera::IntersectAABB2(AABB& aabb)
//{
//	int    ret = 1;
//
//	Plane planes[6];
//	frustum.GetPlanes(planes);
//
//	float3  min = aabb.minPoint, max = aabb.maxPoint;
//
//	for (uint i = 0; i < 6; ++i)
//	{
//
//		float4 plane(planes[i].normal.x, planes[i].normal.y, planes[i].normal.z, planes[i].d);
//
//		if (plane.x * min.x + plane.y * min.y + plane.z * min.z + plane.w > 0)
//			continue;
//		if (plane.x * max.x + plane.y * min.y + plane.z * min.z + plane.w > 0)
//			continue;
//		if (plane.x * min.x + plane.y * max.y + plane.z * min.z + plane.w > 0)
//			continue;
//		if (plane.x * max.x + plane.y * max.y + plane.z * min.z + plane.w > 0)
//			continue;
//		if (plane.x * min.x + plane.y * min.y + plane.z * max.z + plane.w > 0)
//			continue;
//		if (plane.x * max.x + plane.y * min.y + plane.z * max.z + plane.w > 0)
//			continue;
//		if (plane.x * min.x + plane.y * max.y + plane.z * max.z + plane.w > 0)
//			continue;
//		if (plane.x * max.x + plane.y * max.y + plane.z * max.z + plane.w > 0)
//			continue;
//
//		return false;
//	}
//	return true;
//}
