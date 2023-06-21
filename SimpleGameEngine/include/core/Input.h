#pragma once
#include <vector>
#include <SDL_events.h>
#include "Window.h"

namespace sg
{

	class Input
	{
	public:
		static bool getKeyDown(SDL_Keycode key);
		static bool getKeyUp(SDL_Keycode key);
		static bool isKeyPressed(SDL_Keycode key);

		static bool getMouseButtonDown(Uint8 buttonIndex);
		static bool getMouseButtonUp(Uint8 buttonIndex);
		static bool isMouseButtonPressed(Uint8 buttonIndex);

		static vec2 getMouseScreenPosition();
		static vec2 getMouseWorldPosition();
		static void setMouseRelative(bool mouseRelative);

	private:

		struct InputState
		{
			SDL_Keycode keycode;
			int numFramesPressed = 0;
			int frameReleased = 0;
		};

		struct MouseButtonState
		{
			Uint8 button;
			int numFramesPressed = 0;
			int frameReleased = 0;
		};

		Input() = delete;
		Input(const Input&) = delete;
		Input& operator=(const Input&) = delete;
		Input(Input&&) = delete;

		friend class Window;

		static std::vector<InputState> keys;
		static std::vector<MouseButtonState> mouseButtons;
		static vec2 mousePosition;
		
		static void removeKey(SDL_Keycode key);
		static void addKey(SDL_Keycode key);

		static void removeButton(Uint8 buttonIndex);
		static void addButton(Uint8 buttonIndex);
		static void updateMouse(const SDL_MouseMotionEvent& motion);
		
		static void update();
		static std::vector<InputState>::iterator findByKeycode(SDL_Keycode key);
		static std::vector<MouseButtonState>::iterator findByButtonIndex(Uint8 buttonIndex);
		static bool isIteratorValid(const std::vector<InputState>::iterator& iterator);
		static bool isMouseButtonIteratorValid(const std::vector<MouseButtonState>::iterator& iterator);
		static bool keyCodeToInputState(SDL_Keycode key, InputState& oInputState);
		static bool buttonIndexToMouseButtonState(Uint8 buttonIndex, MouseButtonState& oMouseButtonState);
	};
}

