#ifndef __PANEL_PRIMITIVES_H__
#define __PANEL_PRIMITIVES_H__

#include "Globals.h"
#include "Window.h"

#include <vector>

class W_Primitives : public Window
{
public:

	W_Primitives(std::string name, bool active);

	virtual ~W_Primitives();

	void Draw();

	float2 GetSlicesAndStacks(int i);

private:

	std::vector<uint> tex_ids; // image miniatures for each primitive

	GameObject* last_created = nullptr;

};

#endif // __PANEL_PRIMITIVES_H__