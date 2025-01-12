#ifndef __CUBE_H__
#define __CUBE_H__

#define _FILE_OFFSET_BITS 64

#ifdef WIN32
#define _USE_MATH_DEFINES
#endif
#include <cmath>

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <cstdarg>
#include <climits>
#include <cassert>
#include <ctime>

// include C++ standard libs here

#include <string>
#include <thread>
#include <mutex>
#include <regex>

#ifdef WIN32
	#define WIN32_LEAN_AND_MEAN
	#ifdef _WIN32_WINNT
	#undef _WIN32_WINNT
	#endif
	#define _WIN32_WINNT 0x0501 // windows XP
	#include "windows.h"
	#ifndef _WINDOWS
		#define _WINDOWS
	#endif
	#ifndef __GNUC__
		#include <eh.h>
		#include <dbghelp.h>
		#include <intrin.h>
	#endif
	#define ZLIB_DLL
#endif

#ifndef STANDALONE
	#ifdef __APPLE__
		#include "SDL.h"
		#define GL_GLEXT_LEGACY
		#define __glext_h_
		#include <OpenGL/gl.h>
	#else
		#include <SDL.h>
		#include <SDL_opengl.h>
	#endif
	#include <SDL_gamecontroller.h>
#endif

#ifdef DISCORD
	#include <discord/discord_game_sdk.h>
	#include <discord/discord.h>
#endif

#ifdef STEAM
	#include <steam/steam_api.h>
#endif

#include <b64/b64.h>
#include <enet/enet.h>
#include <curl/curl.h>
#include <cJSON.h>
#include <zlib.h>

#include "translate.h"

#include "tools.h"
#include "geom.h"
#include "ents.h"
#include "command.h"
#include "colors.h"

#ifndef STANDALONE
#include "glexts.h"
#include "glemu.h"
#endif

#include "iengine.h"
#include "igame.h"

// TODO: check why is this import needed, it seems like a hack
#include "game.h"

#endif

