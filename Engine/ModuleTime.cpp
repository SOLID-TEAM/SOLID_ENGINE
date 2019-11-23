#include "ModuleTime.h"
#include "Config.h"

ModuleTime::ModuleTime()
{
	name.assign("ModuleTime");
}


bool ModuleTime::Init(Config& config)
{
	ms_timer.Start();
	fps_timer.Start();
	delta_time = 0.f;
	real_time_delta_time = 0.f;
	max_ms = 1000.f / 60.f;
	return true;
}

bool ModuleTime::Start(Config& config)
{
	Load(config);
	return true;
}

update_status ModuleTime::PreUpdate()
{
	++frame_count;

	// Update fps ------------------------

	if (fps_timer.ReadSeconds() >= 1.0f)
	{
		fps_timer.Start();
		fps = frame_count;
		frame_count = 0u;
	}

	// Update Real Time -------------------

	if (max_ms > 0.f && (ms_timer.Read() < max_ms))
		SDL_Delay((Uint32)max_ms - ms_timer.Read());

	last_frame_ms = ms_timer.Read();
	real_time_delta_time = ms_timer.ReadSeconds();
	real_time_since_startup += real_time_delta_time;

	if (real_time_delta_time )

	ms_timer.Start();

	// Update Game Time ------------------

	if (game_state == GameState::RUN)
	{
		delta_time = real_time_delta_time * time_scale;
		time_since_load += delta_time;
	}
	else if (game_state == GameState::STEP_FORWARD)
	{
		delta_time = real_time_delta_time * time_scale;
		time_since_load += delta_time;
		game_state = GameState::PAUSE;
	}
	else
	{
		delta_time = 0.f;
	}

	return UPDATE_CONTINUE;
}

void ModuleTime::SetCap(uint max_frames)
{
	max_fps = (float) max_frames;

	if (max_fps > 0.f) max_ms = 1000.f / max_fps;
	else max_ms = 0.f;
}

uint ModuleTime::GetCap()
{
	return (uint)max_fps;
}

uint ModuleTime::RealTimeSinceStartup()
{
	return real_time_since_startup;
}

uint ModuleTime::TimeSinceGameLoad()
{
	return time_since_load;
}

void ModuleTime::StartGame()
{
	if (game_state != GameState::RUN)
	{
		game_state = GameState::RUN;
		time_since_load = 0.f;

		// J_TODO: Save scene ------------------------------
	}
}

void ModuleTime::PauseGame()
{
	if (game_state != GameState::STOP)
	{
		game_state = GameState::PAUSE;
	}
}

void ModuleTime::ResumeGame()
{
	if (game_state == GameState::PAUSE)
	{
		game_state = GameState::RUN;
	}
}

void ModuleTime::StopGame()
{
	if (game_state != GameState::STOP)
	{
		game_state = GameState::STOP;
		time_since_load = 0.f;

		// J_TODO: Load scene ------------------------------
	}

}

void ModuleTime::StepFowrardTime()
{
	if (game_state != GameState::STOP)
	{
		game_state == GameState::STEP_FORWARD;
	}
}

float ModuleTime::DeltaTime()
{
	return delta_time;
}

float ModuleTime::UnscaledDeltaTime()
{
	return real_time_delta_time;
}

void ModuleTime::SetGameTimeScale(float scale)
{
	if (scale > 0.f)
	{
		time_scale = scale;
	}
}

bool ModuleTime::Save(Config& config)
{
	config.AddInt("framerate_cap", max_fps);

	return true;
}

void ModuleTime::Load(Config& config)
{
	SetCap(config.GetInt("framerate_cap", 60));
}


