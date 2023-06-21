#include "components/DrawableComponent.h"

#include "core/Window.h"

#include <SDL_render.h>
#include <SDL_image.h>
#include <utility>
#include <map>
#include <string>

namespace sg
{
	DrawableComponent::DrawableComponent(Object* obj, const ComponentInitializationData& data)
		: Component(obj), zIndex(data.zIndex), m_centerOrigin(data.centerOrigin)
	{

	}

	DrawableComponent::DrawableComponent(Object* obj, int width, int height)
		: Component(obj)
	{
		m_texture = std::make_unique<Texture>(width, height);
	}

	SDL_Rect DrawableComponent::getBounds() const
	{
		int w = 0, h = 0;

		if (auto* texture = m_texture->get())
		{
			SDL_QueryTexture(texture, nullptr, nullptr, &w, &h);
		}

		vec2 pos = getObject().getPosition();
		SDL_Rect bounds;
		bounds.x = ((int)pos.x - (w / 2));
		bounds.y = ((int)pos.y - (h / 2));
		bounds.w = w;
		bounds.h = h;

		return bounds;
	}
}