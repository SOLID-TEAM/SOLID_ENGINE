#include "PanelConsole.h"
#include "ModuleEditor.h"
#include "Application.h"

PanelConsole::PanelConsole(std::string name): Panel(name)
{
	console_log = App->console_log_aux;
	App->console_log_aux.clear();
}

PanelConsole::~PanelConsole()
{
}

void PanelConsole::Render()
{
	ImGui::SetNextWindowSize(ImVec2(380, 600), ImGuiCond_Once);
	static bool auto_scroll = true;
	if (ImGui::Begin("Console", &active))
	{

		ImGui::Checkbox("Auto scroll", &auto_scroll);
		ImGui::SameLine();
		if (ImGui::Button("Clear console"))
		{
			for (int i = 0; i < console_log.Size; i++)
				delete(console_log[i]);
			console_log.clear();
		}

		ImGui::SameLine();
		if (ImGui::Button("Save History Log"))
		{
			//App->SaveLogToFile();
			LOG("[Info] Log File saved to hard disk");
		}
	
		ImGui::SameLine();  App->editor->HelpMarker("Save all time history, doesn't affect console clearing");

		ImGui::Separator();

		ImGui::BeginChild("ScrollingRegion");

		// TODO: if we need thousands of lines of log, implement clipping (see on imgui_demo.cpp commentary)
		for (int i = 0; i < console_log.size(); i++)
		{
			const char* item = console_log[i];

			// Normally you would store more information in your item (e.g. make Items[](console_log) an array of structure, store color/type etc.)
			bool pop_color = false;

			//if (strstr(item, "[Error]")) { ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.4f, 0.4f, 1.0f)); pop_color = true; }
			// strstr compare full string searching for match

			// change color of the line respect prefix tag
			// on [Error] | [Init] | [CleanUp] etc, only we need to compare the first 2 + \n from log 
			//(if we need we can delete for every line the jump) characters to perform a good match
			// update: deleted from editor log for now
			if (strncmp(item, "[Init]", 4) == 0) { ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor(100, 255, 97, 255)); pop_color = true; }
			else if (strncmp(item, "[Start]", 2) == 0) { ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor(94, 156, 255, 255)); pop_color = true; }
			else if (strncmp(item, "[Info]", 4) == 0) { ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor(255, 230, 70, 255)); pop_color = true; }
			else if (strncmp(item, "[Error]", 2) == 0) { ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.4f, 0.4f, 1.0f)); pop_color = true; }

			ImGui::TextUnformatted(item);
			if (pop_color)
				ImGui::PopStyleColor();
		}

		/*ImGui::TextUnformatted(console_buffer.begin());*/
		if (auto_scroll)
			ImGui::SetScrollHereY(1.0f);
		ImGui::EndChild();
	}
	ImGui::End();
}

void PanelConsole::AddConsoleLog(const char* new_entry)
{
	console_log.push_back(strdup(new_entry));
}
