#include <SDL_image.h>

#include "components/AnimatedTextureComponent.h"
#include "components/ComponentInitializationData.h"

#include "core/Window.h"
#include "core/Object/Object.h"
#include "core/Parser.h"

namespace sg
{
	AnimatedTextureComponent::AnimationCache AnimatedTextureComponent::animCache;
	std::unordered_map<std::string, std::string> AnimatedTextureComponent::texturePaths;

	void AnimatedTextureComponent::loadAnimationFromCache(const std::string& filePath, const AnimatedTextureComponent::AnimationCacheRef& cacheRef)
	{
		// Look for a texture path that matches this animation file and
		// initialize the texture
		auto kvp = texturePaths.find(filePath);
		if (kvp != texturePaths.end())
		{
			initializeTexture(Resources::pathTo(kvp->second));
		}

		// Retrieve cache reference
		animations = std::make_unique<AnimatedTextureComponent::AnimationCacheRef>(cacheRef);

		// Look through animations and check if one should auto-play
		for (std::pair<std::string, Animation> anim : animations->get())
		{
			if (anim.second.autoPlay)
			{
				playAnimation(anim.first);
				break;
			}
		}
	}

	void AnimatedTextureComponent::parseAnimationsAndCache(const std::string& filePath)
	{
		Parser parser(filePath);

		// Set the texture to use (it's either loaded or retrieved in the texture cache)
		initializeTexture(Resources::pathTo(parser.getMainBlockName()));
		// Add the path of the texture to the list of texture paths
		texturePaths.insert(std::make_pair(filePath, parser.getMainBlockName()));

		const Block& anims = parser.getMainBlock();

		// Fill the cache with an empty map and retrieve the reference
		animations = std::make_unique<AnimatedTextureComponent::AnimationCacheRef>
			(
				animCache.add
				(
					filePath,
					std::map<std::string, Animation>(),
					anims.has("persistent")
				)
			);

		// Fill the map we previously created with data in the file
		Animation animData;
		for (const Block& anim : anims.subBlocks)
		{
			animData.frameHeight = anim.getInt("frameHeight", animData.frameHeight);
			animData.frameWidth = anim.getInt("frameWidth", animData.frameWidth);
			animData.frameRate = anim.getInt("frameRate", animData.frameRate);
			animData.horizontalFrameSpacing = anim.getInt("horizontalFrameSpacing", animData.horizontalFrameSpacing);
			animData.verticalFrameSpacing = anim.getInt("verticalFrameSpacing", animData.verticalFrameSpacing);
			animData.numFrames = anim.getInt("numFrames", animData.numFrames);
			animData.startX = anim.getInt("startX", animData.startX);
			animData.startY = anim.getInt("startY", animData.startY);
			animData.progression = Progression(anim.get("progression"), animData.progression);
			animData.autoPlay = false;

			// If play specifier is present flip bool to true
			if (anim.has("play"))
			{
				animData.autoPlay = true;
				defaultAnimName = anim.name;
			}

			addAnimation(anim.name, animData);
		}
	}

	void AnimatedTextureComponent::initializeFromFile(const std::string& filePath)
	{
		auto found = animCache.find(filePath);
		if (found.second)
		{
			loadAnimationFromCache(filePath, found.first);
		}
		else
		{
			parseAnimationsAndCache(filePath);
		}
	}

	AnimatedTextureComponent::AnimatedTextureComponent(Object* obj, const ComponentInitializationData& data) :
		DrawableComponent(obj, data), currentFrame(0), currentAnim(nullptr), timeForNextFrame(0.0f),
		nextFrameTimer(0.0f), playing(false)
	{
		m_useSrcRect = true;
		m_srcrect.x = 0;
		m_srcrect.y = 0;
		m_srcrect.w = 0;
		m_srcrect.h = 0;

		// Analyze file extension to check if it's an *.sganim file which we can parse
		size_t extensionbegin = data.path.find_last_of('.') + 1;
		std::string extension(data.path.begin() + extensionbegin, data.path.end());

		if (extension == "sganim")
		{
			initializeFromFile(Resources::pathTo(data.path));
		}
	}

	AnimatedTextureComponent& AnimatedTextureComponent::addAnimation(const std::string& name, const Animation& anim)
	{
		animations->get().insert(std::make_pair(name, anim));

		if (anim.autoPlay)
		{
			playAnimation(name);
		}

		return *this;
	}

	bool AnimatedTextureComponent::playAnimation(const std::string& name, float duration, Flip flip)
	{		
		auto kvp = animations->get().find(name);

		if (kvp == animations->get().end())
		{
			return false;
		}

		currentAnim = &(kvp->second);

		if (duration >= 0.f)
		{
			durationTimer = duration;
			timedAnimation = true;
		}
		else
		{
			timedAnimation = false;
		}

		m_srcrect.w = currentAnim->frameWidth;
		m_srcrect.h = currentAnim->frameHeight;
		resetAnimation();

		// Using the framerate, calculate how much time needs to elapse to
		// skip to next frame
		timeForNextFrame = 1.0f / (float)currentAnim->frameRate;
		playing = true;
		
		switch (flip)
		{
		case Flip::None:
			m_flip = SDL_FLIP_NONE;
			break;
		case Flip::Horizontal:
			m_flip = SDL_FLIP_HORIZONTAL;
			break;
		case Flip::Vertical:
			m_flip = SDL_FLIP_VERTICAL;
			break;
		default: break;
		}

		return true;
	}

	void AnimatedTextureComponent::resetAnimation()
	{
		currentFrame = 0;
		m_srcrect.x = currentAnim->startX;
		m_srcrect.y = currentAnim->startY;
	}

	void AnimatedTextureComponent::nextFrame()
	{
		if (++currentFrame >= currentAnim->numFrames)
		{
			resetAnimation();
		}
		else
		{
			// Offset source rectangle's location based on progression 
			switch (currentAnim->progression)
			{
			case Progression::LeftToRight:
				m_srcrect.x += currentAnim->frameWidth + currentAnim->horizontalFrameSpacing;
				break;
			case Progression::RightToLeft:
				m_srcrect.x -= currentAnim->frameWidth + currentAnim->horizontalFrameSpacing;
				break;
			case Progression::TopToBottom:
				m_srcrect.y += currentAnim->frameHeight + currentAnim->verticalFrameSpacing;
				break;
			case Progression::BottomToTop:
				m_srcrect.y -= currentAnim->frameHeight + currentAnim->verticalFrameSpacing;
				break;
			default:
				break;
			}
		}
	}

	void AnimatedTextureComponent::update(float deltaSeconds)
	{
		// Make sure we are playing an animation (and that currentAnim is not null)
		// Avoid updating if the anim has one frame or less
		if (playing && currentAnim)
		{
			if (currentAnim->numFrames > 1)
			{
				nextFrameTimer += deltaSeconds;

				// switch to next frame and reset timer
				if (nextFrameTimer >= timeForNextFrame)
				{
					nextFrame();
					nextFrameTimer = 0.0f;
				}
			}

			if (timedAnimation)
			{
				durationTimer -= deltaSeconds;

				// check if animation should stop
				if (durationTimer < 0.f)
				{
					playAnimation(defaultAnimName, -1.f, Flip::UseLast);
				}
			}
		}
	}
}