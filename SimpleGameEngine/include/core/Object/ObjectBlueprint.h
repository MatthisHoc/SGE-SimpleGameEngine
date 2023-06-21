#pragma once

#include <vector>
#include <string>

#include "core/Object/ObjectData.h"
#include "core/Game.h"

namespace sg
{
	class ObjectBlueprint
	{
	public:
		ObjectBlueprint(const std::string& filePath);
		ObjectBlueprint(const class Block& objectData);
		ObjectBlueprint(const ObjectBlueprint&) = default;
	private:
		ObjectBlueprint& operator=(const ObjectBlueprint&) = delete;

		void initializeFromFile(const class Block& objectData);

	private:
		ObjectData m_data;
		std::vector<ObjectBlueprint> m_children;
		std::vector<ComponentInitializationData> m_components;
		std::vector<std::string> m_scripts;

	public:
		Object& instanciate() const { return Game::instanciate(*this); }

		const std::vector<ObjectBlueprint>& getChildrenBlueprints() const { return m_children; }
		const std::vector<ComponentInitializationData>& getComponentsData() const { return m_components; }
		const std::vector<std::string>& getScriptsNames() const { return m_scripts; }
		const std::string& getName() const { return m_data.getName(); }
		const ObjectData& getData() const { return m_data; }
	};
}