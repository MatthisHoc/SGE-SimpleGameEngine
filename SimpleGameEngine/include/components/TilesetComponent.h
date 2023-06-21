#pragma once

#include "TextureComponent.h"

namespace sg
{
	class TilesetComponent : public TextureComponent
	{
	public:
		TilesetComponent(Object* obj, const ComponentInitializationData& data);
		
		virtual ~TilesetComponent() override {};

		void setIndex(int newIndex);
		int getIndex() const { return tileIndex; }

	private:
		int tileIndex = 0;
		int margin = 0;
		int spacing = 0;
		int numTilesWidth = 0;
		int numTilesHeight = 0;
	};
}
