#pragma once
#include <vector>
#include <stdarg.h>
#include <type_traits>
#include <string>
#include <queue>
#include <utility>

//#include "components/Component.h"
//#include "components/ScriptComponent.h"
#include "components/ComponentInitializationData.h"
#include "core/vec2.h"
#include "core/Layers.h"
#include "core/Object/ObjectData.h"

namespace sg
{
	class Component;
	class ScriptComponent;

	class Object
	{
	public:
		~Object();
	protected:
		friend class Game;
		Object(const vec2& pos = { 0, 0 });
		Object(const std::string& filePath);
		Object(const class Block& objectData);
		Object(const class ObjectBlueprint& blueprint);

		void addScriptFromStr(const std::string& scriptName);
		void addComponentFromData(const ComponentInitializationData& data);
	private:
		Object& operator=(const Object&) = delete;

		// This function is meant to be called only by getComponent<T>() of a parent
		// It returns the component as a pointer instead of a reference
		// and returns nullptr instead of throwing and error when none can be found
		template <typename T>
		T* getComponentChild() const
		{
			// Check in list of component if any matches the template type
			for (Component* comp : m_components)
			{
				if (T* cast = dynamic_cast<T*>(comp))
				{
					return cast;
				}
			}

			return nullptr;
		}

		// This function is meant to be called only by getScript<T>() of a parent
		// It returns the script as a pointer instead of a reference
		// and returns nullptr instead of throwing and error when none can be found
		template <typename T>
		T* getScriptChild() const
		{
			for (ScriptComponent* comp : m_scripts)
			{
				if (T* cast = dynamic_cast<T*>(comp))
				{
					return cast;
				}
			}

			return nullptr;
		}

		void initializeFromFile(const Block& objectData);
		void addComponentsFromParser(const class Block& componentsBlock);
		void addScriptsFromParser(const class Block& scriptsBlock);

		void updateScripts(float deltaSeconds);

	private:
		ObjectData m_data;
		Object* m_parent = nullptr;
		std::vector<Object*> m_children;
		std::vector<Component*> m_components;
		std::vector<ScriptComponent*> m_scripts;
		std::queue<ScriptComponent*> m_scriptBeginQueue;

	public:

		void attach(Object* object);
		void attach(Object& object);
		void dettach(Object* object);
		void dettach(Object& object);
		void attachTo(Object* object);
		void attachTo(Object& object);
		void dettachFrom(Object* object);
		void dettachFrom(Object& object);

		// Returns true if this object layers match at least one layer of the other object
		bool matchLayers(int otherLayers) const { return m_data.layers & otherLayers; }
		// Returns true if this object layers match at least one layer of the other object
		bool matchLayers(const Object& other) const { return matchLayers(other.m_data.layers); }

		template <typename TComponent, typename ... TArgs>
		TComponent& addComponent(TArgs&&... args)
		{
			TComponent* ptr = new TComponent(this, std::forward<TArgs>(args)...);
			m_components.push_back(ptr);
			return *ptr;
		}

		void removeComponent(Component* comp);

		// Returns nullptr if the component can't be found or a pointer to the 
		// first component of the provided type.
		template <typename T>
		T* hasComponent() const
		{
			// Check in list of component if any matches the template type
			for (Component* comp : m_components)
			{
				if (T* cast = dynamic_cast<T*>(comp))
				{
					return cast;
				}
			}

			// Otherwise traverse children to find a component of the template type
			for (Object* child : m_children)
			{
				T* comp = child->getComponentChild<T>();
				if (comp)
				{
					return comp;
				}
			}

			return nullptr;
		}

		// Returns a reference to the first component of the provided type.
		// Use this if you are sure that the object has a component otherwise use
		// hasComponent<T>()
		template <typename T>
		T& getComponent() const
		{
			if (T* comp = hasComponent<T>())
			{
				return *comp;
			}
			else
			{
				std::string errorMessage(getName() + ": Could not find a component of the requested type");
				throw ObjectException(errorMessage.c_str());
			}
		}

		template <typename T>
		std::vector<T*> getComponents() const
		{
			std::vector<T*> components;
			// Add all matching components of this object
			for (Component* comp : m_components)
			{
				if (T* cast = dynamic_cast<T*>(comp))
				{
					components.push_back(cast);
				}
			}

			// Insert all matching components of children as well
			for (Object* child : m_children)
			{
				std::vector<T*> childComps = child->getComponents<T>();
				components.insert(components.end(), childComps.begin(), childComps.end());
			}

			return components;
		}

		template <typename T>
		T& addScript(bool canUpdate = true)
		{
			T* ptr = new T;
			ptr->m_object = this;
			ptr->canUpdate = canUpdate;
			m_scripts.push_back(ptr);
			m_scriptBeginQueue.push(ptr);

			return *ptr;
		}

		void removeScript(ScriptComponent* script);

		// Returns nullptr if the script can't be found or a pointer to the 
		// first script of the provided type.
		template <typename T>
		T* hasScript() const
		{
			// Look for script in this object first
			for (ScriptComponent* comp : m_scripts)
			{
				if (T* cast = dynamic_cast<T*>(comp))
				{
					return cast;
				}
			}

			// Then look for the script in children
			for (Object* child : m_children)
			{
				T* script = child->getScriptChild<T>();
				if (script)
				{
					return script;
				}
			}

			return nullptr;
		}

		// Returns a reference to the first script of the provided type.
		// Use this if you are sure that the object has a script otherwise use
		// hasScript<T>()
		template <typename T>
		T& getScript() const
		{
			if (T* script = hasScript<T>())
			{
				return *script;
			}
			else
			{
				std::string errorMessage(getName() + ": Could not find a script of the requested type");
				throw ObjectException(errorMessage.c_str());
			}
		}

		friend std::ostream& operator<<(std::ostream& out, const Object& obj);

		const std::string& getName() const { return m_data.getName(); }
		void setName(const std::string& newName) { m_data.setName(newName); }

		unsigned int getNumScript() const { return static_cast<unsigned int>(m_scripts.size()); }

		bool isOrphan() const { return m_parent == nullptr; }
		std::vector<Object*> getChildren() { return m_children; }

		vec2 getRelativePosition() const { return m_data.position; }
		vec2 getPosition() const;
		void setPosition(const vec2& newPosition) { m_data.position = newPosition; }
		void addToPosition(const vec2& addPosition) { m_data.position = m_data.position + addPosition; }

		vec2 getRelativeSize() const { return m_data.size; }
		vec2 getSize() const;
		void setSize(const vec2& newSize) { m_data.size = newSize; }
		bool isScreenPosition() const { return m_data.screenPosition; }
		bool isVisible() const { return m_data.visible; }
		void setVisibility(bool newVisibility) { m_data.visible = newVisibility; }
		bool shouldDrawDebug() const { return m_data.drawDebug; }
	};
}

