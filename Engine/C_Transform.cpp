#include "C_Transform.h"
#include "GameObject.h"
#include "DynTree.h"
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

	global_transform.SetIdentity();
	local_transform.SetIdentity();
}

bool C_Transform::Update(float dt)
{
	if (to_update)
	{
		GameObject* parent = linked_go->parent;

		// Update Global Transform from parent -----------------------

		UpdateLocalTransformFromGlobal();

		for (GameObject* child : linked_go->childs)
		{
			child->transform->to_update = true;
		}

		// TODO : Not working at all. Revise 
		math::float3 global_scale;
		global_transform.Decompose(float3(), Quat(), global_scale);
		negative_scale = !((local_scale.x * local_scale.y * local_scale.z * global_scale.x * global_scale.y * global_scale.z) >= 0.f);

		// Update Components related with Transform ------------------

		for (Component*  component : linked_go->components)
		{
			if (component != this)
			{
  				component->UpdateTransform();
			}
		}

		// Update bounding box ---------------------------------------

		C_Mesh* c_mesh = linked_go->GetComponent<C_Mesh>();

		if (c_mesh)
		{
			linked_go->obb = c_mesh->mesh_aabb;
			linked_go->obb.Transform(global_transform);
			linked_go->bounding_box.SetNegativeInfinity();
			linked_go->bounding_box.Enclose(linked_go->obb);
		}

		// Update Dynamic Tree ---------------------------------------

		if (linked_go->dyn_node != nullptr && linked_go->bounding_box.IsFinite())
		{
			if (!linked_go->dyn_node->aabb.Contains(linked_go->bounding_box))
			{
				//App->scene->dy.ReleaseNode(treeNode);
				//App->scene->aabbTree.InsertGO(this);
			}
		}

		// Finish transform update -----------------------------------

		to_update = false;
	}

	return true;
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
	Quat q_rotation = Quat::FromEulerZYX(rotation.z, rotation.y,rotation.x);
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
	global_transform.RemoveScale();
	global_transform.Scale(scale);
	UpdateLocalTransformFromGlobal();
}

void C_Transform::SetGlobalTransform(math::float4x4 matrix)
{
	global_transform = matrix;
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
	else
	{
		local_transform = global_transform;
	}

	UpdateTRS();

	to_update = true;
}

// Local ==========================================================

void C_Transform::SetLocalPosition(math::float3 position)
{
	local_transform.SetTranslatePart(position);
	UpdateGlobalTransformFromLocal();
}

void C_Transform::SetLocalRotation(math::float3 rotation)
{
	float3 aux_rotation = rotation * DEGTORAD;
	local_transform.SetRotatePart(Quat::RotateAxisAngle(float3::unitZ, aux_rotation.z) * Quat::RotateAxisAngle(float3::unitY, aux_rotation.y) * Quat::RotateAxisAngle(float3::unitX, aux_rotation.x));
	UpdateGlobalTransformFromLocal();
}

void C_Transform::SetLocalScale(math::float3 scale)
{
	local_transform.RemoveScale();
	local_transform.Scale(scale);

	UpdateGlobalTransformFromLocal();
}

math::float3 C_Transform::GetLocalPosition() const
{
	return local_position;
}

math::float3 C_Transform::GetLocalRotation() const
{
	return local_rotation;
}

math::float3 C_Transform::GetLocalScale() const
{
	return local_scale;
}


math::float4x4 C_Transform::GetLocalTransform() const
{
	return local_transform;
}

void C_Transform::UpdateGlobalTransformFromLocal()
{
	GameObject* parent = linked_go->parent;

	if (parent != nullptr)
	{
		global_transform = parent->transform->global_transform * local_transform;
	}
	else
	{
		global_transform = local_transform;
	}

	UpdateTRS();

	to_update = true;
}

// Transforms Set/Get/Update =============================================================


void C_Transform::UpdateTRS()
{
	local_position = local_transform.TranslatePart();
	local_rotation = RadToDeg(local_transform.ToEulerZYX());
	local_rotation = { local_rotation.z, local_rotation.y ,local_rotation.x };
	local_scale = local_transform.GetScale();

	position = global_transform.TranslatePart();
	rotation = RadToDeg(global_transform.ToEulerZYX()) ;
	rotation = { rotation.z, rotation.y ,rotation.x };
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
	global_transform.SetRotatePart(Quat::identity);
	global_transform = global_transform.LookAt( position, reference, float3::unitZ, float3::unitY, float3::unitY);

	GameObject* parent = linked_go->parent;

	if (parent != nullptr)
	{
		local_transform = parent->transform->global_transform.Inverted() * global_transform;
	}
	else
	{
		local_transform = global_transform;
	}

	UpdateTRS();

	to_update = true;
}

bool C_Transform::DrawPanelInfo()
{
	const double f_min = -1000000000000000.0, f_max = 1000000000000000.0;

	math::float3 delta_position	= local_position;
	math::float3 delta_rotation	= local_rotation;
	math::float3 delta_scale	= local_scale;

	ImGui::Spacing();
	ImGui::Title("Position", 1);	ImGui::DragFloat3("##position", delta_position.ptr(), 0.01f, f_min, f_max, "%.2f");
	ImGui::Title("Rotation", 1);	ImGui::DragFloat3("##rotation", delta_rotation.ptr(), 0.1f,  f_min, f_max, "%.2f");
	ImGui::Title("Scale", 1);		ImGui::DragFloat3("##scale	",	delta_scale.ptr(), 0.01f, f_min, f_max, "%.2f");
	ImGui::Spacing();


	if (!delta_position.Equals(local_position))
	{
		SetLocalPosition(delta_position);
	}

	if (!delta_rotation.Equals(local_rotation))
	{
		SetLocalRotation(delta_rotation);
	}
	
	if (!delta_scale.Equals(local_scale))
	{
		SetLocalScale(delta_scale);
	}


	return true;
}


bool C_Transform::Save(Config& config)
{
	config.AddFloatArray("Position", (float*)&local_position, 3);
	config.AddFloatArray("Rotation", (float*)&local_rotation, 3);
	config.AddFloatArray("Scale", (float*)&local_scale, 3);
	return true;
}

bool C_Transform::Load(Config& config)
{
	// @TODO

	SetLocalPosition(config.GetFloat3("Position", local_position));
	SetLocalRotation(config.GetFloat3("Rotation", local_rotation));
	SetLocalScale(config.GetFloat3("Scale", local_scale));

	return true;
}