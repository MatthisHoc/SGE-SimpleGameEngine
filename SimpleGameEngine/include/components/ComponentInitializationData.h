#pragma once

#include "core/Parser.h"
#include "assistants/Resources.h"

#include <string>

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
		ComponentTypes(const std::string& str, ComponentTypes def = ComponentTypes::BoxComponent);

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
		vec2 size = { 0, 0 };
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
		ComponentInitializationData(const Block& data);

		// Copy member by member
		ComponentInitializationData(const ComponentInitializationData&) = default;
		ComponentInitializationData& operator=(const ComponentInitializationData&) = default;
	};
}