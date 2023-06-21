#pragma once
#include <SDL_render.h>
#include <exception>
#include <string>

#include "core/Object/Object.h"
#include "components/DrawableComponent.h"

namespace sg
{
	class TextureComponent : public DrawableComponent
	{
	public:
		TextureComponent(Object* obj, const ComponentInitializationData& data);

		virtual ~TextureComponent() override {};
	};
}

