#ifndef __PANEL_CONFIG_H__
#define __PANEL_CONFIG_H__

#include "Globals.h"
#include <string>
#include <vector>
#include "SDL/include/SDL_scancode.h"
#include "Panel.h"

struct Hardware_Info
{
	int cpu_count = 0;
	int cpu_cache = 0;
	int sys_ram = 0;
	std::string caps;
	std::string gpu_device;
	std::string gpu_vendor;
	float vram_dedicated = 0.f;
	float vram_available = 0.f;
	float vram_current = 0.f;
	float vram_evicted = 0.f;
};

class PanelConfig : public Panel
{
public:

	PanelConfig(std::string name);

	virtual ~PanelConfig();

	void Render();

	void AddFpsLog(const float fps, const float ms);

	void AddInputLog(uint key, KEY_STATE state, int flag);

private:
	
	void GetHardWareInfo(Hardware_Info* info);

private:

	ImGuiTextBuffer input_buffer;
	std::vector<float> stored_fps;
	std::vector<float> stored_ms;
};

#endif // __PANEL_CONFIG_H__