#include <string>
#include <SDL.h>
#include <SDL_render.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <tuple>
#include <algorithm>

#include "core/Window.h"
#include "core/Input.h"
#include "core/Camera.h"
#include "core/Game.h"
#include "core/Quadtree.h"
#include "core/Audio.h"

#include "components/DrawableComponent.h"
#include "components/BoxComponent.h"

namespace sg
{
	WindowException::WindowException(const char* message, bool sdlError)
	{
		m_message += "[Window Error] ";
		m_message += message;
		if (sdlError)
		{
			m_message += ": ";
			m_message += SDL_GetError();
		}
	}

	const char* WindowException::what() const noexcept
	{
		return m_message.c_str();
	}

	Window* Window::instance = nullptr;
	using DrawablePair = std::tuple<Object*, DrawableComponent*>;

	Window::Window(const char* title, unsigned int x, unsigned int y, unsigned int width, unsigned int height)
	{
		if (instance != nullptr)
		{
			throw WindowException("Can't instanciate another window", false);
		}

		instance = this;
		SDL_Init(SDL_INIT_EVERYTHING);
		TTF_Init();

		m_window = SDL_CreateWindow(title, x, y, width, height, SDL_WINDOW_OPENGL);

		if (m_window == nullptr)
		{
			throw WindowException("Failed to create window");
		}

		m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

		if (m_renderer == nullptr)
		{
			throw WindowException("Failed to create renderer");
		}

		SDL_ShowWindow(m_window);

		m_camera = new Camera();

		// Center the camera
		m_camera->position.x = width / 2.f;
		m_camera->position.y = height / 2.f;

		// Size the camera with the window's size
		m_camera->size.x = (float)width;
		m_camera->size.y = (float)height;

		// Initialize top left coordinates to 0;0
		m_cameraTopLeft = { 0.f, 0.f };

		// keep window size information stored
		m_windowSize.x = (float)width;
		m_windowSize.y = (float)height;

		Audio::initialize();
	}

	Window::~Window()
	{
		Audio::quit();
		SDL_DestroyWindow(m_window);
		SDL_DestroyRenderer(m_renderer);
		TTF_Quit();
		SDL_Quit();
		instance = nullptr;
	}

	void Window::setLogicalSize(unsigned int logicalWidth, unsigned int logicalHeight)
	{
		SDL_RenderSetLogicalSize(m_renderer, (int)logicalWidth, (int)logicalHeight);
	}

	bool Window::processEvents() const
	{
		// Updating input sub-system is used to determine when a key is up/down
		Input::update();

		SDL_Event pendingEvent;
		while (SDL_PollEvent(&pendingEvent))
		{
			switch (pendingEvent.type)
			{
			case SDL_QUIT:
				return false;
				break;

			// Update mouse position
			case SDL_MOUSEMOTION:
				Input::updateMouse(pendingEvent.motion);
				break;

			// Add or remove mouse buttons
			case SDL_MOUSEBUTTONDOWN:
				Input::addButton(pendingEvent.button.button);
				break;
			case SDL_MOUSEBUTTONUP:
				Input::removeButton(pendingEvent.button.button);
				break;

			// add or remove pressed keys
			case SDL_KEYDOWN:
				Input::addKey(pendingEvent.key.keysym.sym);
				break;
			case SDL_KEYUP:
				Input::removeKey(pendingEvent.key.keysym.sym);
				break;
			}
		}

		// Update all scripts
		Game::dispatchUpdates();

		return true;
	}

	void Window::setFullscreen(bool fullscreen)
	{
		SDL_SetWindowFullscreen(m_window, (int)fullscreen);
	}

	void Window::resize(unsigned int newWidth, unsigned int newHeight)
	{
		// Calculate by how much the window grows/shrinks
		// Negative value when the window grows, positive when it shrinks
		int xDelta = (int)m_windowSize.x - (int)newWidth;
		int yDelta = (int)m_windowSize.y - (int)newHeight;

		// Change window size and update the m_windowSize member variable
		SDL_SetWindowSize(m_window, (int)newWidth, (int)newHeight);
		m_windowSize.x = (float)newWidth;
		m_windowSize.y = (float)newHeight;

		// Retrieve current window position
		int currentX, currentY;
		SDL_GetWindowPosition(m_window, &currentX, &currentY);

		int newPosX = currentX + xDelta / 2, newPosY = currentY + yDelta / 2;
		// Offset the window position to keep it centered
		SDL_SetWindowPosition(m_window, newPosX, newPosY);

		// Size and position the camera with the window's size
		m_camera->position.x -= (float)(xDelta / 2);
		m_camera->position.y -= (float)(yDelta / 2);
		m_camera->size.x = (float)newWidth;
		m_camera->size.y = (float)newHeight;
	}

	void Window::updateTopLeftCameraPosition()
	{
		// Camera location to top left corner of screen
		m_cameraTopLeft.x = m_camera->position.x - (m_windowSize.x / 2);
		m_cameraTopLeft.y = m_camera->position.y - (m_windowSize.y / 2);
	}

	void Window::drawTexture(const Object* object, DrawableComponent* component)
	{
		SDL_Rect textureInfo;
		SDL_QueryTexture(component->getTexture()->get(), nullptr, nullptr, &(textureInfo.w), &(textureInfo.h));
		textureInfo.h *= (int)object->getSize().y;
		textureInfo.w *= (int)object->getSize().x;

		// determine upper left corner location of the component
		vec2 componentPositionUpperLeftCorner;
		vec2 componentPosition = object->getPosition();
		if (component->centerOrigin())
		{
			componentPositionUpperLeftCorner.x = (componentPosition.x - (textureInfo.w / 2));
			componentPositionUpperLeftCorner.y = (componentPosition.y - (textureInfo.h / 2));
		}
		else
		{
			componentPositionUpperLeftCorner.x = componentPosition.x;
			componentPositionUpperLeftCorner.y = componentPosition.y;
		}

		if (object->isScreenPosition())
		{
			textureInfo.x = (int)(componentPositionUpperLeftCorner.x);
			textureInfo.y = (int)(componentPositionUpperLeftCorner.y);
		}
		// Offset object by camera translation
		else
		{
			vec2 objCamRelative = positionCamRelative(componentPositionUpperLeftCorner);
			textureInfo.x = (int)(objCamRelative.x);
			textureInfo.y = (int)(objCamRelative.y);
		}

		SDL_RenderCopyEx(m_renderer, component->getTexture()->get(), nullptr, &textureInfo, 0.0f , nullptr, component->getFlipValue());
	}

	void Window::drawTexturePortion(const Object* object, DrawableComponent* component)
	{
		SDL_Rect textureInfo;
		textureInfo.w = component->getSourceRect().w * (int)object->getSize().x;
		textureInfo.h = component->getSourceRect().h * (int)object->getSize().y;

		// determine upper left corner location of the component
		vec2 componentPositionUpperLeftCorner;
		vec2 componentPosition = object->getPosition();
		if (component->centerOrigin())
		{
			componentPositionUpperLeftCorner.x = (componentPosition.x - (textureInfo.w / 2));
			componentPositionUpperLeftCorner.y = (componentPosition.y - (textureInfo.h / 2));
		}
		else
		{
			componentPositionUpperLeftCorner.x = componentPosition.x;
			componentPositionUpperLeftCorner.y = componentPosition.y;
		}

		// Offset object by camera translation
		if (object->isScreenPosition())
		{
			textureInfo.x = (int)(componentPositionUpperLeftCorner.x);
			textureInfo.y = (int)(componentPositionUpperLeftCorner.y);
		}
		else
		{
			vec2 objCamRelative = positionCamRelative(componentPositionUpperLeftCorner);
			textureInfo.x = (int)(objCamRelative.x);
			textureInfo.y = (int)(objCamRelative.y);
		}

		SDL_Rect compSourceRect = component->getSourceRect();
		SDL_RenderCopyEx(m_renderer, component->getTexture()->get(), &compSourceRect, &textureInfo, 0.0f, nullptr, component->getFlipValue());
	}

	std::vector<DrawablePair> Window::getAllDrawableObjects()
	{
		auto objects = Game::getAllOrphanObjects();

		// initialize a vector that contains tuples
		std::vector<DrawablePair> drawables;

		// gather drawable objects
		for (Object* obj : objects)
		{
			// draw object if it's flagged as visible and has DrawableComponents
			if (obj->isVisible())
			{
				for (DrawableComponent* component : obj->getComponents<DrawableComponent>())
				{
					Object* componentOwner = &component->getObject();
					drawables.push_back(std::make_tuple(componentOwner, component));
				}
			}
		}

		return drawables;
	}

	void Window::drawDebugs()
	{
#ifdef _DEBUG
		SDL_Rect objRect;
		objRect.w = objRect.h = 4;
		for (Object* obj : Game::getAllObjects())
		{
			for (BoxComponent* box : obj->getComponents<BoxComponent>())
			{
				// Find box components to debug draw
				if (box->drawDebug)
				{
					SDL_SetRenderDrawColor(m_renderer, 255, 0, 0, 255);
					SDL_Rect rect = box->getAsRect();

					// If owner's position is not screen position the rectangle relative
					// to the camera 
					if (!obj->isScreenPosition())
					{
						vec2 positionCam = positionCamRelative({ (float)rect.x, (float)rect.y });
						rect.x = (int)positionCam.x; rect.y = (int)positionCam.y;
					}

					SDL_RenderDrawRect(m_renderer, &rect);
				}
			}

			// Draw object debug as well if needed
			if (obj->shouldDrawDebug())
			{
				// Position relative to camera if needed
				vec2 position = (obj->isScreenPosition()) ? obj->getPosition() : positionCamRelative(obj->getPosition());
				objRect.x = (int)(position.x - 2.0f);
				objRect.y = (int)(position.y - 2.0f);

				SDL_SetRenderDrawColor(m_renderer, 143, 225, 255, 255);
				SDL_RenderDrawRect(m_renderer, &objRect);
			}
		}
#endif
	}

	void Window::draw()
	{
		// Clear screen
		SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 0);
		SDL_RenderClear(m_renderer);

		updateTopLeftCameraPosition();

		// Gather all drawable objects
		std::vector<DrawablePair> drawables = getAllDrawableObjects();

		// Sort drawable objects according to their zIndex property
		std::sort(drawables.begin(), drawables.end(),
			[](const DrawablePair& a, const DrawablePair& b) -> bool
			{
				return std::get<1>(a)->zIndex < std::get<1>(b)->zIndex;
			});

		// Draw objects
		for (DrawablePair& tuple : drawables)
		{
			auto* component = std::get<1>(tuple);

			component->preDrawOperations();
			if (component->drawFullTexture())
			{
				drawTexture(std::get<0>(tuple), component);
			}
			else
			{
				drawTexturePortion(std::get<0>(tuple), component);
			}
		}

		drawDebugs();
	
		//auto tiles = Game::getQuadtree().computeDrawData();
		//for (SDL_Rect& tile : tiles)
		//{
		//	SDL_Rect rect = tile;
		//	rect.x -= (int)m_cameraTopLeft.x;
		//	rect.y -= (int)m_cameraTopLeft.y;
		//	SDL_RenderDrawRect(m_renderer, &rect);
		//}

		SDL_RenderPresent(m_renderer);
	}
}