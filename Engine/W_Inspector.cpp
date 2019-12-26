#include "Application.h"
#include "ModuleEditor.h"
#include "W_Inspector.h"
#include "ImGui/imgui.h"
#include "GameObject.h"
//#include "Component.h"
#include "Resource.h"
#include <vector>
#include "ImGui/misc/cpp/imgui_stdlib.h" // input text wrapper for std::string


void W_Inspector::Draw()
{
	if (ImGui::Begin(name.c_str(), &active))
	{
		if (App->editor->IsSelectedObjectValid(SelectedObject::Type::GAME_OBJECT))
		{
			DrawGameObjectInfo();
		}
		else if (App->editor->IsSelectedObjectValid(SelectedObject::Type::RESOURCE))
		{
			DrawResourceInfo();
		}
	}

	ImGui::End();
}

// Print all info using components
// each component already has a draw elements for its own data
// here only call this

void W_Inspector::DrawGameObjectInfo()
{
	GameObject* go = (GameObject*)App->editor->GetSelectedObject().data;

	if (go == nullptr)
		return;

	ImGui::Spacing();
	if (ImGui::Checkbox("##active", &go->active))
	{
		App->scene->PushEvent(go, go->active ? EventGoType::ACTIVATE : EventGoType::DEACTIVATE);
	}
	ImGui::SameLine(); 
	ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionMax().x - ImGui::GetCursorPosY() - 85.F);  ImGui::InputText("##etc", &go->name); 
	ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 75.F);

	bool go_is_static = go->is_static;

	if (ImGui::Checkbox("Static", &go_is_static))
	{
		if (go->childs.empty())
		{
			go->SetIsStatic(!go->is_static);
		}
		else
		{
			ImGui::OpenPopup("static_popup");
		}

	}

	DrawStaticPopUp();

	ImGui::Spacing();

	// -----------------------------------------------------------------

	// iterate each component and draw if it contains something to draw
	for (std::vector<Component*>::const_iterator components = go->GetComponents().begin();
		components != go->GetComponents().end(); ++components)
	{
		// TODO: find another way to store individual go opened/closed collapsingheader
		//ImGui::SetNextTreeNodeOpen(!(*components)->collapsed); 

		bool aux = ImGui::CollapsingHeader(("   " + (*components)->name).c_str(), (*components)->flags);

		if ((*components)->flags & ImGuiTreeNodeFlags_AllowItemOverlap)
		{
			ImGui::SameLine(30.f);  ImGui::Checkbox(("##enable" + (*components)->name).c_str(), &(*components)->enable);
		}

		if (aux)
		{
			(*components)->DrawPanelInfo();
			//(*components)->collapsed = false;
		}
		/*else
			(*components)->collapsed = true;*/
	}

	DrawAddComponents(go);

}

void W_Inspector::DrawAddComponents(GameObject* selected_go)
{
	ImGui::Separator();

	ImVec2 window_size = ImVec2(230,120);
	ImGui::SetCursorPosX((ImGui::GetWindowWidth() - window_size.x) * 0.5f);
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);
	{
		//ImGuiWindowFlags window_flags = ImGuiWindowFlags_HorizontalScrollbar;
		//ImGui::SetNextWindowPosCenter(ImGuiCond_Always);
		
		ImGui::BeginChild("Child1", window_size);//ImVec2(ImGui::GetWindowContentRegionWidth() * 0.5f, 260), false, window_flags);
		
		if(ImGui::Button("Add Component", ImVec2(ImGui::GetWindowWidth(), 22)))
			ImGui::OpenPopup("add_component_popup");
		//ImGui::SameLine();
		// TODO: get available space on y, if less than needed window size, substract size.y from pos.y
		ImVec2 win_pos = ImGui::GetWindowPos();
		ImGui::SetNextWindowPos(ImVec2(win_pos.x, win_pos.y + 22));
		ImGui::SetNextWindowSize(window_size);
		
		if (ImGui::BeginPopup("add_component_popup", ImGuiWindowFlags_NoScrollbar))
		{
			const char* names[] = { "Transform", "Mesh", "Material", "Camera", "Light","Mesh Renderer","Box Collider", "Sphere Collider", "Capsule Collider","RigidBody" };
			
			uint size = IM_ARRAYSIZE(names);

			ImGui::SetCursorPosX((ImGui::GetWindowWidth() - strlen("Component") * 8) * 0.5f);
			ImGui::Text("Component");
			ImGui::Separator();
			ImGui::BeginChild("CompoChild", ImVec2(0, 80));

			for (int i = 0; i < size; i++)
			{
				//if (ImGui::Selectable(GetNameFromComponentType(ComponentType(i)).c_str()))
				if (ImGui::Selectable(names[i]))
				{
					//TODO: create any component
					// test
					if (names[i] == "Box Collider")
					{
						selected_go->AddComponent<C_BoxCollider>();
					}
					else if (names[i] == "Sphere Collider")
					{
						selected_go->AddComponent<C_SphereCollider>();
					}
				}
					
			}
			ImGui::EndChild();
			ImGui::EndPopup();
		}

		ImGui::EndChild();
	}
}


void W_Inspector::DrawStaticPopUp()
{
	if (ImGui::BeginPopupModal("static_popup", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		GameObject* go = App->scene->selected_go;

		ImGui::Text("Do you want to enable static flag for all the child objects as well?");

		if (ImGui::Button("Yes, change children"))
		{
			go->SetIsStaticToHierarchy(!go->is_static);
			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();

		if (ImGui::Button("No, this object only"))
		{
			go->SetIsStatic(!go->is_static);
			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();

		if (ImGui::Button("Cancel"))
		{
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

void W_Inspector::DrawResourceInfo()
{
	Resource* res = (Resource*)App->editor->GetSelectedObject().data;

	if (res == nullptr)
		return;

	ImGui::Spacing();

	ImGui::Title("Name");				ImGui::Text( "%s", res->GetName().c_str());
	ImGui::Title("References");			ImGui::Text( "%u", res->CountReferences());
	ImGui::Title("Original File");		ImGui::Text("%s", res->GetOriginalFile().c_str());
	ImGui::Title("Exported File");		ImGui::Text("%s", res->GetExportedFile().c_str());
}

