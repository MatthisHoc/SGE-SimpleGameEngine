#include "components/ScriptComponent.h"

namespace sg
{
	ScriptException::ScriptException(const char* message)
	{
		m_message += "[Script Error] ";
		m_message += message;
	}

	const char* ScriptException::what() const noexcept
	{
		return m_message.c_str();
	}
}