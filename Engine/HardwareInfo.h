#ifndef _HARDWARE_H_
#define _HARDWARE_H_

class HardwareInfo
{
public:
	HardwareInfo();
	~HardwareInfo();

	void UpdateStaticVars();
	void UpdateDynamicVars();

public:

	struct System
	{
		int cpu_count = 0;
		int cpu_cache = 0;
		int sys_ram = 0;
		std::string caps;

	} system;

	struct GPU
	{
		std::string device;
		std::string vendor;
		float vram_dedicated = 0.f;
		float vram_available = 0.f;
		float vram_current = 0.f;
		float vram_evicted = 0.f;
	} gpu;

	struct RAM_Usage
	{
		int total_reported_mem = 0;
		int total_actual_mem = 0;
		int peak_reported_mem = 0;
		int peak_actual_mem = 0;
		int accumulated_reported_mem = 0;
		int accumulated_actual_mem = 0;
		int accumulated_alloc_unit = 0;
		int total_alloc_unity_count = 0;
		int peak_alloc_unit_count = 0;
	} ram_usage;

};

#endif // !_HARDWARE_H_

