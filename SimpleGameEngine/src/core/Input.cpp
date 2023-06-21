#include <vector>
#include <iostream>

#include "core/Input.h"

namespace sg
{
	std::vector<Input::InputState> Input::keys;
	std::vector<Input::MouseButtonState> Input::mouseButtons;
	vec2 Input::mousePosition;

	std::vector<Input::InputState>::iterator Input::findByKeycode(SDL_Keycode key)
	{
		auto it = keys.begin();
		while (it != keys.end())
		{
			if ((*it).keycode == key)
			{
				break;
			}
			++it;
		}
		
		return it;
	}

	std::vector<Input::MouseButtonState>::iterator Input::findByButtonIndex(Uint8 buttonIndex)
	{
		auto it = mouseButtons.begin();
		while (it != mouseButtons.end())
		{
			if ((*it).button == buttonIndex)
			{
				break;
			}
			++it;
		}

		return it;
	}

	bool Input::isIteratorValid(const std::vector<Input::InputState>::iterator& iterator)
	{
		return keys.size() > 0 && iterator != keys.end();
	}

	bool Input::isMouseButtonIteratorValid(const std::vector<Input::MouseButtonState>::iterator& iterator)
	{
		return mouseButtons.size() > 0 && iterator != mouseButtons.end();
	}

	bool Input::keyCodeToInputState(SDL_Keycode key, Input::InputState& oInputState)
	{
		auto it = findByKeycode(key);
		bool iteratorValid = isIteratorValid(it);

		if (iteratorValid)
		{
			oInputState = (*it);
			return true;
		}

		return false;
	}

	bool Input::buttonIndexToMouseButtonState(Uint8 buttonIndex, MouseButtonState& oMouseButtonState)
	{
		auto it = findByButtonIndex(buttonIndex);
		bool iteratorValid = isMouseButtonIteratorValid(it);

		if (iteratorValid)
		{
			oMouseButtonState = (*it);
			return true;
		}

		return false;
	}


	void Input::removeKey(SDL_Keycode key)
	{
		auto it = findByKeycode(key);
		if (it != keys.end())
		{
			auto& state = *it;
			state.frameReleased = state.numFramesPressed;
		}
	}

	void Input::addKey(SDL_Keycode key)
	{
		// Make sure the key isn't already in the list
		if (findByKeycode(key) == keys.end())
		{
			Input::InputState state;
			state.keycode = key;
			keys.push_back(state);
		}
	}

	bool Input::getKeyDown(SDL_Keycode key)
	{
		Input::InputState state;

		if (keyCodeToInputState(key, state))
		{
			return state.numFramesPressed == 0;
		}

		return false;
	}

	bool Input::getKeyUp(SDL_Keycode key)
	{
		Input::InputState state;

		if (keyCodeToInputState(key, state))
		{
			return state.frameReleased != 0 && state.frameReleased != state.numFramesPressed;
		}

		return false;
	}

	bool Input::isKeyPressed(SDL_Keycode key)
	{
		Input::InputState state;

		if (keyCodeToInputState(key, state))
		{
			return state.frameReleased == 0;
		}

		return false;
	}

	void Input::removeButton(Uint8 buttonIndex)
	{
		auto it = findByButtonIndex(buttonIndex);
		if (it != mouseButtons.end())
		{
			auto& state = *it;
			state.frameReleased = state.numFramesPressed;
		}
	}

	void Input::addButton(Uint8 buttonIndex)
	{
		// Make sure the button isn't already in the list
		if (findByKeycode(buttonIndex) == keys.end())
		{
			Input::MouseButtonState state;
			state.button = buttonIndex;
			mouseButtons.push_back(state);
		}
	}

	bool Input::getMouseButtonDown(Uint8 buttonIndex)
	{
		Input::MouseButtonState state;

		if (buttonIndexToMouseButtonState(buttonIndex, state))
		{
			return state.numFramesPressed == 0;
		}

		return false;
	}

	bool Input::getMouseButtonUp(Uint8 buttonIndex)
	{
		Input::MouseButtonState state;

		if (buttonIndexToMouseButtonState(buttonIndex, state))
		{
			return state.frameReleased != 0 && state.frameReleased != state.numFramesPressed;
		}

		return false;
	}

	bool Input::isMouseButtonPressed(Uint8 buttonIndex)
	{
		Input::MouseButtonState state;

		if (buttonIndexToMouseButtonState(buttonIndex, state))
		{
			return state.frameReleased == 0;
		}

		return false;
	}

	void Input::update()
	{
		// Update keys
		for (int i = static_cast<int>(keys.size()) - 1; i >= 0; --i)
		{
			Input::InputState& state = keys[i];

			// Remove the key one frame after it has been released
			if (state.frameReleased != 0 &&
				state.numFramesPressed > state.frameReleased)
			{
				keys.erase(keys.begin() + i);
			}

			++state.numFramesPressed;
		}

		// Update mouse buttons
		for (int i = static_cast<int>(mouseButtons.size()) - 1; i >= 0; --i)
		{
			Input::MouseButtonState& state = mouseButtons[i];

			// Remove the button one frame after it has been released
			if (state.frameReleased != 0 &&
				state.numFramesPressed > state.frameReleased)
			{
				mouseButtons.erase(mouseButtons.begin() + i);
			}

			++state.numFramesPressed;
		}
	}

	vec2 Input::getMouseScreenPosition()
	{
		return mousePosition;
	}

	vec2 Input::getMouseWorldPosition()
	{
		if (SDL_GetRelativeMouseMode())
		{
			return getMouseScreenPosition();
		}
		else
		{
			return getMouseScreenPosition() + Window::getCameraTopLeft();
		}
	}

	void Input::setMouseRelative(bool mouseRelative)
	{
		if (mouseRelative)
			SDL_SetRelativeMouseMode(SDL_TRUE);
		else
			SDL_SetRelativeMouseMode(SDL_FALSE);
	}

	void Input::updateMouse(const SDL_MouseMotionEvent& motion)
	{
		if (SDL_GetRelativeMouseMode())
		{
			mousePosition.x = (float)motion.xrel;
			mousePosition.y = (float)motion.yrel;
		}
		else
		{
			mousePosition.x = (float)motion.x;
			mousePosition.y = (float)motion.y;
		}
	}
}
