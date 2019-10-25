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
	RenderConfig& config = App->renderer3D->render_config;
	ViewportOptions& vp = App->editor->viewport_options;

	ImGui::Title("Fill Faces", 1);	ImGui::Checkbox("##fill_mode", &vp.fill_faces);
	ImGui::Title("Color", 2);		ImGui::ColorEdit4("fILL Color##2f", (float*)&vp.fill_color, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
	ImGui::Title("Alpha", 2);		ImGui::SliderFloat("##alpha_slider", &vp.fill_color.w, config.min_alpha, config.max_alpha, "%.1f", 1.0f);
	ImGui::Separator();
	ImGui::Title("Wireframe", 1);	ImGui::Checkbox("##wireframe", &vp.wireframe);
	ImGui::Title("Color", 2);		ImGui::ColorEdit4("Line Color##2f", (float*)&vp.wire_color, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
	ImGui::Title("Width", 2);		ImGui::SliderFloat("##wire_line_width", &vp.wire_line_width, config.min_line_w, config.max_line_w, "%.1f", 1.0f);
	
	ImGui::Separator();
	ImGui::Title("Textured", 1);	ImGui::Checkbox("##textured", &textured);

	ImGui::Separator();
	ImGui::Title("Material color", 1); ImGui::ColorEdit4("Color##2f", (float*)&data->albedo_color, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
	ImGui::SameLine();
	App->editor->HelpMarker("Used only when the selected gameObject has no texture or textured mode is unchecked");
	
	ImGui::Separator();
	App->editor->ShowCheckerTexture(checker_gl_id, view_checker);

	return true;
}
