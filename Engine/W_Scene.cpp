#include "W_Scene.h"

#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleResources.h"
#include "ModuleInput.h"

#include "CameraEditor.h"
#include "Viewport.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "ImGuizmo/ImGuizmo.h"
#include "IconFontAwesome/IconsFontAwesome5.h"

#include "C_Camera.h"
#include "Resource.h"
#include "R_Model.h"

void W_Scene::Draw()
{
	ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));

	if (ImGui::Begin(" " ICON_FA_CUBES " Scene", &active , ImGuiWindowFlags_::ImGuiWindowFlags_MenuBar  /*| ImGuiWindowFlags_::ImGuiWindowFlags_NoDecoration*/))
	{
		if (ImGui::BeginMenuBar())
		{	
			ImGui::SetSeparationType(ImGuiSeparationType::ImGui_MenuSeparation);
			ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_WindowPadding, ImVec2(10.f, 10.f));

			ModesMenu();

			if (ImGui::BeginMenu(ICON_FA_BORDER_ALL))
			{
				GridMenu();
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu(ICON_FA_VIDEO))
			{
				CameraMenu();
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu(ICON_FA_EYE))
			{
				DebugMenu();
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();

			ImGui::PopStyleVar();
			ImGui::SetSeparationType(ImGuiSeparationType::ImGui_WindowSeparation);
		}

		// Drag & Drop -------------------------------------------------------

		if (ImGui::BeginDragDropTargetCustom(ImGui::GetCurrentWindow()->Rect(), ImGui::GetID("Scene_Window")))
		{
			const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("resource_node");

			if (payload != nullptr && payload->Data != nullptr)
			{
				Resource** res = (Resource**)ImGui::GetDragDropPayload()->Data;

				if ((*res)->GetType() == Resource::Type::MODEL)
				{
					App->scene->CreateGameObjectFromModel((*res)->GetUID());
				}
			}

			ImGui::EndDragDropTarget();
		}

		// Input camera ----------------------------------------------------

		is_focused = ImGui::IsWindowFocused();
		viewport_min = ImGui::GetCursorScreenPos();
		viewport_max = ImGui::GetCursorScreenPos() + ImGui::GetContentRegionAvail();

		bool mouse_is_hovering = ImGui::IsMouseHoveringRect(viewport_min, viewport_max);

		if (App->scene->editor_camera)
		{
			if (!App->scene->editor_camera->mouse_right_pressed && !App->scene->editor_camera->mouse_left_pressed)
			{
				App->scene->editor_camera->enable_mouse_input = mouse_is_hovering;
			}
		}

		// Attach texture to window ----------------------------------------

		ImVec2 current_viewport_size = ImGui::GetContentRegionAvail();

		ImGui::Image((ImTextureID)App->scene->scene_viewport->GetTexture(), ImVec2(current_viewport_size.x, current_viewport_size.y), ImVec2(0, 1), ImVec2(1, 0));

		// Set Guizmo Config ----------------------------------------------

		ImGuizmo::SetRect(viewport_min.x, viewport_min.y, current_viewport_size.x, current_viewport_size.y);
		ImGuizmo::SetDrawlist();

		if (App->scene->selected_go != nullptr)
		{
			ImGuizmo::DrawGizmo();
		}

		// Set viewport values --------------------------------------------

		App->scene->scene_viewport->SetPos(float2(viewport_min.x, viewport_min.y)); // Viewport position after menu bar 

		if (!(current_viewport_size == viewport_size)) // Viewport resize if window size change
		{
			viewport_size = current_viewport_size;
			App->scene->scene_viewport->SetSize(viewport_size.x, viewport_size.y);
		}
	}


	App->scene->scene_viewport->active = active; // Active/disavtive with window active

	//    --------------------------------------------------------------------

	ImGui::PopStyleVar();
	ImGui::End();
}

void W_Scene::ModesMenu()
{
	ViewportOptions& vp = App->editor->viewport_options;
	static const char* items[] = { "Shaded", "Wireframe", "Shaded Wireframe", "Depth View" };
	static const char* current_item = items[0];

	ImGui::SetCursorPosX(0);
	ImGui::SetNextItemWidth(140);

	if (ImGui::BeginComboEx(std::string("##viewport_modes").c_str(), std::string(" " + std::string(current_item)).c_str(), 200, ImGuiComboFlags_NoArrowButton))
	{
		for (int n = 0; n < IM_ARRAYSIZE(items); ++n)
		{
			bool is_selected = (current_item == items[n]);

			if (ImGui::Selectable(std::string("   " + std::string(items[n])).c_str(), is_selected))
			{
				current_item = items[n];

				if		(current_item == "Shaded")				vp.mode = V_MODE_SHADED;
				else if (current_item == "Wireframe")			vp.mode = V_MODE_WIREFRAME;
				else if (current_item == "Shaded Wireframe")	vp.mode = V_MODE_SHADED_WIREFRAME;
				else if (current_item == "Depth View")			vp.mode = V_MODE_DEPTH_FILTER;
			}

			if (is_selected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}

		ImGui::EndCombo();
	}
}


void W_Scene::GridMenu()
{
	RenderConfig& config = App->renderer3D->render_config;
	static int units = 15;
	ImGui::Title("Active", 1);		ImGui::Checkbox("##active_grid", &App->test->main_grid->active);
	ImGui::Title("Color", 2);		ImGui::ColorEdit4("##grid_color", App->test->main_grid->color.ptr(), ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
	ImGui::Title("Opacity", 2);		ImGui::SetNextItemWidth(150);  ImGui::SliderFloat("##grid_opacity", &App->test->main_grid->color.w, config.min_alpha, config.max_alpha, "%.1f", 1.0f);
	ImGui::Title("Distance", 2);    if (ImGui::SliderInt("##grid_units", &units, 1, 50)) App->test->main_grid->SetUnits(units);
	ImGui::Title("Line Width", 2);	ImGui::SliderFloat("##grid_width", &App->test->main_grid->line_width, config.min_line_w, config.max_line_w, "%.1f", 1.0f);
}

void W_Scene::DebugMenu()
{
	RenderConfig& config = App->renderer3D->render_config;
	ViewportOptions& vp = App->editor->viewport_options;

	ImGui::Title("Bounding Box", 1);	ImGui::Checkbox("##bb_enable", &vp.debug_bounding_box);
	ImGui::Title("Color", 2);			ImGui::ColorEdit4("##bb_color", (float*)&vp.bb_color, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
	ImGui::Title("Width", 2);			ImGui::SliderFloat("##bb_line_width", &vp.bb_line_width, config.min_line_w, config.max_line_w, "%.1f", 1.0f);
	ImGui::Spacing();

	ImGui::Separator();

	ImGui::Spacing();
	ImGui::Title("Wireframe", 1);	ImGui::Text("");
	ImGui::Title("Color", 2);		ImGui::ColorEdit4("Line Color##2f", (float*)&vp.wire_color, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
	ImGui::Title("Width", 2);		ImGui::SliderFloat("##wire_line_width", &vp.wire_line_width, config.min_line_w, config.max_line_w, "%.1f", 1.0f);
	ImGui::Spacing();

	ImGui::Separator();

	ImGui::Spacing();
	ImGui::Title("Normals", 1);			ImGui::Text("");
	ImGui::Title("Vertices", 2);		ImGui::Checkbox("##d_vertex_normals", &vp.debug_vertex_normals);
	ImGui::Title("Point Color", 2);		ImGui::ColorEdit4("Vertex Point Color##2f", (float*)&vp.d_vertex_p_color, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
	ImGui::Title("Point Size",  2);		ImGui::SetNextItemWidth(150);  ImGui::SliderFloat("##d_vertex_p_size", &vp.v_point_size, config.min_point_size, config.max_point_size, "%.1f", 1.0f);
	ImGui::Title("Line Color",  2);		ImGui::ColorEdit4("Vertex Line Color##2f", (float*)&vp.d_vertex_l_color, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
	ImGui::Title("Line Width",  2);		ImGui::SliderFloat("##d_vertex_line_width", &vp.v_n_line_width, config.min_line_w, config.max_line_w, "%.1f", 1.0f);
	ImGui::Title("Line Lenght", 2);

	//if (ImGui::SliderFloat("##d_vertex_line_lenght", &vp.v_n_line_length, config.min_n_length, config.max_n_length, "%.1f", 1.0f))
	//{
	//	if (vp.debug_vertex_normals)
	//	{
	//		if (App->editor->ddmesh != nullptr)
	//		{
	//			App->editor->ddmesh->ComputeVertexNormals(App->scene->selected_go->GetMeshes(), vp.v_n_line_length);
	//			App->editor->ddmesh->FillVertexBuffer();
	//		}
	//	}
	//}

	ImGui::Spacing();
	ImGui::Spacing();

	ImGui::Title("Faces", 2);			ImGui::Checkbox("##d_face_normals", &vp.debug_face_normals);
	ImGui::Title("Point Color", 2);		ImGui::ColorEdit4("Face Point Color##2f", (float*)&vp.d_vertex_face_color, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
	ImGui::Title("Point Size",  2);		ImGui::SliderFloat("##d_face_p_size", &vp.f_v_point_size, config.min_point_size, config.max_point_size, "%.1f", 1.0f);
	ImGui::Title("Line Color",  2);		ImGui::ColorEdit4("Face Line Color##2f", (float*)&vp.d_vertex_face_n_color, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
	ImGui::Title("Line Width",  2);		ImGui::SliderFloat("##d_face_line_width", &vp.f_n_line_width, config.min_line_w, config.max_line_w, "%.1f", 1.0f);
	ImGui::Title("Line Lenght", 2);

	//if (ImGui::SliderFloat("##d_face_line_lenght", &vp.f_n_line_length, config.min_n_length, config.max_n_length, "%.1f", 1.0f))
	//{
	//	if (vp.debug_face_normals)
	//	{
	//		if (App->editor->ddmesh != nullptr)
	//		{
	//			App->editor->ddmesh->ComputeFacesNormals(App->scene->selected_go->GetMeshes(), vp.f_n_line_length);
	//			App->editor->ddmesh->FillFacesBuffer();
	//		}
	//	}
	//}
}

void W_Scene::CameraMenu()
{
	C_Camera* editor_camera = App->scene->editor_camera->GetComponent<C_Camera>();

	math::FrustumType type = editor_camera->GetFrustumType();
	float fov = editor_camera->GetFov();
	float near_plane = editor_camera->GetClippingNearPlane();
	float far_plane = editor_camera->GetClippingFarPlane();

	math::FrustumType last_type = type;
	float last_fov = fov;
	float last_near_plane = near_plane;
	float last_far_plane = far_plane;

	static const char* items[] = { "Perspective", "Orthographic" };
	static const char* current_item = items[0];

	ImGui::Spacing();

	ImGui::Title("Projection", 1);

	if (ImGui::BeginComboEx(std::string("##frustum_type").c_str(), std::string(" " + std::string(current_item)).c_str(), 120, ImGuiComboFlags_NoArrowButton))
	{
		for (int n = 0; n < IM_ARRAYSIZE(items); ++n)
		{
			bool is_selected = (current_item == items[n]);

			if (ImGui::Selectable(std::string("   " + std::string(items[n])).c_str(), is_selected))
			{
				current_item = items[n];

				if (current_item == "Perspective")			type = math::FrustumType::PerspectiveFrustum;
				else if (current_item == "Orthographic")	type = math::FrustumType::OrthographicFrustum;
			}

			if (is_selected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	ImGui::Title("Field of View", 1);	ImGui::SetNextItemWidth(150); ImGui::SliderFloat("##fov", &fov, 4.f, 179.f, "%.1f");
	ImGui::Title("Clipping", 1);  ImGui::Text("");

	ImGui::Spacing();
	ImGui::Title("Near Plane", 3);	ImGui::DragFloat("##near_plane", &near_plane, 0.1f, 0.00f, 0.f, "%.1f");
	ImGui::Title("Far Plane", 3);	ImGui::DragFloat("##far_plane", &far_plane, 0.1f, 0.00f, 0.f, "%.1f");
	ImGui::Spacing();

	if (last_type != type)
	{
		editor_camera->SetFrustumType(type);
		ImGuizmo::SetOrthographic((type == FrustumType::OrthographicFrustum) ? true : false);
	}

	if (last_fov != fov)
	{
		editor_camera->SetFov(fov);
	}

	if (last_near_plane != near_plane)
	{
		editor_camera->SetClippingNearPlane(near_plane);
	}

	if (last_far_plane != far_plane)
	{
		editor_camera->SetClippingFarPlane(far_plane);
	}
}
