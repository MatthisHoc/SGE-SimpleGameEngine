#include "components/TilesetComponent.h"
#include "assistants/Resources.h"
#include <iostream>

namespace sg
{
	TilesetComponent::TilesetComponent(Object* obj, const ComponentInitializationData& data)
		: TextureComponent(obj, data), margin(data.margin), spacing(data.spacing)
	{
		m_useSrcRect = true;
		m_srcrect.w = (int)data.size.x;
		m_srcrect.h = (int)data.size.y;

		SDL_Rect bounds = getBounds();

		int width = (bounds.w - margin * 2);
		width -= ((int)(width / (int)m_srcrect.w) - 1) * spacing;
		numTilesWidth = width / (int)m_srcrect.w;

		int height = (bounds.h - margin * 2);
		height -= ((int)(height / (int)m_srcrect.h) - 1) * spacing;
		numTilesHeight = height / (int)m_srcrect.h;
		
		setIndex(0);
	}

	void TilesetComponent::setIndex(int newIndex)
	{
		tileIndex = newIndex;
		int tileX = tileIndex % numTilesWidth;
		int tileY = tileIndex / numTilesWidth;
		m_srcrect.x = margin + m_srcrect.w * tileX + spacing * tileX;
		m_srcrect.y = margin + m_srcrect.h * tileY + spacing * tileY;
	}
}