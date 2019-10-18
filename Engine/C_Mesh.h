#pragma once

#include "Component.h"
#include "ModelData.h"
#include "ImGui/imgui.h"

class ComponentMesh : public Component
{
public:
	ComponentMesh(GameObject* parent);
	~ComponentMesh();

	bool Update(float dt);
	bool PostUpdate(float dt);
	bool Draw();
	bool DrawPanelInfo();
	bool CleanUp();

public:
	ModelData* mesh = nullptr;

	/*bool wireframe = false;
	bool outline = false;
	bool fill_faces = true;
	bool debug_vertex_normals = true;
	bool debug_face_normals = true;
	ImVec4 fill_color = ImVec4(1.0f, 0.35f, 1.0f, 1.0f);
	ImVec4 wire_color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);;
	ImVec4 d_vertex_p_color = ImVec4(0.59f, 0.22f, 1.0f, 1.0f);
	ImVec4 d_vertex_l_color = ImVec4(0.2f, 1.0f, 0.0f, 1.0f);
	ImVec4 d_vertex_face_color = ImVec4(1.0f, 0.5f, 0.2f, 1.0f);
	ImVec4 d_vertex_face_n_color = ImVec4(0.0f, 0.5f, 1.0f, 1.0f);

	float wire_line_width = 1.0f;
	float v_point_size = 5.0f;
	float f_v_point_size = 5.0f;
	float v_n_line_width = 1.0f;
	float f_n_line_width = 1.0f;

	float v_n_line_length = 0.4f;
	float f_n_line_length = 0.4f;*/
};