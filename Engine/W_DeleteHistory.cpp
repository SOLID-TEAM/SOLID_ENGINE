#include "Application.h"
#include "W_DeleteHistory.h"

W_DeleteHistory::W_DeleteHistory(std::string name, bool active) : Window(name, active)
{
	
}

W_DeleteHistory::~W_DeleteHistory() {}

void W_DeleteHistory::Draw()
{
	if (ImGui::Begin("Delete History", &active));
	{
		std::deque<GameObject*>::iterator it = App->scene->GetUndoDeque().begin();

		for (; it != App->scene->GetUndoDeque().end(); ++it)
		{
			ImGui::Selectable((*it)->GetName());
		}
	}

	ImGui::End();
}