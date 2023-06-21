#include "components/BoxComponent.h"

namespace sg
{
	BoxComponent::BoxComponent(Object* obj, const ComponentInitializationData& data) :
		Component(obj), size(data.size), drawDebug(data.drawDebug)
	{
		min = { getObject().getPosition().x, getObject().getPosition().y };
		max = min + size;
	}

	bool BoxComponent::isOverlapping(const BoxComponent& other)
	{
		// Ignore if both objects aren't in the same layer
		if (!getObject().matchLayers(other.getObject()))
			return false;

		if (other.active && active)
			return (other.getMin() <= getMax()) && (getMin() <= other.getMax());
		else
			return false;
	}

	bool BoxComponent::isOverlapping(const BoxComponent* other)
	{
		return isOverlapping(*other);
	}

	bool BoxComponent::isOverlapping(const Object* other)
	{			
		BoxComponent* comp = &(other->getComponent<BoxComponent>());

		if (comp)
			return isOverlapping(comp);
		else
			return contains(other->getPosition());
	}

	bool BoxComponent::isOverlapping(const Object& other)
	{
		BoxComponent* comp = &(other.getComponent<BoxComponent>());
		
		if (comp)
			return isOverlapping(comp);
		else
			return contains(other.getPosition());
	}

	bool BoxComponent::contains(const vec2& position, int layers)
	{
		if (!getObject().matchLayers(layers)) return false;

		if (active)
		{
			vec2 min = getMin();
			vec2 max = getMax();

			return (position.x >= min.x && position.x <= max.x) && (position.y >= min.y && position.y <= max.y);
		}
		else return false;
	}

	bool BoxComponent::contains(const Object& other)
	{
		if (!other.matchLayers(getObject())) return false;

		return contains(other.getPosition());
	}

	bool BoxComponent::contains(const Object* other)
	{
		if (!other->matchLayers(getObject())) return false;

		return contains(other->getPosition());
	}

	vec2 BoxComponent::getMin() const
	{
		return getObject().getPosition();
	}

	vec2 BoxComponent::getMax() const
	{
		vec2 position = getObject().getPosition();
		return
		{
			position.x + size.x,
			position.y + size.y
		};
	}

	SDL_Rect BoxComponent::getAsRect() const
	{
		vec2 position = getObject().getPosition();
		SDL_Rect rect;
		rect.x = (int)(position.x); rect.y = (int)(position.y);
		rect.w = (int)size.x; rect.h = (int)size.y;

		return rect;
	}
}
