#include <stdarg.h>
#include <iostream>

#include "core/Object/Object.h"
#include "core/Parser.h"
#include "core/Object/ObjectBlueprint.h"

#include "components/Updatable.h"
#include "components/AnimatedTextureComponent.h"
#include "components/BoxComponent.h"
#include "components/TextComponent.h"
#include "components/TextureComponent.h"
#include "components/TilesetComponent.h"

namespace sg
{
	std::ostream& operator<<(std::ostream& out, const Object& obj)
	{
		out << obj.getName();
		return out;
	}

	Object::Object(const vec2& position)
	{
		this->setPosition(position);
	}

	Object::Object(const std::string& filePath)
	{
		Parser parser(sg::Resources::pathTo(filePath));
		initializeFromFile(parser.getMainBlock());
	}

	Object::Object(const Block& objectData)
	{
		initializeFromFile(objectData);
	}

	void Object::addComponentFromData(const ComponentInitializationData& data)
	{
		switch (data.type)
		{
		case ComponentTypes::AnimatedTextureComponent:
			addComponent<class AnimatedTextureComponent>(data);
			break;
		case ComponentTypes::TextComponent:
			addComponent<class TextComponent>(data);
			break;
		case ComponentTypes::TextureComponent:
			addComponent<class TextureComponent>(data);
			break;
		case ComponentTypes::TilesetComponent:
			addComponent<class TilesetComponent>(data);
		default:
			addComponent<class BoxComponent>(data);
			break;
		}
	}

	Object::Object(const ObjectBlueprint& blueprint) :
		m_data(blueprint.getData())
	{
		for (const auto& compData : blueprint.getComponentsData())
		{
			addComponentFromData(compData);
		}

		for (const auto& scriptName : blueprint.getScriptsNames())
		{
			addScriptFromStr(scriptName);
		}

		for (const ObjectBlueprint& child : blueprint.getChildrenBlueprints())
		{
			attach(child.instanciate());
		}
	}

	void Object::initializeFromFile(const Block& objectData)
	{		
		// Initialize object data
		m_data.initializeFromFile(objectData);

		// add components and scripts
		const auto& componentsBlock = objectData.findBlock("Components");
		// Compare object addresses to see if a match was found
		if (&componentsBlock != &objectData)
		{
			addComponentsFromParser(componentsBlock);
		}

		const auto& scriptsBlock = objectData.findBlock("Scripts");
		// Compare addresses to see if a match was found
		if (&scriptsBlock != &objectData)
		{
			addScriptsFromParser(scriptsBlock);
		}

		// Get all blocks that are not Components or Scripts and treat them as children objects
		for (const Block* childObjectData : objectData.findBlocksNotOf({ "Components", "Scripts" }))
		{
			if (childObjectData->has("path"))
			{
				// Instanciate object by treating it's name as the path to another .sgo file
				// and attach it
				attach(Game::instanciate(Resources::pathTo(childObjectData->name)));
			}
			else
			{
				// Instanciate object by treating the block as a whole object
				// and attach it
				attach(Game::instanciate(*childObjectData));
			}
		}
	}

	void Object::addComponentsFromParser(const Block& componentsBlock)
	{
		// Each subBlock should contain data for a component
		for (const Block& componentData : componentsBlock.subBlocks)
		{
			ComponentInitializationData data(componentData);
			addComponentFromData(data);
		}
	}

	void Object::addScriptsFromParser(const Block& scriptsBlock)
	{
		for (const std::string& scriptName : scriptsBlock.other)
		{
			addScriptFromStr(scriptName);
		}
	}

	Object::~Object()
	{
		for (ScriptComponent* script : m_scripts)
		{
			delete script;
		}

		for (Component* comp : m_components)
		{
			delete comp;
		}
	}

	vec2 Object::getPosition() const
	{
		if (m_parent == nullptr)
		{
			return m_data.position;
		}
		else
		{
			return m_data.position + m_parent->getPosition();
		}
	}

	vec2 Object::getSize() const
	{
		if (m_parent == nullptr)
		{
			return m_data.size;
		}
		else
		{
			return m_data.size + m_parent->getSize();
		}
	}

	void Object::updateScripts(float deltaSeconds)
	{
		for (auto comp : m_components)
		{
			if (sg::Updatable* updatable = dynamic_cast<sg::Updatable*>(comp))
			{
				updatable->update(deltaSeconds);
			}
		}

		for (auto it = m_scripts.begin(); it < m_scripts.end(); ++it)
		{
			auto* script = (*it);

			while (!m_scriptBeginQueue.empty())
			{
				m_scriptBeginQueue.front()->begin();
				m_scriptBeginQueue.pop();
			}

			if (script->canUpdate)
			{
				script->update(deltaSeconds);
			}
		}
	}

	void Object::removeComponent(Component* comp)
	{
		for (auto it = m_components.begin(); it < m_components.end(); ++it)
		{
			if (comp == *it)
			{
				m_components.erase(it);
				return;
			}
		}

		std::string errorMessage(getName() + ": Can't remove component. Argument is either nullptr or not in this object");
		throw ObjectException(errorMessage.c_str());
	}

	void Object::removeScript(ScriptComponent* scriptToRemove)
	{
		for (auto it = m_scripts.begin(); it != m_scripts.end(); ++it)
		{
			if (scriptToRemove == (*it))
			{
				m_scripts.erase(it);
				return;
			}
		}

		std::string errorMessage(getName() + ": Can't remove script. Argument is either nullptr or not in this object");
		throw ObjectException(errorMessage.c_str());
	}

	void Object::attach(Object* object)
	{
		if (object->m_parent == nullptr)
		{
			// Set attached object's parent
			object->m_parent = this;
			// Add the newly attached object to the children list
			this->m_children.push_back(object);
		}
		else
		{
			std::string errorMessage(getName() + ": Can't attach object " + object->getName() + " because it already has a parent");
			throw ObjectException(errorMessage.c_str());
		}
	}

	void Object::dettach(Object* object)
	{
		if (object->m_parent == this)
		{
			// Remove parent of previously attached object
			object->m_parent = nullptr;

			// Find object in children list and remove it
			// It's guaranteed that the children is in this list if it's parent is valid
			for (auto it = m_children.begin(); it != m_children.end(); ++it)
			{
				if (*it == object)
				{
					this->m_children.erase(it);
					break;
				}
			}
		}
		else
		{
			std::string errorMessage(getName() + ": Can't dettach object " + object->getName() + " because it's parent is nullptr or not the same as the target object");
			throw ObjectException(errorMessage.c_str());
		}
	}

	void Object::attachTo(Object* object)
	{
		// Same as attach but the parent object is the argument
		object->attach(this);
	}

	void Object::dettachFrom(Object* object)
	{
		// Same as dettach but the parent object is the argument
		object->dettach(this);
	}

	// Overloads of the 4 previous functions with reference parameters
	void Object::attach(Object& object)
	{
		attach(&object);
	}
	void Object::dettach(Object& object)
	{
		dettach(&object);
	}
	void Object::attachTo(Object& object)
	{
		attachTo(&object);
	}
	void Object::dettachFrom(Object& object)
	{
		dettachFrom(&object);
	}
}
