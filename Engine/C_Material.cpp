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