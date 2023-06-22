#pragma once
#include <stdexcept>

#include "core/vec2.h"
#include "core/Object/Object.h"

namespace sg
{
	class ComponentException : public std::exception
	{
	public:
		ComponentException(const char* message);
		virtual const char* what() const noexcept override;
	private:
		std::string m_message;
	};

	class Component
	{
	public:
		Component(Object* obj = nullptr) : m_object(obj) {}
		virtual ~Component() = 0;

		Object& getObject() const { return *m_object; }

		template <typename T, typename ... TArgs>
		T& addComponent(TArgs&&... args) 
		{ 
			//return getObject().addComponent<T>(args); <-- This generates error with G++ because unexpented parameter pack
			return getObject().addComponent<T>(std::forward<TArgs>(args)...);

		}

		template <class T>
		T& addScript(bool canUpdate = true)
		{
			static_assert(std::is_base_of<class ScriptComponent, T>::value, "T must be a ScriptComponent");
			return getObject().addScript<T>(canUpdate);
		}

		friend class ObjectData;
		Object* m_object;
	};
}

