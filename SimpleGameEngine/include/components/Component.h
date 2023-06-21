#pragma once
#include <stdexcept>

#include "assistants/Resources.h"

#include "core/Parser.h"
#include "core/vec2.h"

namespace sg
{
	class ComponentTypes
	{
	public:
		enum Value : uint8_t 
		{ 
			AnimatedTextureComponent,
			BoxComponent,
			TextComponent,
			TextureComponent,
			TilesetComponent,
		};

		constexpr ComponentTypes(Value type = BoxComponent) : value(type) {}
		ComponentTypes(const std::string& str, ComponentTypes def = ComponentTypes::BoxComponent)
		{
			if (str == "AnimatedTextureComponent") value = AnimatedTextureComponent;
			else if (str == "BoxComponent") value = BoxComponent;
			else if (str == "TextComponent") value = TextComponent;
			else if (str == "TextureComponent") value = TextureComponent;
			else if (str == "TilesetComponent") value = TilesetComponent;
			else value = def.value;
		}

		operator Value() const { return value; }
		explicit operator bool() const = delete;

		constexpr bool operator==(ComponentTypes p) const { return p.value == value; }
		constexpr bool operator!=(ComponentTypes p) const { return p.value != value; }

	private:
		Value value;
	};

	struct ComponentInitializationData
	{
		ComponentTypes type = ComponentTypes::BoxComponent;
		std::string path = "";
		vec2 size = { 0 };
		int ptSize = 0;
		std::string text = "";
		int zIndex = 0;
		bool drawDebug = false;
		bool centerOrigin = false;
		int margin = 0;
		int spacing = 0;

		ComponentInitializationData() {}
		~ComponentInitializationData() {}

		// Constructor from parser block
		ComponentInitializationData(const Block& data)
		{
			type = ComponentTypes(data.name);
			path = data.get("path");
			size = data.getVec2("size");
			ptSize = data.getInt("ptSize");
			text = data.get("text");
			zIndex = data.getInt("z-index");
			drawDebug = data.has("draw-debug");
			centerOrigin = data.has("center-origin");
			margin = data.getInt("margin");
			spacing = data.getInt("spacing");
		}

		// Copy member by member
		ComponentInitializationData(const ComponentInitializationData&) = default;
		ComponentInitializationData& operator=(const ComponentInitializationData&) = default;
	};

	class ComponentException : public std::exception
	{
	public:
		ComponentException(const char* message);
		virtual const char* what() const override;
	private:
		std::string m_message;
	};

	class Object;
	struct ComponentInfo;

	class Component
	{
	public:
		Component(Object* obj = nullptr) : m_object(obj) {}
		virtual ~Component() = 0 {}

		Object& getObject() const { return *m_object; }

		template <typename T, typename ... TArgs>
		T& addComponent(TArgs&&... args) { return getObject().addComponent<T>(args); }

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

