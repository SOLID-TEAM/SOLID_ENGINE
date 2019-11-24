#pragma once

// Warning disabled ---
#pragma warning( disable : 4577 ) // Warning that exceptions are disabled
#pragma warning( disable : 4530 ) // Warning that exceptions are disabled

#include <windows.h>
#include <stdio.h>

#define LOG(format, ...) log(__FILE__, __LINE__, format, __VA_ARGS__);

void log(const char file[], int line, const char* format, ...);

#define CAP(n) ((n <= 0.0f) ? n=0.0f : (n >= 1.0f) ? n=1.0f : n=n)

#define DEGTORAD 0.0174532925199432957f
#define RADTODEG 57.295779513082320876f
#define HAVE_M_PI


typedef unsigned int uint;
typedef unsigned long ulong;
typedef unsigned long long UID;

enum update_status
{
	UPDATE_CONTINUE = 1,
	UPDATE_STOP,
	UPDATE_ERROR
};

// Configuration -----------
#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
#define SCREEN_SIZE 1.f
#define WIN_FULLSCREEN false
#define WIN_RESIZABLE true
#define WIN_BORDERLESS false
#define WIN_FULLSCREEN_DESKTOP false
#define VSYNC true
#define TITLE "Solid Engine"

#define ASSETS_FOLDER "Assets/"
#define ASSETS_MODEL_FOLDER "Assets/Models/"
#define ASSETS_TEXTURES_FOLDER "Assets/Textures/"
#define LIBRARY_FOLDER "Library/"
#define LIBRARY_EDITOR_FOLDER "Library/Editor/"
#define LIBRARY_MODEL_FOLDER "Library/Models/"
#define LIBRARY_MESH_FOLDER "Library/Meshes/" // mesh own file format data
#define LIBRARY_MATERIAL_FOLDER "Library/Materials/" // textures in dds
#define LIBRARY_SETTINGS_FOLDER "Library/Settings/"
#define LIBRARY_TEXTURES_FOLDER "Library/Textures/"


// Deletes a buffer
#define RELEASE( x )\
    {\
       if( x != nullptr )\
       {\
         delete x;\
	     x = nullptr;\
       }\
    }

// Deletes an array of buffers
#define RELEASE_ARRAY( x )\
	{\
       if( x != nullptr )\
       {\
           delete[] x;\
	       x = nullptr;\
		 }\
	 }
//#ifndef _HAS_EXCEPTIONS
//#define _HAS_EXCEPTIONS 0
//#endif
//#define _STATIC_CPPLIB