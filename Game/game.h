#pragma once

#ifdef WIN32
	#ifdef GAME_EXPORTS
		#define GAME_API __declspec(dllexport)
	#else
		#define GAME_API __declspec(dllimport)
	#endif
#elif defined(__GNUC__)
	#define GAME_API __attribute__((visibility("default")))
#else
	#define GAME_API
	#pragma warning Unkown dynamic link import/export semantic
#endif

extern "C++" GAME_API int run();