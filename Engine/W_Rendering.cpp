#include "GL/glew.h"
#include "Application.h"
#include "W_Rendering.h"
#include "ImGui/imgui.h"
#include "ModuleRenderer3D.h"

void W_Rendering::Draw()
{
	RenderConfig& config = App->renderer3D->render_config;

	if (ImGui::Begin(name.c_str(), &active))
	{
		//if (ImGui::CollapsingHeader("OpenGL Test", ImGuiTreeNodeFlags_DefaultOpen))
		//{
		//	ImGui::Title("Color Material");	
		//	if (ImGui::Checkbox("##GL_COLOR_MATERIAL", &config.gl_color_material))
		//	{
		//		if (config.gl_color_material)
		//			glEnable(GL_COLOR_MATERIAL);
		//		else
		//		{
		//			App->renderer3D->SetDefaultColorMaterial();
		//			glDisable(GL_COLOR_MATERIAL);
		//		}
		//			
		//	}
		//	ImGui::Title("Depht Test");		
		//	if (ImGui::Checkbox("##GL_DEPTH_TEST", &config.gl_depth_test))
		//	{
		//		if (config.gl_depth_test)
		//			glEnable(GL_DEPTH_TEST);
		//		else
		//		{
		//			glDisable(GL_DEPTH_TEST);
		//		}
		//	}
		//	ImGui::Title("Cull Faces");		
		//	if (ImGui::Checkbox("##GL_CULL_FACE", &config.gl_cull_face))
		//	{
		//		if (config.gl_cull_face)
		//		{
		//			glEnable(GL_CULL_FACE);
		//		}
		//		else
		//			glDisable(GL_CULL_FACE);
		//	}
		//	ImGui::Title("Lighting");		
		//	if (ImGui::Checkbox("##GL_LIGHTING", &config.gl_lighting))
		//	{
		//		if (config.gl_lighting)
		//		{
		//			glEnable(GL_LIGHTING);
		//		}
		//		else
		//			glDisable(GL_LIGHTING);
		//	}
		//}
	}

	ImGui::End();
}
