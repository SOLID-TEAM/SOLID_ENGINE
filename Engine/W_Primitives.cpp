#include "Application.h"
#include "W_Primitives.h"
#include "ImGui/imgui.h"

#include "ModuleImporter.h"
#include "C_Material.h"
#include "D_Material.h"

W_Primitives::W_Primitives(std::string name, bool active) : Window(name, active)
{
	// TODO: add/get the folder from filesystem 
	// THIS ORDER MATTERS, for simplicity we follow the same order as enumerator PrimitiveType
	tex_ids.push_back(App->textures->LoadTexture("Project/Editor/Images/cube.png"));
	tex_ids.push_back(App->textures->LoadTexture("Project/Editor/Images/sphere.png"));
	tex_ids.push_back(App->textures->LoadTexture("Project/Editor/Images/icosphere.png"));
	tex_ids.push_back(App->textures->LoadTexture("Project/Editor/Images/plane.png"));
	tex_ids.push_back(App->textures->LoadTexture("Project/Editor/Images/cylinder.png"));
	tex_ids.push_back(App->textures->LoadTexture("Project/Editor/Images/connus.png"));
	tex_ids.push_back(App->textures->LoadTexture("Project/Editor/Images/torus.png"));
}

void W_Primitives::Draw()
{
	static ImVec4 color = ImVec4(220.0f / 255.0f, 220.0f / 255.0f, 220.0f / 255.0f, 255.0f / 255.0f);
	ImVec2 buttonSize(100, 100);

	ImGuiStyle& style = ImGui::GetStyle();

	ImVec2 tempSize = style.WindowMinSize;

	style.WindowMinSize = ImVec2(1, 1);

	//ImGui::SetNextWindowSizeConstraints(ImVec2(80, 300), ImVec2(80, 400)); // min and max

	bool last_state = active;

	ImGui::SetNextItemWidth(120);
	if (ImGui::Begin("Primitives Creator", &active, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize));
	{
		int counter = 0;
		for (std::vector<uint>::const_iterator miniatures = tex_ids.begin(); miniatures != tex_ids.end(); ++miniatures)
		{
			if (counter % 2 != 0) ImGui::SameLine();

			if (ImGui::ImageButton((ImTextureID)(*miniatures), buttonSize, ImVec2(0, 1), ImVec2(1, 0)))
			{
				last_created = App->importer->CreatePrimitive(PrimitiveType(counter), { 0,0,0 }, { 1,1,1 }, GetSlicesAndStacks(counter), {color.x, color.y, color.z, color.w});
			}

			++counter;
		}

		
		
		ImGuiColorEditFlags flags =  ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar;
		ImGuiColorEditFlags misc_flags = ImGuiColorEditFlags_AlphaPreviewHalf | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoTooltip; //  ImGuiColorEditFlags_NoDragDrop | 
		//if (!alpha)            flags |= ImGuiColorEditFlags_NoAlpha;        // This is by default if you call ColorPicker3() instead of ColorPicker4()
		//if (alpha_bar)         flags |= ImGuiColorEditFlags_AlphaBar;
		//if (!side_preview)     flags |= ImGuiColorEditFlags_NoSidePreview;
		//if (picker_mode == 1)  flags |= ImGuiColorEditFlags_PickerHueBar;
		//if (picker_mode == 2)  flags |= ImGuiColorEditFlags_PickerHueWheel;
		//if (display_mode == 1) flags |= ImGuiColorEditFlags_NoInputs;       // Disable all RGB/HSV/Hex displays
		//if (display_mode == 2) flags |= ImGuiColorEditFlags_DisplayRGB;     // Override display mode
		//if (display_mode == 3) flags |= ImGuiColorEditFlags_DisplayHSV;
		//if (display_mode == 4) flags |= ImGuiColorEditFlags_DisplayHex;
		ImGui::SameLine();
		
		ImGui::ColorButton("MatColor##3c", *(ImVec4*)&color, misc_flags, buttonSize + style.FramePadding * 2.0f);
		//ImGui::SetNextItemWidth(80);
		if (ImGui::ColorPicker4("##matPicker", (float*)&color, flags))
		{
			// TODO: when we delete gameobjects, on preupdate check all linked go's of each module if anyone is going to delete
			if (last_created != nullptr)
			{
				C_Material* c_mat = (C_Material*)last_created->GetComponentsByType(ComponentType::MATERIAL);
				c_mat->data->diffuse_color = { color.x, color.y, color.z, color.w};
			}
		}
		if (last_state != active)
		{
			LOG("Closed Primitives Panel");
			last_created = nullptr;
		}
			
	}

	ImGui::End();

	// restore previous settings
	// TODO: better constraint every window specific with setnextwindowsiszeconstraints (for all panels)
	style.WindowMinSize = tempSize;
}

// return default values for primitives creation
float2 W_Primitives::GetSlicesAndStacks(int i)
{
	float2 ret = { 10,10 };

	switch (PrimitiveType(i))
	{
	case CUBE:
		//ret = { 1,1 };
		break;
	case SPHERE:
		ret = { 20,20 };
		break;
	case ICOSPHERE:
		//ret = { 1,1 };
		break;
	case PLANE:
		ret = { 1,1 };
		break;
	case CYLINDER:
		ret = { 20,1 };
		break;
	case CONE:
		ret = { 10,1 };
		break;
	case TORUS:
		ret = { 20,20 };
		break;
	case MAX:
		break;
	default:
		break;
	}


	return ret;
}

W_Primitives::~W_Primitives()
{
}