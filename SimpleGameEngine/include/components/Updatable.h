#pragma once

namespace sg
{
	class Updatable
	{
	public:
		virtual void update(float deltaSeconds) = 0;
	};
}