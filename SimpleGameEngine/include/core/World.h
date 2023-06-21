#pragma once

#include <stdexcept>
#include <string>
#include <queue>

namespace sg
{
	class WorldException : public std::exception
	{
	public:
		WorldException(const char* message);
		virtual const char* what() const override;
	private:
		std::string m_message;
	};

	class World
	{
	public:
		World(const std::string& path) : worldPath(path)
		{}

		void load();
		void unload();

		~World() {}

	private:
		std::string worldPath;

		// Pointers to objects that were loaded with this world
		std::queue<class Object*> worldObjects;
	};
}
