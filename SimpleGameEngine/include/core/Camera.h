#pragma once
#include "core/vec2.h"

namespace sg
{
	class Camera
	{
	public:
		void move(const vec2& movement)
		{
			position.x += movement.x;
			position.y += movement.y;
		}

	public:
		vec2 position;
		vec2 size;
	private:
		// A Camera can only be instanciated by a window
		friend class Window;
		Camera(const vec2& pos = { 0, 0 }, const vec2& size = { 0, 0 }) :
			position(pos), size(size)
		{}
		
		Camera& operator=(const Camera&) = delete;
		Camera(const Camera&) = delete;
		Camera(Camera&&) = delete;
	};
}

