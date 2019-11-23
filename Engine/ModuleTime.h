#ifndef _MODULE_TIME_H__
#define _MODULE_TIME_H__

#include "Globals.h"
#include "Module.h"
#include "Timer.h"

#define MAX_REAL_FRAME_MS  0.04F
#define DEFAULT_SCALE 1.0f

enum class GameState
{
	RUN,
	PAUSE,
	STOP,
	STEP_FORWARD
};

class ModuleScene;
class ModuleEditor;
class Application;
class W_Config;

class ModuleTime : public Module
{
public:

	friend W_Config;
	friend ModuleEditor;
	friend ModuleScene;
	friend Application;

public:

	ModuleTime();

	bool Init(Config& config);

	bool Start(Config& config);

	update_status PreUpdate();

	float DeltaTime();

	float UnscaledDeltaTime();

	void SetGameTimeScale( float scale);

	bool Save(Config& config);

	void Load(Config& config);

private:

	// Real functions -------------------------

	void SetCap ( uint max_frames);

	uint GetCap();

	// Game functions -------------------------

	GameState GetGameState();

	uint RealTimeSinceStartup();

	uint TimeSinceGameLoad();

	void StartGame();

	void PauseGame();

	void ResumeGame();

	void StopGame();

	void StepFowrardGame();

private:

	// General ---------------------------
	GameState game_state = GameState::STOP;	

	Timer	  ms_timer;
	Timer	  fps_timer;

	// Game Time -------------------------
	float	time_scale = DEFAULT_SCALE;	// Game speed multiplicator
	float	time_since_load = 0.0f;						// Time since game start (seconds)
	float	delta_time = 0.0f;					// Last frame time (seconds)

	// Real Time -------------------------
	float	max_ms = 16.f;
	float	max_fps = 60.f;
	uint	last_frame_ms = 0.f;

	float	real_time_since_startup = 0.0f;		// Time since application init 
	float	real_time_delta_time = 0.0f;		// Application real delta time
	uint	frame_count = 0u;					// Internal frame counter
	uint    fps = 0u;							// Last frmaes per second
};

#endif // !_MODULE_TIME_H__

