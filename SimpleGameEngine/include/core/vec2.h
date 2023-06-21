#pragma once

namespace sg
{
	struct vec2
	{
		float x;
		float y;

		bool operator<=(const vec2& other) const { return (x <= other.x) && (y <= other.y); }
		vec2 operator+(const vec2& other) const { return { x + other.x, y + other.y }; }
		vec2 operator-(const vec2& other) const { return { x - other.x, y - other.y }; }
		vec2 operator*(float scalar) const { return { x * scalar, y * scalar }; }
	};
}