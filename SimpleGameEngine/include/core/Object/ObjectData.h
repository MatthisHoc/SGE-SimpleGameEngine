#pragma once

#include <vector>
#include <string>
#include <stdexcept>
#include <stdarg.h>
#include <utility>
#include <type_traits>

#include "components/Component.h"
#include "components/ScriptComponent.h"
#include "core/vec2.h"
#include "core/Layers.h"

namespace sg
{
	class ObjectException : public std::exception
	{
	public:
		ObjectException(const char* message);
		virtual const char* what() const override;
	private:
		std::string m_message;
	};

	class ObjectData
	{
	public:
		~ObjectData();
		ObjectData() {}
		ObjectData(const ObjectData& other);
		void initializeFromFile(const class Block& objectData);
	private:
		ObjectData& operator=(const ObjectData&) = delete;

	public:
		vec2 position = { 0.f, 0.f };
		vec2 size = { 1.0f, 1.0f };
		bool visible = true;
		bool screenPosition = false;
		bool drawDebug = false;
		int layers = LAYER_WORLD;
	private:
		std::string m_name;

	public:

		const std::string& getName() const { return m_name; }
		void setName(const std::string& newName) { m_name = newName; }
	};
}
