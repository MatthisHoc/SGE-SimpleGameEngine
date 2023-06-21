#pragma once

#include <SDL_render.h>
#include <map>
#include <memory>

#include "core/vec2.h"
#include "core/Object/Object.h"
#include "core/Texture.h"

#include "Component.h"


namespace sg
{
	class DrawableComponent : public Component
	{
	public:
		DrawableComponent(Object* obj, const ComponentInitializationData& data);
		DrawableComponent(Object* obj, int width, int height);
		virtual ~DrawableComponent() {};

		int zIndex = 0;

	public:

		Texture* getTexture() const { return (m_texture) ? m_texture.get() : nullptr; }

		SDL_Rect getSourceRect() const { return m_srcrect; }

		SDL_RendererFlip getFlipValue() const { return m_flip; }

		bool drawFullTexture() const { return !m_useSrcRect; }

		bool centerOrigin() const { return m_centerOrigin; }

		SDL_Rect getBounds() const;
		
		virtual void preDrawOperations() {}

	protected:
		void initializeTexture(const std::string& path) { m_texture = std::make_unique<Texture>(path); }

		std::unique_ptr<Texture> m_texture;
		SDL_Rect m_srcrect = { 0 };
		bool m_useSrcRect = false;
		bool m_centerOrigin = false;
		SDL_RendererFlip m_flip = SDL_FLIP_NONE;
	};
}