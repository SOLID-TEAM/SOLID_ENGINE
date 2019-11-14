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

	local_position = float3::zero;
	local_rotation = float3::zero;
	local_scale = float3::one;
	global_transform = math::float4x4::identity;

	UpdateLocalTransformFromTRS();

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

		position = global_transform.TranslatePart();
		rotation = global_transform.RotatePart().ToEulerXYZ() * RADTODEG;
		scale = global_transform.GetScale();

		forward = global_transform.WorldZ();
		up = global_transform.WorldY();
		right = global_transform.WorldX();

		math::float3 global_scale;
		global_transform.Decompose(float3(), Quat(), global_scale);
		negative_scale = !((local_scale.x * local_scale.y * local_scale.z * global_scale.x * global_scale.y * global_scale.z) >= 0.f);

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

void C_Transform::Rotate(math::float3 axis, float angle)
{
	angle *= DEGTORAD;
	global_transform = global_transform * Quat::RotateAxisAngle(axis, angle);

	UpdateLocalTransformFromGlobal();
}

// World ==========================================================

void C_Transform::SetPosition(math::float3 position)
{
	global_transform.SetTranslatePart(position);

	UpdateLocalTransformFromGlobal();
}

void C_Transform::SetRotation(math::float3 rotation)
{
	rotation *= DEGTORAD;
	Quat q_rotation = Quat::FromEulerXYZ(rotation.x, rotation.y,rotation.z);
	global_transform.SetRotatePart(q_rotation);

	UpdateLocalTransformFromGlobal();
}

void C_Transform::SetRotation(math::Quat q_rotation)
{
	global_transform.SetRotatePart(q_rotation);

	UpdateLocalTransformFromGlobal();
}

void C_Transform::SetScale(math::float3 scale)
{
	global_transform.Scale(scale);

	UpdateLocalTransformFromGlobal();
}

math::float3 C_Transform::GetPosition() const
{
	return position;
}

math::float3 C_Transform::GetRotation() const
{
	return global_transform.ToEulerXYZ() * RADTODEG;
}

math::float3 C_Transform::GetScale() const
{
	return scale;
}

math::float4x4 C_Transform::GetGlobalTransform() const
{
	return global_transform;
}

void C_Transform::UpdateLocalTransformFromGlobal()
{
	GameObject* parent = linked_go->parent;

	if (parent != nullptr)
	{
		local_transform = parent->transform->global_transform.Inverted() * global_transform;
	}

	UpdateTRS();

	to_update = true;
}

// Local ==========================================================

void C_Transform::SetLocalPosition(math::float3 position)
{
	this->local_position = position;
	UpdateLocalTransformFromTRS();
}

void C_Transform::SetLocalRotation(math::float3 rotation)
{
	this->local_rotation = rotation;
	UpdateLocalTransformFromTRS();
}

void C_Transform::SetLocalScale(math::float3 scale)
{
	this->local_scale = scale;
	UpdateLocalTransformFromTRS();
}

math::float3 C_Transform::GetLocalPosition() const
{
	return math::float3();
}

math::float3 C_Transform::GetLocalRotation() const
{
	return math::float3();
}

math::float3 C_Transform::GetLocalScale() const
{
	return math::float3();
}


math::float4x4 C_Transform::GetLocalTransform() const
{
	return local_transform;
}

void C_Transform::UpdateLocalTransformFromTRS()
{
	math::Quat q_rotation = math::Quat::FromEulerXYZ(local_rotation.x, local_rotation.y, local_rotation.z);
	local_transform = math::float4x4::FromTRS(local_position, q_rotation, local_scale);
	to_update = true;
}

// Transforms Set/Get/Update =============================================================


void C_Transform::UpdateTRS()
{
	math::Quat q_rotation;
	local_transform.Decompose(local_position, q_rotation, local_scale);
	local_rotation = q_rotation.ToEulerXYZ();

	position = global_transform.TranslatePart();
	rotation = global_transform.RotatePart().ToEulerXYZ() * RADTODEG;
	scale = global_transform.GetScale();

	forward = global_transform.WorldZ();
	up = global_transform.WorldY();
	right = global_transform.WorldX();
}

bool C_Transform::HasNegativeScale()
{
	return negative_scale;
}

void C_Transform::LookAt(math::float3 reference)
{
	global_transform.SetRotatePart(float3x3::LookAt(float3::unitZ, (position - reference).Normalized(), float3::unitY, float3::unitY));

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
	math::float3 last_position	= local_position;
	math::float3 last_rotation	= local_rotation;
	math::float3 last_scale		= local_scale;

	math::float3 euler_angles = local_rotation * RADTODEG;

	ImGui::Spacing();
	ImGui::Title("Position", 1);	ImGui::DragScalarN("##position",ImGuiDataType_Float, local_position.ptr(), 3, 0.01f, &f_min, &f_max, "%.2f" , 0.05f);
	ImGui::Title("Rotation", 1);	ImGui::DragScalarN("##rotation",ImGuiDataType_Float, euler_angles.ptr(), 3, 0.1f,  &f_min, &f_max, "%.2f",  0.05f);
	ImGui::Title("Scale", 1);		ImGui::DragScalarN("##scale	",  ImGuiDataType_Float, local_scale.ptr(), 3, 0.01f, &f_min, &f_max, "%.2f", 0.05f);
	ImGui::Spacing();

	local_rotation = euler_angles * DEGTORAD;


	if (!last_position.Equals(local_position) || !last_rotation.Equals(local_rotation) || !last_scale.Equals(local_scale))
	{
		UpdateLocalTransformFromTRS();
	}


	return true;
}


bool C_Transform::Save(Config& config)
{
	//Config myConfig;

	config.AddFloatArray("Position", (float*)&position, 3);
	config.AddFloatArray("Rotation", (float*)&rotation, 3);
	config.AddFloatArray("Scale", (float*)&scale, 3);

	//config.AddArrayEntry(myConfig);

	return true;
}