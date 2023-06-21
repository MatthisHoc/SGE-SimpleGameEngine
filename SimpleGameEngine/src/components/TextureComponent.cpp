#include "components/TextureComponent.h"
#include "core/Window.h"

#include <SDL_render.h>
#include <SDL_image.h>
#include <SDL_video.h>
#include <string>

namespace sg
{
	TextureComponent::TextureComponent(Object* obj, const ComponentInitializationData& data) :
		DrawableComponent(obj, data)
	{
		initializeTexture(Resources::pathTo(data.path));
	}
}
