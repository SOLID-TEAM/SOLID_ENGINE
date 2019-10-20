#ifndef _MODULE_EDITOR_H__
#define _MODULE_EDITOR_H__

#include "Globals.h"
#include "Module.h"

#include "external/Parson/parson.h"
#include <string>
#include <vector>
#include "GameObject.h"

class Window;
class W_Config;
class W_Console;
class W_Hierarchy;
class W_Rendering;
class W_Scene;
class W_Inspector;

// TODO: change this/remove define
#define MAX_STORED_FPS 100

struct ViewportOptions
{
	bool wireframe = false;
	bool outline = false;
	bool fill_faces = true;

	bool debug_vertex_normals = true;
	bool debug_face_normals = true;
	ImVec4 fill_color = ImVec4(0.f, 0.f, 0.f, 0.f);
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
	float f_n_line_length = 0.4f;
};

class DebugDataMesh
{
public:
	//DebugDataMesh();
	DebugDataMesh(uint n_vertices, uint n_indices);
	~DebugDataMesh();

	// needs to be called when all normals are computed
	void GenBuffers();
	void FillVertexBuffer();
	void FillFacesBuffer();

	bool Clean();

	void ComputeVertexNormals(D_Mesh* goMesh, float length);
	bool ComputeFacesNormals(D_Mesh* goMesh, float length);

	bool DebugRenderVertex(float pointSize);
	bool DebugRenderVertexNormals(float lineWidth);
	bool DebugRenderFacesVertex(float pointSize);
	bool DebugRenderFacesNormals(float lineWidth);

	void SetSizes(uint n_vertices, uint n_indices);

private:
	// meshes debug visualization (vertex and face normals)
	uint n_vertices = 0;
	uint n_idx = 0;
	// debug draw normals data ------
	float* debug_v_normals = nullptr;
	// maybe we dont need this for nothing more than debug ----
	//float* debug_f_vertices = nullptr; // this array should store 1 computed vertex(for draw the point on midface) for each face
	float* debug_f_normals = nullptr; // and 3 startpoint and 3 endpoint of each face
	// DEBUG DRAW PURPOSES BUFFERS ids -------------------------------
	uint debug_v_normals_gl_id = 0;
	// for debug draw faces points and line normals
	// TODO: implement this with stride in one float array (debug_f_vertices and debug_f_normals float arrays)
	//uint debug_f_vertices_gl_id = 0;
	uint debug_f_normals_gl_id = 0;

public:
	// store last line lengths for vertex and faces normals
	// evades when a user check one of the two modes, and change line length
	// for the deactivated one
	// Since we calc both when one option are switched to on, we need to perform
	// a second check if the ddmesh is still active for same go
	// and the precalculated length doesn't match with the current one
	float f_last_ll = 0.0f;
	float v_last_ll = 0.0f;
	
};

class ModuleEditor : public Module
{
public:
	ModuleEditor(bool start_enabled = true);

	~ModuleEditor();

	bool Init(Config& config);

	bool Start(Config& config);

	bool CleanUp();

	update_status PreUpdate(float dt);

	update_status Update(float dt);

	update_status PostUpdate(float dt);

	update_status Draw(); // for viewport draw loop

	bool Save(Config& config);

	void Load(Config& config);

	/*bool SaveEditorConfig(const char* path);

	bool LoadEditorConfig(const char* path);*/

	// temporaly utils for imgui prefab functions
	void HelpMarker(const char* desc) const;
	void ShowSoftwareInfo() const;

	//bool SliderIntWithSteps(const char* label, int* v, int v_min, int v_max, int v_step, const char* display_format);

	void AddWindow(Window* panel_to_add);

	void DestroyWindow(Window* panel_to_add);

	// TODO: move this utility out here
	float map(float value, float s1, float stop1, float s2, float stop2) const;

private:

	bool DrawMainMenuBar();

	void DrawPopUps();

private:

	std::vector<Window*> windows;

	// General editor info ---------

	std::string editor_filename;

	bool show_restore_popup = false;
	bool show_demo_imgui = false;
	bool show_about_popup = false;
	bool show_console = true;
	bool show_configuration = false;

public:

	// Windows -----------------------
	W_Config*		w_config = nullptr;
	W_Console*		w_console = nullptr;
	W_Hierarchy*	w_hierarchy = nullptr;
	W_Rendering*	w_rendering = nullptr;
	W_Scene*		w_scene = nullptr;
	W_Inspector*	w_inspector = nullptr;

	// WARNING: TODO: when we delete gameobjects, remember to unlink all pointers
	// hierarchy / selected go
	GameObject* selected_go = nullptr;
	ViewportOptions viewport_options;
	// if we need store more debug data, pass this to a vector/list
	DebugDataMesh* ddmesh = nullptr; // debug data data
	// if the gameobject itself has more than one data (not its childs)
	std::vector<DebugDataMesh*> ddmeshes;
	// store last precalculated go from debug normals
	GameObject* last_go_precalc = nullptr;
};

#endif // !_MODULE_EDITOR_H__