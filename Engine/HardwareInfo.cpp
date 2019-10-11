#include <string>

#include "external/mmgr/mmgr.h"

#include "Globals.h"
#include "HardwareInfo.h"
#include "SDL/include/SDL_cpuinfo.h"
#include "GL/glew.h"

HardwareInfo::HardwareInfo()
{
	bool UpdateStaticVars();
	bool UpdateDynamicVars();
}

HardwareInfo::~HardwareInfo()
{
}

void HardwareInfo::UpdateStaticVars()
{

}

void HardwareInfo::UpdateDynamicVars()
{
	// CPU ------------------------------------
	system.cpu_count = SDL_GetCPUCount();
	system.cpu_cache = SDL_GetCPUCacheLineSize();

	// RAM ------------------------------------
	int sys_ram = SDL_GetSystemRAM();
	// CAPS -----------------------------------
	system.caps.clear();

	if (SDL_Has3DNow())
		system.caps.append("3DNow, ");
	if (SDL_HasAVX())
		system.caps.append("AVX, ");
	if (SDL_HasAVX2())
		system.caps.append("AVX2, ");
	if (SDL_HasMMX())
		system.caps.append("MMX, ");
	if (SDL_HasRDTSC())
		system.caps.append("RDTSC, ");
	if (SDL_HasSSE())
		system.caps.append("SSE, ");
	if (SDL_HasSSE2())
		system.caps.append("SSE2, ");
	if (SDL_HasSSE3())
		system.caps.append("SSE3, ");
	if (SDL_HasSSE41())
		system.caps.append("SSE41, ");
	if (SDL_HasSSE42())
		system.caps.append("SSE42, ");
	if (SDL_HasAltiVec)
		system.caps.append("AltiVec, ");

	// GPU ----------------------------------
	gpu.device.assign((const char*)glGetString(GL_RENDERER));
	gpu.vendor.assign((const char*)glGetString(GL_VENDOR));

	glGetFloatv(GL_GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX, &gpu.vram_dedicated);
	gpu.vram_dedicated /= 1024.f;
	glGetFloatv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &gpu.vram_available);
	gpu.vram_available /= 1024.f;
	glGetFloatv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &gpu.vram_current);
	gpu.vram_current /= 1024.f;
	glGetFloatv(GL_GPU_MEMORY_INFO_EVICTED_MEMORY_NVX, &gpu.vram_evicted);
	gpu.vram_evicted /= 1024.f;

	sMStats stats = m_getMemoryStatistics();

	ram_usage.total_reported_mem		= stats.totalReportedMemory;
	ram_usage.total_actual_mem			= stats.totalActualMemory;
	ram_usage.peak_reported_mem			= stats.peakReportedMemory;
	ram_usage.peak_actual_mem			= stats.peakActualMemory;
	ram_usage.accumulated_reported_mem	= stats.accumulatedReportedMemory;
	ram_usage.accumulated_actual_mem	= stats.accumulatedActualMemory;
	ram_usage.accumulated_alloc_unit	= stats.accumulatedAllocUnitCount;
	ram_usage.total_alloc_unity_count	= stats.totalAllocUnitCount;
	ram_usage.peak_alloc_unit_count		= stats.peakAllocUnitCount;
}

