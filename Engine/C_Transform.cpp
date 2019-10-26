#include "C_Transform.h"
#include "GameObject.h"

#include "ImGui/imgui.h"

C_Transform::C_Transform(GameObject* parent) : Component(parent, ComponentType::TRANSFORM)
{
	name.assign("Transform");
	flags |= ImGuiTreeNodeFlags_DefaultOpen;

	position = float3::zero;
	rotation = Quat::identity;
	scale = float3::one;

	UpdateLocalTransform();
	global_transform = math::float4x4::identity;

}

C_Transform::~C_Transform()
{

}

bool C_Transform::Update(float dt)
{
	if (to_update)
	{
		GameObject* parent = linked_go->parent;

		if (parent != nullptr)
		{

			global_transform = parent->transform->global_transform * local_transform;

			if (parent != nullptr)
			{
				for (std::vector<GameObject*>::iterator child = linked_go->childs.begin(); child != linked_go->childs.end(); ++child)
				{
					(*child)->transform->to_update = true;
				}
			}
		}

		math::float3 global_scale;
		global_transform.Decompose(float3(), Quat(), global_scale);
		negative_scale = !((scale.x * scale.y * scale.z * global_scale.x * global_scale.y * global_scale.z) >= 0.f);

		to_update = false;
	}

	return true;
}

// TRS Set/Get/Update Functions ==========================================================

void C_Transform::SetPosition(math::float3 position)
{
	this->position = position;
}

void C_Transform::SetRotation(math::Quat rotation) // Set rotation with quaternion
{
	// Set Quaternion Rotation ------------------
	this->rotation = rotation;
}

void C_Transform::SetScale(math::float3 scale)
{

}

math::float3 C_Transform::GetWorldPosition() const
{
	return global_transform.TranslatePart();
}

math::float3 C_Transform::GetPosition() const
{
	return position;
}

math::Quat C_Transform::GetQuatRotation() const
{
	return rotation;
}

math::float3 C_Transform::GetEaRotation() const
{
	return float3::zero;
}

math::float3 C_Transform::GetScale() const
{
	return scale;
}


void C_Transform::UpdateTRS()  // Update TRS with local transform valeus
{
	local_transform.Decompose(position, rotation, scale); 
}


// Transforms Set/Get/Update =============================================================

void C_Transform::UpdateLocalTransform()    // Update Local transform with TRS values
{
	GameObject* parent = linked_go->parent;

	local_transform = math::float4x4::FromTRS(position, rotation, scale);

	to_update = true;
}

math::float4x4 C_Transform::GetLocalTransform() const
{
	return local_transform;
}

math::float4x4 C_Transform::GetGlobalTransform() const
{
	return global_transform;
}

bool C_Transform::HasNegativeScale()
{
	return negative_scale;
}

bool C_Transform::DrawPanelInfo()
{
	const double f_min = -1000000000000000.0, f_max = 1000000000000000.0;
	math::float3 last_position	= position;
	math::Quat	 last_rotation	= rotation;
	math::float3 last_scale		= scale;

	math::float3 euler_angles = rotation.ToEulerXYZ() * RADTODEG;
	math::float3 last_euler_angles = euler_angles;

	ImGui::Spacing();
	ImGui::Title("Position", 1);	ImGui::DragScalarN("##position",ImGuiDataType_Float, position.ptr(), 3, 0.01f, &f_min, &f_max, "%.2f" , 0.05f);
	ImGui::Title("Rotation", 1);	ImGui::DragScalarN("##rotation",ImGuiDataType_Float, euler_angles.ptr(), 3, 0.1f,  &f_min, &f_max, "%.2f",  0.05f);
	ImGui::Title("Scale", 1);		ImGui::DragScalarN("##scale	",  ImGuiDataType_Float, scale.ptr(), 3, 0.01f, &f_min, &f_max, "%.2f", 0.05f);
	ImGui::Spacing();

	float3 delta = (euler_angles - last_euler_angles) * DEGTORAD;
    rotation = rotation * Quat::FromEulerXYZ(delta.x, delta.y, delta.z);

	if (!last_position.Equals(position) || !last_rotation.Equals(rotation) || !last_scale.Equals(scale))
	{
		UpdateLocalTransform();
	}


	return true;
}