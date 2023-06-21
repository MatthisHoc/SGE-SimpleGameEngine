#pragma once
#include "components/Component.h"
#include "core/Object/Object.h"
#include "core/Layers.h"

#include <SDL.h>

namespace sg
{
	class BoxComponent : public Component
	{
	public:
		BoxComponent(Object* obj, const ComponentInitializationData& data);
		virtual ~BoxComponent() override {}

		bool isOverlapping(const BoxComponent& other);
		bool isOverlapping(const BoxComponent* other);
		bool isOverlapping(const Object& other);
		bool isOverlapping(const Object* other);

		bool contains(const vec2& position, int layers = LAYER_WORLD);
		bool contains(const Object& other);
		bool contains(const Object* other);

		SDL_Rect getAsRect() const;

		// Compute min from Object's position
		vec2 getMin() const;

		// Compute max from Object's position
		vec2 getMax() const;

		bool active = true;
		bool drawDebug = false;

	private:
		vec2 size;

		vec2 min;
		vec2 max;
	};
}

