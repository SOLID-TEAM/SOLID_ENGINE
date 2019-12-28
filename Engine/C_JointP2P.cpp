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

	// on component creation adds this constraint to world alone ------
	// search for rb
	//btRigidBody* rbody = linked_go->GetComponent<C_RigidBody>();
	C_Collider* col = linked_go->GetComponent<C_Collider>();

	// TODO: if no RigidBody attached, create one with empty shape
	if (col == nullptr)
	{
		col = linked_go->AddComponent<C_BoxCollider>();
		col->CreateCollider();
	}

	constraint = new btPoint2PointConstraint(*col->body, btVector3(pivotA.x, 10, pivotA.z));
	constraint->setDbgDrawSize(2.0f);

	App->physics->AddConstraint(constraint, bodies_collision);

}

C_JointP2P::~C_JointP2P() {}

bool C_JointP2P::Update() 
{
	if (fromLoad)
	{
		connected_body = App->scene->FindByUID(connected_body_id, App->scene->root_go);
		
		if (connected_body == nullptr)
		{
			connected_body_id = 0;
			pivotB = float3::zero;
		}

		fromLoad = false;
	}

	CheckForValidConnectedBody();

	return true;
}

bool C_JointP2P::CheckForValidConnectedBody()
{
	if (connected_body)
	{
		// TODO: change to rigidbody
		if (connected_body->GetComponent<C_Collider>() == nullptr)
			connected_body = nullptr;
	}

	return true;
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

	ImGui::Title("Connected Body"); ImGui::Button(body_linked ? connected_body->GetName() : "none", ImVec2(120, 22)); ImGui::SameLine();
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
		C_Collider* col = (*go)->GetComponent<C_Collider>();

		if (col)
		{
			if (col->body)
			{
				color = color_green;
				validBody = true;
			}
		}

		ImGui::PushStyleColor(ImGuiCol_DragDropTarget, color);
		
		if (ImGui::AcceptDragDropPayload("gameobject_object") && validBody)
		{
			connected_body = *go;
		}

		ImGui::EndDragDropTarget();

		ImGui::PopStyleColor();
	}
	App->editor->HelpMarker("Drag and drop a gameobject with rigidbody component attached to link to");
	
	// --------------------
	ImGui::PushID(linked_go);
	ImGui::Title("Pivot A", 1);	
	if (ImGui::DragFloat3("##anchor", pivotA.ptr(), 0.1f))
	{
		//constraint.


	}
	ImGui::PopID();
	// linked rigibody ---
	if (!body_linked)
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
	ImGui::Title("Pivot B", 1);	ImGui::DragFloat3("##anchor", pivotB.ptr(), 0.1f);
	if(!body_linked)
		ImGui::PopStyleColor();

	ImGui::Title("Bodies Collision", 1); ImGui::Checkbox("##col", &bodies_collision);
	// --------------------

	

	return true;
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
	config.AddBool("BodiesCollision", bodies_collision);

	return true;
}
bool C_JointP2P::Load(Config& config)
{
	connected_body_id = config.GetInt("connected_body", 0);
	pivotA = config.GetFloat3("pivotA", { 0.f ,0.f, 0.f });
	pivotB = config.GetFloat3("pivotB", { 0.f ,0.f, 0.f });
	bodies_collision = config.GetBool("BodiesCollision", true);

	if (connected_body_id > 0)
		fromLoad = true;

	return true;
}

//bool C_JointP2P::