#pragma once
#include <vector>
#include <queue>
#include <string>

#include "Object/Object.h"
#include "World.h"
#include "Camera.h"
#include "Quadtree.h"

namespace sg
{
	class Game
	{
	public:
		//static const Quadtree& getQuadtree() { return quadtree; }
		// Instanciate a new object and returns a reference to it
		static Object& instanciate(vec2 pos = { 0, 0 }, const std::string& name = ""); 
		static Object& instanciate(const std::string& filePath);
		static Object& instanciate(const class Block& objectData);
		static Object& instanciate(const class ObjectBlueprint& blueprint);

		// Destroys an object, the reference will become invalid after the call to this function
		static void destroy(Object& obj);
		// Destroys an object, the pointer will point to trash data after the call to this function
		static void destroy(Object* obj) { destroy(*obj); }
		// Destroys the owning oject of this component. Use Object::removeComponent() to remove the component
		static void destroy(Component& comp) { destroy(comp.getObject()); }
		// Destroys the owning oject of this component. Use Object::removeComponent() to remove the component
		static void destroy(Component* comp) { destroy(comp->getObject()); }

		static std::vector<Object*>& getAllObjects();
		static std::vector<Object*> getAllOrphanObjects();
		static void dispatchUpdates();
	private:
		Game() = delete;
		Game(const Game&) = delete;
		Game& operator=(const Game&) = delete;
		Game(Game&&) = delete;

		static Object& prepareObject(Object* obj);
		static void setObjectName(Object* obj, const std::string& name = "");
		static void processDestruction();

	private:
		static std::vector<Object*> objects;
		static std::queue <Object*> destroyQueue;
		//static Quadtree quadtree;
	};
}

