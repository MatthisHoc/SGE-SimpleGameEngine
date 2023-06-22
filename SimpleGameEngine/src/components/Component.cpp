#include "components/Component.h"

#include <SDL.h>

namespace sg
{
	ComponentException::ComponentException(const char* message)
	{
		m_message += "[Component Error] ";
		m_message += message;
		m_message += ": ";
		m_message += SDL_GetError();
	}

	const char* ComponentException::what() const noexcept
	{
		return m_message.c_str();
	}

	Component::~Component() {}
}
