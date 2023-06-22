#include <sstream>
#include <chrono>
#include <iostream>

#include "core/Game.h"
#include "core/Parser.h"
#include "core/Object/ObjectBlueprint.h"
#include "components/Component.h"

namespace sg
{
	std::vector<Object*> Game::objects;
	std::queue<Object*> Game::destroyQueue;

	//Quadtree Game::quadtree;

	void Game::setObjectName(Object* obj, const std::string& name)
	{
		unsigned int numObj = static_cast<unsigned int>(objects.size());
		if (name != "")
		{
			obj->setName(name);
		}
		else if (numObj == 1)
		{
			obj->setName("Object");
		}
		else
		{
			std::stringstream ss;
			ss << "Object #";
			ss << numObj;
			obj->setName(ss.str());
		}
	}

	Object& Game::prepareObject(Object* obj)
	{
		objects.push_back(obj);

		//quadtree.insert(obj);

		return *(*(objects.end() - 1));
	}

	Object& Game::instanciate(vec2 pos, const std::string& name)
	{
		Object* obj = new Object(pos);
		setObjectName(obj, name);
		return prepareObject(obj);
	}

	Object& Game::instanciate(const std::string& filePath)
	{
		Object* obj = new Object(filePath);
		return prepareObject(obj);
	}

	Object& Game::instanciate(const Block& objectData)
	{
		Object* obj = new Object(objectData);
		return prepareObject(obj);
	}

	Object& Game::instanciate(const ObjectBlueprint& blueprint)
	{
		Object* obj = new Object(blueprint);
		return prepareObject(obj);
	}

	void Game::destroy(Object& obj)
	{
		destroyQueue.push(&obj);

		for (Object* child : obj.getChildren())
		{
			destroyQueue.push(child);
		}
	}

	void Game::destroy(Component& comp)
	{
		destroy(comp.getObject());
	}

	void Game::destroy(Component* comp)
	{
		destroy(comp->getObject());
	}

	std::vector<Object*>& Game::getAllObjects()
	{
		return objects;
	}

	std::vector<Object*> Game::getAllOrphanObjects()
	{
		std::vector<Object*> orphans;
		for (Object* object : objects)
		{
			if (object->isOrphan())
			{
				orphans.push_back(object);
			}
		}

		return orphans;
	}

	void Game::dispatchUpdates()
	{
		static auto lastFramePoint = std::chrono::steady_clock::now();
		
		auto thisFramePoint = std::chrono::steady_clock::now();

		std::chrono::duration<double> duration = thisFramePoint - lastFramePoint;
		float delta = static_cast<float>(duration.count());
		
		// We don't use iterator here because an update might instanciate
		// a new object
		for (unsigned int i = 0; i < objects.size(); ++i)
		{
			objects[i]->updateScripts(delta);
		}

		// Destroy objects
		processDestruction();

		lastFramePoint = std::chrono::steady_clock::now();
	}

	void Game::processDestruction()
	{
		while (!destroyQueue.empty())
		{
			Object* obj = destroyQueue.front();

			for (auto it = objects.begin(); it < objects.end(); ++it)
			{
				if (*it == obj)
				{
					//quadtree.remove(*it);

					Object* ptr = *it;
					objects.erase(it);
					delete ptr;
					break;
				}
			}

			destroyQueue.pop();
		}
	}
}
