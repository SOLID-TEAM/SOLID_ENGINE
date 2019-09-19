#include "Globals.h"
#include "Application.h"

#include <gl/GL.h>
#include "ModuleEditor.h"


ModuleEditor::ModuleEditor(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleEditor::~ModuleEditor()
{}

// Load assets
bool ModuleEditor::Start()
{
	LOG("Loading Editor");
	bool ret = true;

	// DEAR IMGUI SETUP ---------------------------------------------------------- 
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer bindings
	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer3D->context);
	ImGui_ImplOpenGL2_Init();
	// ----------------------------------------------------------------------------

	return ret;
}

// Load assets
bool ModuleEditor::CleanUp()
{
	LOG("Unloading Editor");

	ImGui_ImplSDL2_Shutdown();
	ImGui_ImplOpenGL2_Shutdown();
	ImGui::DestroyContext();
	return true;
}

// Update
update_status ModuleEditor::Update(float dt)
{
	// IMGUI UPDATE ----------------------------------------------------------------------
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	// Start the Dear ImGui frame
	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();

	if (show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);

	ImGui::Begin("Hello, world!");

	ImGui::Text("This is some useful text.");
	static int counter = 0;
	if (ImGui::Button("EXIT"))
		return UPDATE_STOP;
	ImGui::SameLine();
	if (ImGui::Button("TOGGLE DEMO WINDOW"))
		show_demo_window = !show_demo_window;

	/*	counter++;
	ImGui::SameLine();
	ImGui::Text("counter = %d", counter);*/

	ImGui::End();

	// ----------------------------------------------------------------------------------

	return UPDATE_CONTINUE;
}

update_status ModuleEditor::PostUpdate(float dt)
{
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::Render();
	glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
	//glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
	//glUseProgram(0); // You may want this if using this code in an OpenGL 3+ context where shaders may be bound
	ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

	return UPDATE_CONTINUE;
}
