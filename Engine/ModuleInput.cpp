#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleEditor.h"
#include "W_Config.h"
#include "W_Rendering.h"
#include "Event.h"

#include "ImGui/Impl/imgui_impl_sdl.h"
#include "ImGui/imgui_internal.h"

// TODO: DELETE FROM HERE
#include "C_Material.h"
#include "Component.h"
#include "R_Material.h"

// TODO: maybe temporal untill we get to fully work module filesystem
// the most recommended approach to deal with paths/extensions/filenames in windows
// before boost lib
//#include <filesystem> // for search filename extension / file name | needed c++17

#define MAX_KEYS 300

ModuleInput::ModuleInput(bool start_enabled) : Module(start_enabled)
{
	name.assign("Input");

	keyboard = new KEY_STATE[MAX_KEYS];
	memset(keyboard, KEY_IDLE, sizeof(KEY_STATE) * MAX_KEYS);
	memset(mouse_buttons, KEY_IDLE, sizeof(KEY_STATE) * MAX_MOUSE_BUTTONS);
}

// Destructor
ModuleInput::~ModuleInput()
{
	delete[] keyboard;
}

// Called before render is available
bool ModuleInput::Init(Config& config)
{
	LOG("[Init] SDL input event system");
	bool ret = true;
	SDL_Init(0);

	if(SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
	{
		LOG("SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	// SDL Enables -----------------------------
	SDL_EventState(SDL_DROPFILE, SDL_ENABLE);
	
	return ret;
}

bool ModuleInput::CleanUp()
{
	LOG("Quitting SDL input event subsystem");
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	return true;
}

// Called every draw update
update_status ModuleInput::PreUpdate(float dt)
{
	SDL_PumpEvents();

	const Uint8* keys = SDL_GetKeyboardState(NULL);
	
	for (int i = 0; i < MAX_KEYS; ++i)
	{
		if (keys[i] == 1)
		{
			// TODO: maybe we dont need to update every key repeat the input log
			if (keyboard[i] == KEY_IDLE)
			{
				keyboard[i] = KEY_DOWN;
				App->editor->w_config->AddInputLog(i, KEY_DOWN, 0);
			}
			else
			{
				keyboard[i] = KEY_REPEAT;
			}
		}
		else
		{
			if (keyboard[i] == KEY_REPEAT || keyboard[i] == KEY_DOWN)
			{
				keyboard[i] = KEY_UP;
				App->editor->w_config->AddInputLog(i, KEY_UP, 0);
			}
			else
			{
				keyboard[i] = KEY_IDLE;
			}
		}
	}

	Uint32 buttons = SDL_GetMouseState(&mouse_x, &mouse_y);

	mouse_x /= SCREEN_SIZE;
	mouse_y /= SCREEN_SIZE;
	mouse_z = 0;

	for(int i = 0; i < MAX_MOUSE_BUTTONS; ++i)
	{
		if(buttons & SDL_BUTTON(i))
		{
			if (mouse_buttons[i] == KEY_IDLE)
			{
				mouse_buttons[i] = KEY_DOWN;
				App->editor->w_config->AddInputLog(i, KEY_DOWN, 1 );
			}
			else
			{
				mouse_buttons[i] = KEY_REPEAT;
			}
		}
		else
		{
			if (mouse_buttons[i] == KEY_REPEAT || mouse_buttons[i] == KEY_DOWN)
			{
				mouse_buttons[i] = KEY_UP;
				App->editor->w_config->AddInputLog(i, KEY_UP, 1);
			}
			else
			{
				mouse_buttons[i] = KEY_IDLE;
			}
		}
	}

	mouse_x_motion = mouse_y_motion = 0;

	bool quit = false;
	SDL_Event event;

	//char* dropped_filedir = nullptr;

	//SDL_DisplayMode DM;
	//SDL_GetCurrentDisplayMode(0, &DM);
	//auto width = DM.w;
	//auto height = DM.h;
	//int offset = 12;

	while (SDL_PollEvent(&event))
	{
		ImGui_ImplSDL2_ProcessEvent(&event);

		switch (event.type)
		{
		case SDL_MOUSEWHEEL:
			mouse_z = event.wheel.y;
			break;

		case SDL_MOUSEMOTION:

			mouse_x = event.motion.x / SCREEN_SIZE;
			mouse_y = event.motion.y / SCREEN_SIZE;

			mouse_x_motion = (event.motion.xrel / SCREEN_SIZE) ;
			mouse_y_motion = (event.motion.yrel / SCREEN_SIZE);

			break;

		case SDL_DROPFILE:
		{
			Event e(Event::file_dropped);
			e.string.ptr = event.drop.file;
			App->BroadcastEvent(e);
			SDL_free(event.drop.file);
			break;
		}
		case SDL_QUIT:
			quit = true;
			break;

		case SDL_WINDOWEVENT:

			if (event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED)
			{
				App->window->window_focused = true;
			}

			else if (event.window.event == SDL_WINDOWEVENT_TAKE_FOCUS)
			{
				App->window->window_focused = true;
			}

			else if (event.window.event == SDL_WINDOWEVENT_FOCUS_LOST)
			{
				App->window->window_focused = false;
			}

			if (event.window.event == SDL_WINDOWEVENT_MAXIMIZED)
				App->window->maximized = true;

			if (event.window.event == SDL_WINDOWEVENT_RESTORED)
				App->window->maximized = false;
		}
	}

	if(quit == true || keyboard[SDL_SCANCODE_ESCAPE] == KEY_UP)
		return UPDATE_STOP;

	return UPDATE_CONTINUE;
}

bool ModuleInput::Save(Config& config)
{
	return true;
}

void ModuleInput::Load(Config& config)
{

}


//if (mouse_x > width - offset)
//{
//	int last_x = mouse_x;
//	App->input->SetMouseX(offset);
//	mouse_offset_x = mouse_x - last_x;
//	ImGui::GetIO().MousePos.x = mouse_x;
//}
//else if (mouse_x < offset)
//{
//	int last_x = mouse_x;
//	App->input->SetMouseX(width - offset);
//	mouse_offset_x = mouse_x - last_x;
//	ImGui::GetIO().MousePos.x = mouse_x;
//}

//if (mouse_y > height - offset)
//{
//	int last_y = mouse_y;
//	App->input->SetMouseY(offset);
//	mouse_offset_y = mouse_y - last_y;
//	ImGui::GetIO().MousePos.y = mouse_y;
//}
//else if (mouse_y < offset)
//{
//	int last_y = mouse_y;
//	App->input->SetMouseY(height - offset);
//	mouse_offset_y = mouse_y - last_y;
//	ImGui::GetIO().MousePos.y = mouse_y;
//}

//ImGui::ResetMouseDragDelta(0);
//ImGui::ResetMouseDragDelta(1);
//ImGui::ResetMouseDragDelta(2);
//ImGui::GetCurrentContext()->ActiveIdIsJustActivated = true;

//LOG("mouse x: %i  mouse y: %i", mouse_x, mouse_y);