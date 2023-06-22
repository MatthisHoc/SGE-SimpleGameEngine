#include "core/Object/ObjectData.h"
#include "core/Parser.h"

namespace sg
{
	ObjectException::ObjectException(const char* message)
	{
		m_message += "[Object Exception] ";
		m_message += message;
	}

	const char* ObjectException::what() const noexcept
	{
		return m_message.c_str();
	}

	ObjectData::ObjectData(const ObjectData& other):
		position(other.position),
		size(other.size),
		visible(other.visible),
		screenPosition(other.screenPosition),
		drawDebug(other.drawDebug),
		layers(other.layers),
		m_name(other.m_name)
	{

	}

	ObjectData::~ObjectData()
	{
		
	}

	void ObjectData::initializeFromFile(const Block& objectData)
	{
		m_name = objectData.name;
		position = objectData.getVec2("position");
		size = objectData.getVec2("size", { 1.0f, 1.0f });
		visible = !objectData.has("not-visible");
		screenPosition = objectData.has("screen-position");
		drawDebug = objectData.has("draw-debug");

		// Should be replaced with a more maintainable solution in the future
		if (objectData.has("layer-world"))
			layers |= LAYER_WORLD;

		if (objectData.has("layer-ui"))
			layers |= LAYER_UI;
	}
}