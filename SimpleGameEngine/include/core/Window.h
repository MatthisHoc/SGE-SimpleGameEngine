#pragma once

#define SDL_MAIN_HANDLED

#include <SDL_video.h>
#include <SDL_events.h>
#include <SDL_render.h>

#include <vector>
#include <exception>
#include <string>

#include "Camera.h"

namespace sg
{
	class WindowException : public std::exception
	{
	public:
		WindowException(const char* message, bool sdlError = true);
		virtual const char* what() const noexcept override;
	private:
		std::string m_message;
	};

	class Object;
	class Window
	{
	public:
		Window(const char* title, unsigned int x, unsigned int y, unsigned int width, unsigned int height);
		~Window();

		// Processes events and updates engine components. Returns false when a exit event is triggered
		bool processEvents() const;

		// Set the logical size for this window's renderer. The logical size is used to
		// keep proportions when resizing window
		void setLogicalSize(unsigned int logicalWidth, unsigned int logicalHeight);

		// Resize window with the provided width and height
		void resize(unsigned int newWidth, unsigned int newHeight);

		void setFullscreen(bool fullscreen);

		void draw();

		static SDL_Renderer* getRenderer() { return instance->m_renderer; }
		static Camera* getCamera() { return instance->m_camera; }
		static const vec2& getCameraTopLeft() { return instance->m_cameraTopLeft; }
		static const vec2& getWindowSize() { return instance->m_windowSize; }

	private:
		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;
		Window(Window&&) = delete;
		static Window* instance;

		void updateTopLeftCameraPosition();
		void drawTexture(const Object* object, class DrawableComponent* component);
		void drawTexturePortion(const Object* object, class DrawableComponent* component);
		void drawDebugs();
		vec2 positionCamRelative(const vec2& position) const { return position - m_cameraTopLeft; }
		std::vector<std::tuple<Object*, class DrawableComponent*>> getAllDrawableObjects();

	private:
		SDL_Window* m_window = nullptr;
		SDL_Renderer* m_renderer = nullptr;
		vec2 m_windowSize;

		Camera* m_camera;
		vec2 m_cameraTopLeft;
	};
}