#include "core/World.h"

#include <vector>

#include "assistants/Resources.h"
#include "core/Parser.h"
#include "core/Game.h"

namespace sg
{

	WorldException::WorldException(const char* message)
	{
		m_message += "[World Exception] ";
		m_message += message;
	}

	const char* WorldException::what() const noexcept
	{
		return m_message.c_str();
	}

	void World::load()
	{
		Parser parser(Resources::pathTo(worldPath));

		// Each block corresponds to an object
		for (const Block& block : parser.getBlocks())
		{
			// If the block has a path specifier, interpret it's name as a path
			if (block.has("path"))
			{
				worldObjects.push(&sg::Game::instanciate(block.name));
			}
			// Otherwise pass the block directly and leave the object's constructor initialize it's data
			else
			{
				worldObjects.push(&sg::Game::instanciate(block));
			}
		}
	}

	void World::unload()
	{
		while (!worldObjects.empty())
		{
			// Destroy every objects instantiated with this world
			sg::Game::destroy(worldObjects.front());
			worldObjects.pop();
		}
	}
}