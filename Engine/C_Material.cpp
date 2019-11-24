#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleEditor.h"

#include "C_Material.h"
#include "R_Material.h"

C_Material::C_Material(GameObject* parent) : Component(parent, ComponentType::MATERIAL)
{
	name.assign("Material");
}

bool C_Material::DrawPanelInfo()
{
	R_Material* r = (R_Material*)App->resources->Get(resource);
	if (r == nullptr)
		return false;

	R_Texture* tex = static_cast<R_Texture*>(App->resources->Get(r->textures[R_Material::TextureType::DIFFUSE]));

	ImGui::Spacing();
	ImGui::Title("Name", 1);		ImGui::Text("%s", r->GetName().c_str());
	ImGui::Title("Resource", 1);	ImGui::Text("%i", r->GetUID());
	ImGui::Title("Instances", 1);	ImGui::Text("%i", r->CountReferences());
	ImGui::Separator();

	ImGui::Spacing();
	ImGui::Title("Textured", 1);	ImGui::Checkbox("##textured", &textured);

	ImGui::Separator();
	ImGui::Title("Diffuse", 1); ImGui::ColorEdit4("Color##2f", (float*)&r->diffuse_color, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
	ImGui::SameLine();
	App->editor->HelpMarker("Used only when the selected gameObject has no texture or textured mode is unchecked");

	// //TODO: rework how the texture is shown and add more options
	if (tex != nullptr)
	{
		if(ImGui::BeginChildFrame(1, ImVec2(ImGui::GetWindowContentRegionWidth(), 200)))
		{
			if (ImGui::BeginChild(1, ImVec2(ImGui::GetWindowContentRegionWidth() * 0.5f, 192) , true))
			{
				ImVec4 color = ImVec4(0.2f, 1.0f, 0.2f, 1.0f);
				// get texture data
				ImGui::Text("Texture type: "); ImGui::SameLine(); ImGui::TextColored(color, "%s", "DIFFUSE"); // TODO: get texture type from resource texture
				ImGui::Text("Format: "); ImGui::SameLine(); ImGui::TextColored(color, "%i",(int)tex->format);
				ImGui::Text("Width: "); ImGui::SameLine(); ImGui::TextColored(color, "%i", tex->width);
				ImGui::Text("Height: "); ImGui::SameLine(); ImGui::TextColored(color, "%i", tex->height);
				ImGui::Text("Bpp: "); ImGui::SameLine(); ImGui::TextColored(color, "%i", tex->bpp);
				ImGui::Text("Buffer id: "); ImGui::SameLine(); ImGui::TextColored(color, "%i", tex->buffer_id);
				//ImGui::Text("Referenced: "); ImGui::SameLine(); ImGui::TextColored(color, "%i", tex->CountReferences());
				
				ImGui::EndChild();
			}
			ImGui::SameLine();
			if (ImGui::BeginChild(2, ImVec2((ImGui::GetWindowContentRegionWidth() * 0.5f) - 8.0f, 192), true)) // 8 default padding
			{
				//ImGui::Separator();
				ImGui::Image((ImTextureID)tex->buffer_id, ImVec2(170, 170),ImVec2(0,1), ImVec2(1, 0));

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
	Resource* r = App->resources->Get(resource);
	if (r)
		r->Release();

	return true;
}

bool C_Material::Save(Config& config)
{
	
	config.AddInt("Resource", resource);
	config.AddBool("Active", active);


	return true;
}

bool C_Material::Load(Config& config)
{
	SetMaterialResource(config.GetInt("Resource", resource));
	active = config.GetBool("Active", active);

	return true;
}

bool C_Material::SetMaterialResource(UID uid)
{
	bool ret = false;

	// if any resource attached
	if (resource != 0)
	{
		Resource* r = App->resources->Get(resource);
		if (r != nullptr)
			r->Release();
	}

	if (uid != 0)
	{
		Resource* new_r = App->resources->Get(uid);
		if (new_r != nullptr)
		{
			if (new_r->GetType() == Resource::Type::MATERIAL)
			{
				R_Material* m =  static_cast<R_Material*>(new_r);

				if (m->LoadToMemory())
				{
					resource = uid;

					// check if texture exists on resources
					R_Texture* tex = static_cast<R_Texture*>(App->resources->Get(m->textures[R_Material::TextureType::DIFFUSE]));
					if (tex)
						textured = true;

					ret = true;
				}
			}
		}
		else
		{
			LOG("[Error] Resource not found");
		}

	}

	return ret;
}

