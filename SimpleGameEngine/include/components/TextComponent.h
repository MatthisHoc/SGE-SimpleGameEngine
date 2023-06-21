#pragma once

#include <string>
#include <SDL_ttf.h>
#include <SDL_render.h>

#include "components/DrawableComponent.h"
#include "core/Object/Object.h"

namespace sg
{
	class TextComponent : public DrawableComponent
	{
	public:
		TextComponent(Object* obj, const ComponentInitializationData& data);
		virtual ~TextComponent() override;

		virtual void preDrawOperations() override;

	public:
		std::string text;

		void setFontSize(int newPtSize);
		int getFontSize() { return ptSize; }

	private:
		int ptSize;
		std::string fontPath;
		TTF_Font* m_font;
	};
}