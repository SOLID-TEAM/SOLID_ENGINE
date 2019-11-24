#include "W_Project.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleEditor.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "IconFontAwesome/IconsFontAwesome5.h"
#include "Viewport.h"
#include <string>
W_Project::W_Project(std::string name, bool active) : Window(name, active) 
{

};

void W_Project::Draw()
{
	// Set selected resource ---------------------------

	if (App->editor->IsSelectedObjectValid(SelectedObject::Type::RESOURCE))
	{
		selected_resource = (Resource*)App->editor->GetSelectedObject().data;
	}
	else
	{
		selected_resource = nullptr;
	}

	// Draw Window -------------------------------------

	ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));

	if (ImGui::Begin(" " ICON_FA_FOLDER " Project", &active, ImGuiWindowFlags_::ImGuiWindowFlags_MenuBar))
	{
		if (ImGui::BeginMenuBar())
		{
			ImGui::EndMenuBar();
		}

		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ChildWindowBg, ImVec4(.2f,.2f,.2f,2.f));
		ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_WindowPadding, ImVec2(3.f, 3.f));

		static bool init_width = true;

		ImGui::BeginColumns("project_columns", 2);

		if (init_width)
		{
			ImGui::SetColumnWidth(0, ImGui::GetWindowSize().x * 0.15f);
			init_width = false;
		}

		// Draw tree ----------------------------
		if (ImGui::BeginChild("##project_tree", { 0 , 0 }, ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysUseWindowPadding))
		{
			ImGui::Spacing();
			DrawTree();
			ImGui::EndChild();
		};


		ImGui::PopStyleColor();
		ImGui::NextColumn();
		folder_column_width = ImGui::GetColumnWidth();
		// Draw inside folder --------------------
		ImGui::PopStyleVar();
		ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_WindowPadding, ImVec2(10.f, 10.f));
		if (ImGui::BeginChild("##project_view", { 0 , 0 }, ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysUseWindowPadding))
		{
			DrawFolder();
			ImGui::EndChild();
		}

		ImGui::EndColumns();
		ImGui::PopStyleVar();
	}

	ImGui::End();
	ImGui::PopStyleVar();
}

void W_Project::DrawTree()
{
	static char* assets[] = { "All Models", "All Meshes", "All Materials" ,"All Textures" };
	static char* selected = "Nothing";

	for (uint i = 0; i < 4; ++i)
	{
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_Leaf;

		if (selected == assets[i])
		{
			flags |= ImGuiTreeNodeFlags_Selected;
		}

		std::string title(ICON_FA_SEARCH " ");
		title +=  assets[i];

		bool open = ImGui::TreeNodeEx(title.c_str() , flags);

		if (ImGui::IsItemClicked())
		{
			selected = assets[i];
			if (selected	  ==  "All Models")		resource_type = Resource::Type::MODEL;
			else if (selected ==  "All Meshes")		resource_type = Resource::Type::MESH;
			else if (selected ==  "All Materials")	resource_type = Resource::Type::MATERIAL;
			else if (selected ==  "All Textures")	resource_type = Resource::Type::TEXTURE;

			visible_resources = App->resources->GetAllResourcesByType(resource_type); // TODO: Update with fix time this vector
		}

		if (open) ImGui::TreePop();
	}

	if (ImGui::IsMouseDown(0) && ImGui::IsAnyItemHovered() == false && ImGui::IsWindowHovered())
	{
		selected = "Nothing";
	}
}

void W_Project::DrawFolder()
{
	
	float total_width = folder_column_width;
	float item_width = 80 , item_height = 80;
	ImVec2 last_cursor_pos ={ 0,0 };
	float width_amount = 0;

	// Draw folder files ------------------------------

	ImGui::Columns( max(1, int (folder_column_width / (item_width + 20.f) ) ), "project_folder_columns", false);

	for (Resource* resource : visible_resources)
	{
		ImGui::PushID(resource);

		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, (resource == selected_resource) ? ImVec4(1.f, 0.75f, 0.f, 1.f) : ImVec4(0.f, 0.f, 0.f, 0.f));
		// TODO: Change texture
		ImGui::ImageButton((ImTextureID)App->scene->scene_viewport->GetTexture(), { item_width,item_height }, { 1,1 }, { 0,0 });
		ImGui::PopStyleColor();

		if (ImGui::IsItemClicked())
		{
			App->editor->SetSelectedObject(resource, SelectedObject::Type::RESOURCE);
			// TODO: Select resource and show info
		}
		if (ImGui::IsMouseDoubleClicked(0))
		{
			// TODO:Create model
		}

		// Drag  same as double click ?
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceNoDisableHover)) 
		{
			ImGui::SetDragDropPayload("resource_node", &resource, sizeof(Resource), ImGuiCond_Once);
			ImGui::Text(resource->GetName().c_str());

			ImGui::EndDragDropSource();
		}

	

		// Copy this to target window
		if (ImGui::BeginDragDropTarget())
		{
			const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("resource_node");

			if (payload != nullptr && payload->Data != nullptr)
			{
				/*ResoruceModel* model = (ResoruceModel*)ImGui::GetDragDropPayload()->Data;*/
			}

			ImGui::EndDragDropTarget();
		}


		ImGui::NewLine();
		ImGui::SameLine();

		ImGui::Text(resource->GetName().c_str());

		ImGui::Spacing();
		ImGui::NextColumn();

		ImGui::PopID();
	}

	if (ImGui::IsMouseDown(0) && ImGui::IsAnyItemHovered() == false && ImGui::IsWindowHovered())
	{
		App->editor->DeselectSelectedObject();
	}
}
