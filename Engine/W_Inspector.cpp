#include "Application.h"
#include "W_Inspector.h"

W_Inspector::W_Inspector(std::string name, bool active) : Window(name, active)
{
}

void W_Inspector::Draw()
{
	if (ImGui::Begin("Inspector"))
	{
		if (ImGui::CollapsingHeader("BLABLA"))
		{
			
		}
	}

	ImGui::End();
}