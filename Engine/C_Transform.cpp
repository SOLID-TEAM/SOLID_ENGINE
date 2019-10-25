#include "C_Transform.h"
#include "GameObject.h"

#include "ImGui/imgui.h"

C_Transform::C_Transform(GameObject* parent) : Component(parent, ComponentType::TRANSFORM)
{
	name.assign("Transform");
	flags |= ImGuiTreeNodeFlags_DefaultOpen;

	local_transform.SetIdentity();
	global_transform.SetIdentity();
	SetPosition(math::float3::zero);
	SetRotation(math::float3::zero);
	SetScale(math::float3::one);
}

C_Transform::~C_Transform()
{

}

bool C_Transform::Update(float dt)
{
	if (to_update)
	{
		if (linked_go->parent)
		{
			for (std::vector<GameObject*>::iterator child = linked_go->childs.begin(); child != linked_go->childs.end(); ++child)
			{
				(*child)->transform->to_update = true;
			}

			global_transform = linked_go->parent->transform->GetGlobalTransform() * local_transform;
			UpdateTRS();
		}

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
	quat_rotation = rotation;
	// Set Euler Angles Rotation ----------------
	UpdateEaFromQuat();
}

void C_Transform::SetRotation(math::float3 rotation) // Set Rotation with euler angles
{
	// Set Quaternion Rotation ------------------
	math::float3 ea_aux = (rotation - ea_rotation) * DEGTORAD;
	math::Quat quat_aux = Quat::FromEulerXYZ(ea_aux.x, ea_aux.y, ea_aux.z);
	quat_rotation = quat_rotation * quat_aux;

	// Set Euler Angles Rotation ----------------
	ea_rotation = rotation;

}

void C_Transform::SetScale(math::float3 scale)
{
	this->scale = scale;
	negative_scale = (scale.x * scale.y * scale.z) >= 0 ? false : true;
}

math::float3 C_Transform::GetPosition() const
{
	return position;
}

math::Quat C_Transform::GetQuatRotation() const
{
	return quat_rotation;
}

math::float3 C_Transform::GetEaRotation() const
{
	return ea_rotation;
}

math::float3 C_Transform::GetScale() const
{
	return scale;
}


void C_Transform::UpdateTRS()  // Update TRS with local transform valeus
{
	local_transform.Decompose(position, quat_rotation, scale); 
	UpdateEaFromQuat();
}

void C_Transform::UpdateEaFromQuat()
{
	ea_rotation = quat_rotation.ToEulerXYZ() ;
	ea_rotation *= RADTODEG;
}


// Transforms Set/Get/Update =============================================================

void C_Transform::UpdateLocalTransform()    // Update Local transform with TRS values
{
	local_transform = math::float4x4::FromTRS(position, quat_rotation, scale);
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

bool C_Transform::DrawPanelInfo()
{
	bool update_local_transform = false;

	float3 pos = position;
	float3 rot = ea_rotation;
	float3 scal = scale;


	ImGui::Spacing();
	ImGui::Title("Position", 1);	if (ImGui::DragFloat3("##position", (float*)&pos,  0.01f, 0.0f, 0.0f, "%.2f" , 0.05f)) {   SetPosition(pos); ; update_local_transform = true;};
	ImGui::Title("Rotation", 1);	if (ImGui::DragFloat3("##rotation", (float*)&rot,  0.1f, 0.0f, 0.0f, "%.2f",  0.05f))	{	SetRotation(rot); update_local_transform = true;};
	ImGui::Title("Scale", 1);		if (ImGui::DragFloat3("##scale	 ", (float*)&scal, 0.01f, 0.0f, 0.0f, "%.2f", 0.05f))	{	SetScale(scal);	 update_local_transform = true;};
	ImGui::Spacing();

	if (update_local_transform)
	{
		UpdateLocalTransform();
	}


	return true;
}