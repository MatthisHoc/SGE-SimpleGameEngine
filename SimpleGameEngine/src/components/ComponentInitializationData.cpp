#include "components/ComponentInitializationData.h"
#include "core/Parser.h"

namespace sg
{
    ComponentTypes::ComponentTypes(const std::string& str, ComponentTypes def)
    {
        if (str == "AnimatedTextureComponent") value = AnimatedTextureComponent;
        else if (str == "BoxComponent") value = BoxComponent;
        else if (str == "TextComponent") value = TextComponent;
        else if (str == "TextureComponent") value = TextureComponent;
        else if (str == "TilesetComponent") value = TilesetComponent;
        else value = def.value;
    }

    ComponentInitializationData::ComponentInitializationData(const Block& data)
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
}