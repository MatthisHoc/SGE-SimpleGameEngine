#include "components/TextComponent.h"

#include "assistants/Resources.h"
#include "core/Window.h"

namespace sg
{
	TextComponent::TextComponent(Object* obj, const ComponentInitializationData& data) :
		DrawableComponent(obj, data), text(data.text), ptSize(data.ptSize)
	{
		fontPath = data.path;
		m_font = TTF_OpenFont(Resources::pathTo(fontPath).c_str(), data.ptSize);

		if (!m_font)
		{
			throw ComponentException("Failed to open font");
		}
	}

	TextComponent::~TextComponent()
	{
		TTF_CloseFont(m_font);
	}

	void TextComponent::preDrawOperations()
	{
		if (m_texture)
		{
			SDL_DestroyTexture(m_texture->get());
			m_texture = nullptr;
		}

		SDL_Surface* surface = TTF_RenderText_Solid(m_font, text.c_str(), { 255, 255, 255, 255 });
		m_texture = std::make_unique<Texture>(surface);
	}

	void TextComponent::setFontSize(int newPtSize)
	{
		TTF_CloseFont(m_font);
		ptSize = newPtSize;

		m_font = TTF_OpenFont(Resources::pathTo(fontPath).c_str(), ptSize);
	}
}