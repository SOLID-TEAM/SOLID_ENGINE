#include "Application.h"
#include "C_JointP2P.h"
#include "GameObject.h"

// TODO:
//constraint->getFixedBody // TODO: use this to track the link between , delete connected_body info of this class

C_JointP2P::C_JointP2P(GameObject* go) : Component(go, ComponentType::JOINTP2P)
{
	name.assign("Component Joint P2P");

	pivotA = float3::zero;
	pivotB = float3::zero;
}

C_JointP2P::~C_JointP2P() {}

bool C_JointP2P::Update() 
{
	//dynamic_cast<btPoint2PointConstraint*>(constraint)->m_setting.m_damping = 0.f;
	//dynamic_cast<btPoint2PointConstraint*>(constraint)->m_setting.m_tau = 1.f;
	//dynamic_cast<btPoint2PointConstraint*>(constraint)->m_setting.m_impulseClamp = 1.f;

	if (is_loaded)
	{
		connected_body = App->scene->FindByUID(connected_body_id, App->scene->root_go);

		if (connected_body == nullptr || connected_body == App->scene->root_go)
		{
			connected_body_id = 0;
			pivotB = float3::zero;
			connected_body = nullptr;
		}
	}
	else
	{
		if (check_rigid_body_exist)
		{
			C_RigidBody* c_rbody = linked_go->GetComponent<C_RigidBody>();

			if (c_rbody == nullptr)
			{
				c_rbody = linked_go->AddComponent<C_RigidBody>();
			}

			check_rigid_body_exist = false;
		}

		RecreateConstraint();
		is_loaded = false;
	}

	if (constraint == nullptr)
	{
		RecreateConstraint();
	}
	
	if (constraint != nullptr)
	{
		dynamic_cast<btPoint2PointConstraint*>(constraint)->setEnabled(true);
		dynamic_cast<btPoint2PointConstraint*>(constraint)->setPivotA(btVector3(pivotA.x, pivotA.y, pivotA.z));
		dynamic_cast<btPoint2PointConstraint*>(constraint)->setPivotB(btVector3(pivotB.x, pivotB.y, pivotB.z));
	}

	return true;
}

void C_JointP2P::BodyDeleted( GameObject* go)
{
	if (connected_body == go)
	{
		connected_body = nullptr;
		RecreateConstraint();
	}
}

void C_JointP2P::RecreateConstraint()
{
	if (constraint)
	{
		App->physics->RemoveConstraint(constraint);
		delete constraint;
		constraint = nullptr;
	}

	C_RigidBody* c_rb_a = linked_go->GetComponent<C_RigidBody>();
	C_RigidBody* c_rb_b = (connected_body) ? connected_body->GetComponent<C_RigidBody>() : nullptr;

	if (c_rb_a && c_rb_b)
	{
		if (is_loaded == false)
		{
			float3 pos = c_rb_a->linked_go->transform->position;
			pos = c_rb_b->linked_go->transform->GetGlobalTransform().Inverted().TransformPos(pos);
			pivotB = pos;
		}

		constraint = new btPoint2PointConstraint(*c_rb_a->body, *c_rb_b->body, ToBtVector3(pivotA), ToBtVector3(pivotB));
	}
	else if (c_rb_a)
	{
		if (is_loaded == false)
		{
			pivotB = c_rb_a->linked_go->transform->position;
		}
		constraint = new btPoint2PointConstraint(*c_rb_a->body, ToBtVector3(pivotA));	
	}

	if (constraint != nullptr)
	{
		constraint->setUserConstraintPtr(this);  // Add ptr reference	
		constraint->setDbgDrawSize(2.0f);
		App->physics->AddConstraint(constraint, disable_collision);
	}
}

bool C_JointP2P::Render()
{
	if (constraint)
		App->physics->RenderConstraint(constraint);

	return true;
}

bool C_JointP2P::DrawPanelInfo()
{
	bool body_linked = connected_body ? true : false;
	bool last_disable_collision = disable_collision;
	bool changed_conected_body = false;

	ImGui::Title("Connected Body"); ImGui::Button(body_linked ? connected_body->GetName() : "none", ImVec2(120, 22)); ImGui::SameLine();

	if (body_linked) 
	{
		if (ImGui::BeginPopupContextItem(linked_go->GetName()))
		{
			if (ImGui::Button("Remove connexion"))
			{
				connected_body = nullptr;
				RemakeConstraint();
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}
	
	if (ImGui::BeginDragDropTarget())
	{
		ImVec4 color_red = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
		ImVec4 color_green = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
		ImVec4& color = color_red;
		bool validBody = false;

		// check if the gameobject has rigidbody attached before accept to change visual color and evade accepting directly
		const ImGuiPayload* payload = ImGui::GetDragDropPayload();

		IM_ASSERT(payload->DataSize == sizeof(GameObject*));
		GameObject** go = (GameObject**)payload->Data;

		// TODO: CHANGE TO COMPONENT RIGIDBODY
		C_RigidBody* c_rbody = (*go)->GetComponent<C_RigidBody>();

		if (c_rbody && *go != linked_go)
		{
			if (c_rbody->body)
			{
				color = color_green;
				validBody = true;
				changed_conected_body = true;
			}
		}

		ImGui::PushStyleColor(ImGuiCol_DragDropTarget, color);
		
		if (ImGui::AcceptDragDropPayload("gameobject_object") && validBody)
		{
			connected_body = *go;
			RemakeConstraint();
		}

		ImGui::EndDragDropTarget();

		ImGui::PopStyleColor();
	}
	App->editor->HelpMarker("Drag and drop a gameobject with rigidbody component attached to link to");
	
	// --------------------
	ImGui::Title("Pivot A", 1);	
	ImGui::DragFloat3("##pivotA", pivotA.ptr(), 0.1f);
	// linked rigibody ---
	if (!body_linked)
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.1f, 1.0f));
	ImGui::Title("Pivot B", 1);	
	ImGui::DragFloat3("##pivotB", pivotB.ptr(), 0.1f);
	if(!body_linked)
		ImGui::PopStyleColor();
	ImGui::Title("Disable Collision", 1); 
	ImGui::Checkbox("##col", &disable_collision);

	if (last_disable_collision != disable_collision || changed_conected_body)
	{
		RecreateConstraint();
	}

	// --------------------

	return true;
}

void C_JointP2P::RemakeConstraint()
{
	//App->physics->RemoveConstraint(constraint);

	//delete constraint;

	//btRigidBody* bodyB = nullptr;
	//if (connected_body != nullptr)
	//	bodyB = connected_body->GetComponent<C_RigidBody>()->body;

	//if (bodyB)
	//{
	//	constraint = new btPoint2PointConstraint(
	//		*linked_go->GetComponent<C_RigidBody>()->body,
	//		*bodyB,
	//		btVector3(pivotA.x, pivotA.y, pivotA.z),
	//		btVector3(pivotB.x, pivotB.y, pivotB.z));
	//}
	//else
	//{
	//	constraint = new btPoint2PointConstraint(*linked_go->GetComponent<C_RigidBody>()->body, btVector3(pivotA.x, pivotA.y, pivotA.z));
	//}

	//btVector3 _pivotA = dynamic_cast<btPoint2PointConstraint*>(constraint)->getPivotInA();
	//pivotA = { _pivotA.x(), _pivotA.y(), _pivotA.z() };

	//btVector3 _pivotB = dynamic_cast<btPoint2PointConstraint*>(constraint)->getPivotInB();
	//pivotB = { _pivotB.x(), _pivotB.y(), _pivotB.z() };

	//constraint->setDbgDrawSize(2.0f);

	//App->physics->AddConstraint(constraint, disable_collision);
}

bool C_JointP2P::CleanUp()
{
	if (constraint)
	{
		App->physics->RemoveConstraint(constraint);
		constraint = nullptr;
	}

	return true;
}

bool C_JointP2P::Save(Config& config)
{
	if (connected_body)
		config.AddInt("connected_body", connected_body->GetUID());

	config.AddFloatArray("pivotA", (float*)&pivotA, 3);
	config.AddFloatArray("pivotB", (float*)&pivotB, 3);
	config.AddBool("BodiesCollision", disable_collision);

	return true;
}
bool C_JointP2P::Load(Config& config)
{
	connected_body_id = config.GetInt("connected_body", 0);
	pivotA = config.GetFloat3("pivotA", { 0.f ,0.f, 0.f });
	pivotB = config.GetFloat3("pivotB", { 0.f ,0.f, 0.f });
	disable_collision = config.GetBool("BodiesCollision", true);

	return true;
}