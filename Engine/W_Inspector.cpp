#include "Application.h"
#include "ModuleEditor.h"
#include "W_Inspector.h"
#include "ImGui/imgui.h"
#include "GameObject.h"
#include "Resource.h"
#include "Component.h"
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
	ImGui::Checkbox("##active", &go->active); ImGui::SameLine(); ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionMax().x - ImGui::GetCursorPosY() - 85.F);  ImGui::InputText("##etc", &go->name); ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 75.F);

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
			ImGui::SameLine(30.f);  ImGui::Checkbox(("##active" + (*components)->name).c_str(), &(*components)->active);
		}

		if (aux)
		{
			(*components)->DrawPanelInfo();
			//(*components)->collapsed = false;
		}
		/*else
			(*components)->collapsed = true;*/
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

