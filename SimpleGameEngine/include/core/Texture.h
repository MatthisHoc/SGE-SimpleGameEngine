#pragma once

#include <SDL_render.h>
#include <string>
#include <stdexcept>
#include <memory>

#include "core/Cache.h"
#include "core/vec2.h"

namespace sg
{
	class TextureException : public std::exception
	{
	public:
		TextureException(const char* message);
		virtual const char* what() const noexcept override;
	private:
		std::string m_message;
	};

	class Texture
	{
	public:
		Texture(const std::string& path, bool useCache = true);
		Texture(SDL_Surface* surface);
		Texture(int width, int height);
		~Texture();

		void startDrawingOnTexture();
		void renderOnTexture(const Texture& texture, const vec2 &position = { 0.f, 0.f });
		void stopDrawingOnTexture();
		SDL_Texture* get() const { return (m_texture) ? m_texture : m_cachedTexture->get(); }
	private:
		// m_cachedTexture is non_null if we are using a texture from the cache
		std::unique_ptr<CacheRef<std::string, SDL_Texture*>> m_cachedTexture = nullptr;
		// m_texture is non-null if we are not using the cache
		SDL_Texture* m_texture = nullptr;

		bool isTargetTexture = false;
		bool initializedTextureDrawing = false;

		void makeTextureFromCache(const std::string& path);
		void makeTexture(const std::string& path);

		static Cache<std::string, SDL_Texture*> cachedTextures;
	};
	
}