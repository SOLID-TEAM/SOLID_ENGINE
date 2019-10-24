#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleEditor.h"
#include "W_Config.h"
#include "W_Rendering.h"

#include "ImGui/Impl/imgui_impl_sdl.h"

// TODO: maybe temporal untill we get to fully work module filesystem
// the most recommended approach to deal with paths/extensions/filenames in windows
// before boost lib
#include <filesystem> // for search filename extension / file name | needed c++17

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

	char* dropped_filedir = nullptr;

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

			mouse_x_motion = event.motion.xrel / SCREEN_SIZE;
			mouse_y_motion = event.motion.yrel / SCREEN_SIZE;
			break;

		case SDL_DROPFILE:
		{
			// TODO: remove this testing from here, and do specific functions/change with our filesystem ----------
			//dropped_filedir = event.drop.file;
			std::experimental::filesystem::path filepath = event.drop.file;
			std::experimental::filesystem::path filename = filepath.filename();
			std::experimental::filesystem::path extension = filepath.extension();

			LOG("%s", filepath.generic_string().data());
			LOG("%s", extension.generic_string().data());

			std::string comparer = extension.generic_string();

			std::transform(comparer.begin(), comparer.end(), comparer.begin(), [](unsigned char c) { return std::tolower(c); });

			if (comparer == ".fbx" ||
				comparer == ".obj" ||
				comparer == ".solid")
			{
				LOG("// ---------------------------------------------------");
				LOG("[Info] Possible 3D %s model", comparer.data());
				LOG("// ---------------------------------------------------");
				App->importer->ImportModelFile(filepath.generic_string().data());
			}
			if (comparer == ".png" ||
				comparer == ".jpg" ||
				comparer == ".dds" ||
				comparer == ".tif")
			{
			// TODO !!!! : testing to reload model texture with the dropped one
			// still need to reload only the desired "focused" model
			// still need to delete previous texture buffer
				LOG("// ---------------------------------------------------");
				LOG("[Info] Possible texture");
				uint new_tex_id = 0;
				// load texture already checks if the texture is previously loaded and return its id if it, new id if not
				new_tex_id = App->textures->LoadTexture(filepath.generic_string().data());

				// TODO- FIXED(go specific): in rare circunstances we can delete all gl texture buffers(not necessarily all,
				// only needs to delete one id previously associated with the checker tex). If we have
				// checker texture active with some linked id to image visualization(active checker tex), when we load a new texture
				// (and delete) with the panel checker texture not active, and returning to it, the image shown is not the checker
				// previously deleted, is the new one! bad
				// workaround | WE NEED to deal with this when we implement checker for active gameobject only
				// if the new id texture is the same as the one we have linked to checker texture, something weird occurs, then
				// what that means some id are deleted and another texture is loaded without returning to this panel
				// FIXED ---
				/*if (new_tex_id == App->editor->w_rendering->checker_tex_gl_id)
					App->editor->w_rendering->checker_tex_gl_id = 0;*/
				
				/*if (new_tex_id > 0)
					App->importer->ReloadTextureForAllModels(new_tex_id);*/
			}

			SDL_free(event.drop.file);

			break;
			// ----------------------------------------------------------------------------------------------------
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
		}
	}

	if(quit == true || keyboard[SDL_SCANCODE_ESCAPE] == KEY_UP)
		return UPDATE_STOP;

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleInput::CleanUp()
{
	LOG("Quitting SDL input event subsystem");
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	return true;
}

bool ModuleInput::Save(Config& config)
{
	return true;
}

void ModuleInput::Load(Config& config)
{

}


