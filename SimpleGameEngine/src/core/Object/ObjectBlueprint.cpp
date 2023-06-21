#include "core/Object/ObjectBlueprint.h"

#include "core/Parser.h"
#include "components/Component.h"

namespace sg
{
	ObjectBlueprint::ObjectBlueprint(const std::string& filePath)
	{
		Parser parser(sg::Resources::pathTo(filePath));
		initializeFromFile(parser.getMainBlock());
	}

	ObjectBlueprint::ObjectBlueprint(const Block& objectData)
	{
		initializeFromFile(objectData);
	}

	void ObjectBlueprint::initializeFromFile(const Block& objectData)
	{
		m_data.initializeFromFile(objectData);

		// Add scripts names to the blueprint
		auto scriptsBlock = objectData.findBlock("Scripts");
		if (&scriptsBlock != &objectData)
		{
			for (const std::string& scriptName : scriptsBlock.other)
			{
				m_scripts.push_back(scriptName);
			}
		}
		
		auto componentsBlock = objectData.findBlock("Components");
		if (&componentsBlock != &objectData)
		{
			for (const Block& componentData : componentsBlock.subBlocks)
			{
				m_components.push_back(ComponentInitializationData(componentData));
			}
		}

		for (const Block* childObjectData : objectData.findBlocksNotOf({ "Components", "Scripts" }))
		{
			if (childObjectData->has("path"))
			{
				m_children.push_back(ObjectBlueprint(Resources::pathTo(childObjectData->name)));
			}
			else
			{
				m_children.push_back(ObjectBlueprint(*childObjectData));
			}
		}
	}
}