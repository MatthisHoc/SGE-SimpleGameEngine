#include "core/Texture.h"
#include "core/Window.h"
#include "assistants/Resources.h"

#include <SDL_image.h>
#include <SDL_render.h>
#include <memory>

namespace sg
{
	Cache<std::string, SDL_Texture*> Texture::cachedTextures;

	TextureException::TextureException(const char* message) : m_message(message) {}

	const char* TextureException::what() const { return m_message.c_str(); }

	void Texture::makeTextureFromCache(const std::string& path)
	{
		// Look for texture in cache
		std::pair<CacheRef<std::string, SDL_Texture*>, bool> ref = cachedTextures.find(path);
		if (ref.second)
		{
			m_cachedTexture = std::make_unique<CacheRef<std::string, SDL_Texture*>>(ref.first);
		}
		// Otherwise load a new texture and add it to the cache
		else
		{
			SDL_Surface* surface = IMG_Load(path.c_str());

			if (surface == nullptr)
			{
				throw TextureException("Failed to load image");
			}

			m_cachedTexture = std::make_unique<CacheRef<std::string, SDL_Texture*>>(cachedTextures.add(path, SDL_CreateTextureFromSurface(Window::getRenderer(), surface)));
			SDL_FreeSurface(surface);
		}
	}

	void Texture::makeTexture(const std::string& path)
	{
		SDL_Surface* surface = IMG_Load(Resources::pathTo(path).c_str());

		if (surface == nullptr)
		{
			throw TextureException("Failed to load image");
		}

		m_texture = SDL_CreateTextureFromSurface(Window::getRenderer(), surface);
		SDL_FreeSurface(surface);
	}

	Texture::Texture(const std::string& path, bool useCache)
	{
		if (useCache)
		{
			makeTextureFromCache(path);
		}
		else
		{
			makeTexture(path);
		}
	}

	Texture::Texture(SDL_Surface* surface)
	{
		m_texture = SDL_CreateTextureFromSurface(Window::getRenderer(), surface);
		SDL_FreeSurface(surface);
	}

	Texture::Texture(int width, int height)
	{
		m_texture = SDL_CreateTexture(Window::getRenderer(), SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, width, height);
		isTargetTexture = true;
	}

	Texture::~Texture()
	{
		// If we are not relying on the cache for this texture we must destroy it
		if (m_texture)
		{
			SDL_DestroyTexture(m_texture);
		}

		// Make sure this texture is the last reference to the texture in the cache
		if (m_cachedTexture && m_cachedTexture->getNumRef() == 1)
		{
			SDL_DestroyTexture(m_cachedTexture->get());
		}
	}

	void Texture::startDrawingOnTexture()
	{
		if (isTargetTexture)
		{
			SDL_SetRenderTarget(Window::getRenderer(), m_texture);
			initializedTextureDrawing = true;
		}
	}

	void Texture::renderOnTexture(const Texture& texture, const vec2& position)
	{
		if (isTargetTexture && initializedTextureDrawing)
		{
			int w, h;
			SDL_QueryTexture(texture.get(), nullptr, nullptr, &w, &h);
			SDL_Rect textureInfo{ 0 };
			textureInfo.x = (int)position.x;
			textureInfo.y = (int)position.y;
			textureInfo.w = w;
			textureInfo.h = h;
			SDL_RenderCopyEx(Window::getRenderer(), texture.get(), nullptr, &textureInfo, 0.f, nullptr, SDL_FLIP_NONE);
		}
	}

	void Texture::stopDrawingOnTexture()
	{
		if (isTargetTexture && initializedTextureDrawing)
		{
			initializedTextureDrawing = false;
			SDL_SetRenderTarget(Window::getRenderer(), nullptr);
		}
	}
}