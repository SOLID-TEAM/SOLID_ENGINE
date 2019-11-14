#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleEditor.h"

#include "C_Material.h"
#include "D_Material.h"

C_Material::C_Material(GameObject* parent) : Component(parent, ComponentType::MATERIAL)
{
	name.assign("Material");
}

bool C_Material::DrawPanelInfo()
{
	// TODO: individual data for each gameobject/d_mesh

	ImGui::Spacing();
	ImGui::Title("Textured", 1);	ImGui::Checkbox("##textured", &textured);

	ImGui::Separator();
	ImGui::Title("Diffuse", 1); ImGui::ColorEdit4("Color##2f", (float*)&data->diffuse_color, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
	ImGui::SameLine();
	App->editor->HelpMarker("Used only when the selected gameObject has no texture or textured mode is unchecked");

	// TODO: rework how the texture is shown and add more options
	if (data->textures[0] != nullptr)
	{
		if(ImGui::BeginChildFrame(1, ImVec2(ImGui::GetWindowContentRegionWidth(), 200)))
		{
			if (ImGui::BeginChild(1, ImVec2(ImGui::GetWindowContentRegionWidth() * 0.5f, 192) , true))
			{
				ImVec4 color = ImVec4(0.2f, 1.0f, 0.2f, 1.0f);
				std::string format;
				std::string texName;
				App->file_sys->SplitFilePath(App->textures->GetTextureName(data->textures[0]->buffer_id), nullptr, &texName, &format);
				// get texture data
				int w = 0, h = 0;
				App->textures->GetTextureSize(data->textures[0]->buffer_id, w, h);
				ImGui::Text("Texture type: "); ImGui::SameLine(); ImGui::TextColored(color, "%s", "DIFFUSE"); // TODO: get texture type from resource texture
				ImGui::Text("Name: "); ImGui::SameLine(); ImGui::TextColored(color, "%s", texName.c_str());
				ImGui::Text("Format: "); ImGui::SameLine(); ImGui::TextColored(color, "%s",format.c_str());
				ImGui::Text("Width: "); ImGui::SameLine(); ImGui::TextColored(color, "%i", w);
				ImGui::Text("Height: "); ImGui::SameLine(); ImGui::TextColored(color, "%i", h);
				ImGui::Text("Buffer id: "); ImGui::SameLine(); ImGui::TextColored(color, "%i", data->textures[0]->buffer_id);
				
				ImGui::EndChild();
			}
			ImGui::SameLine();
			if (ImGui::BeginChild(2, ImVec2((ImGui::GetWindowContentRegionWidth() * 0.5f) - 8.0f, 192), true)) // 8 default padding
			{
				//ImGui::Separator();
				ImGui::Image((ImTextureID)data->textures[0]->buffer_id, ImVec2(170, 170),ImVec2(0,1), ImVec2(1, 0));

				ImGui::EndChild();
			}

			ImGui::EndChildFrame();
		}

	}
	
	ImGui::Separator();
	App->editor->ShowCheckerTexture(checker_gl_id, view_checker);

	ImGui::Spacing();


	return true;
}

bool C_Material::CleanUp()
{
	// DELETES only the data, not free the texture itself (TODO: warning for another objects with same texture)
	if(data->textures!= nullptr)
		delete data->textures[0]; // TODO: currently only diffuse channel (0) 
	
	delete data;

	return true;
}

bool C_Material::Save(Config& config)
{
	// TODO: resources
	int resource_test = 2213;
	config.AddInt("Resource", resource_test);
	config.AddBool("Active", active);


	return true;
}

bool C_Material::Load(Config& config)
{

	return true;
}