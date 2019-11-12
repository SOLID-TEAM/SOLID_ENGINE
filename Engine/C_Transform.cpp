#include "C_Transform.h"
#include "GameObject.h"

#include "ImGui/imgui.h"

C_Transform::C_Transform(GameObject* parent) : Component(parent, ComponentType::TRANSFORM)
{
	name.assign("Transform");
	flags |= ImGuiTreeNodeFlags_DefaultOpen;

	position = float3::zero;
	rotation = float3::zero;
	scale = float3::one;
	global_transform = math::float4x4::identity;

	UpdateLocalTransform();

}

C_Transform::~C_Transform()
{

}

bool C_Transform::Update(float dt)
{
	if (to_update)
	{
		GameObject* parent = linked_go->parent;

		// Update Global Transform from parent -----------------------

		if (parent != nullptr)
		{
			global_transform = parent->transform->global_transform * local_transform;

			for (std::vector<GameObject*>::iterator child = linked_go->childs.begin(); child != linked_go->childs.end(); ++child)
			{
				(*child)->transform->to_update = true;
			}
		}

		// Update scale value factor ---------------------------------

		math::float3 global_scale;
		global_transform.Decompose(float3(), Quat(), global_scale);
		negative_scale = !((scale.x * scale.y * scale.z * global_scale.x * global_scale.y * global_scale.z) >= 0.f);

		// Update Components related with Transform ------------------

		std::vector< Component*> components = linked_go->GetComponents();

		for (std::vector<Component*>::iterator itr = components.begin(); itr != components.end(); ++itr)
		{
			(*itr)->UpdateTransform();
		}

		// Finish transform update -----------------------------------

		to_update = false;
	}

	return true;
}

// TRS Set/Get/Update Functions ==========================================================

void C_Transform::SetPosition(math::float3 position)
{
	this->position = position;
}

void C_Transform::SetRotation(math::float3 rotation) 
{
	this->rotation = rotation;
}

void C_Transform::SetScale(math::float3 scale)
{
	this->scale = scale;
}

math::float3 C_Transform::GetPosition() const
{
	return position;
}

math::float3 C_Transform::GetRotation() const
{
	return float3::zero;
}

math::float3 C_Transform::GetScale() const
{
	return scale;
}

math::float3 C_Transform::GetWorldPosition() const
{
	return global_transform.TranslatePart();
}

void C_Transform::UpdateTRS()
{
	math::Quat q_rotation;
	local_transform.Decompose(position, q_rotation, scale);
	rotation = q_rotation.ToEulerXYZ();
}

// Transforms Set/Get/Update =============================================================

math::float4x4 C_Transform::GetLocalTransform() const
{
	return local_transform;
}

math::float4x4 C_Transform::GetGlobalTransform() const
{
	return global_transform;
}

void C_Transform::UpdateLocalTransform()
{
	math::Quat q_rotation = math::Quat::FromEulerXYZ(rotation.x, rotation.y, rotation.z);
	local_transform = math::float4x4::FromTRS(position, q_rotation, scale);
	to_update = true;
}

bool C_Transform::HasNegativeScale()
{
	return negative_scale;
}

void C_Transform::LookAt(math::float3 reference)
{
	global_transform.SetRotatePart(float3x3::LookAt(float3::unitZ, (GetWorldPosition() - reference).Normalized(), float3::unitY, float3::unitY));

	GameObject* parent = linked_go->parent;

	if (parent != nullptr)
	{
		local_transform  = parent->transform->global_transform.Inverted() * global_transform;
	}

	UpdateTRS();

	to_update = true;
}

bool C_Transform::DrawPanelInfo()
{
	const double f_min = -1000000000000000.0, f_max = 1000000000000000.0;
	math::float3 last_position	= position;
	math::float3 last_rotation	= rotation;
	math::float3 last_scale		= scale;

	math::float3 euler_angles = rotation * RADTODEG;

	ImGui::Spacing();
	ImGui::Title("Position", 1);	ImGui::DragScalarN("##position",ImGuiDataType_Float, position.ptr(), 3, 0.01f, &f_min, &f_max, "%.2f" , 0.05f);
	ImGui::Title("Rotation", 1);	ImGui::DragScalarN("##rotation",ImGuiDataType_Float, euler_angles.ptr(), 3, 0.1f,  &f_min, &f_max, "%.2f",  0.05f);
	ImGui::Title("Scale", 1);		ImGui::DragScalarN("##scale	",  ImGuiDataType_Float, scale.ptr(), 3, 0.01f, &f_min, &f_max, "%.2f", 0.05f);
	ImGui::Spacing();

	rotation = euler_angles * DEGTORAD;


	if (!last_position.Equals(position) || !last_rotation.Equals(rotation) || !last_scale.Equals(scale))
	{
		UpdateLocalTransform();
	}


	return true;
}