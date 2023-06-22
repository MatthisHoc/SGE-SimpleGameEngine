#pragma once

#include "components/DrawableComponent.h"
#include "components/Updatable.h"
#include "core/Cache.h"

#include <unordered_map>
#include <memory>
#include <map>
#include <string>

namespace sg
{
	class Progression
	{
	public:
		enum Value :uint8_t { LeftToRight, RightToLeft, TopToBottom, BottomToTop };
		
		constexpr Progression(Value progression = LeftToRight) : value(progression) {}
		Progression(const std::string& str, Progression def = Progression::LeftToRight)
		{
			if (str == "leftToRight") value = LeftToRight;
			else if (str == "rightToLeft") value = RightToLeft;
			else if (str == "topToBottom") value = TopToBottom;
			else if (str == "bottomToTop") value = BottomToTop;
			else value = def.value;
		}

		operator Value() const { return value; }
		explicit operator bool() const = delete;

		constexpr bool operator==(Progression p) const { return p.value == value; }
		constexpr bool operator!=(Progression p) const { return p.value != value; }

	private:
		Value value;
	};

	struct Animation
	{
		// How long this animation is in frames
		int numFrames = 0;
		// Number of pixels separating two frames horizontally
		int horizontalFrameSpacing = 0;
		// Number of pixels separating two frames vertically 
		int verticalFrameSpacing = 0;
		// Height of a frame in pixels
		int frameHeight = 0;
		// Width of a frame in pixels
		int frameWidth = 0;
		// X coordinates in pixels of the top left corner of the first frame of the animation
		int startX = 0;
		// Y coordinates in pixels of the top left corner of the first frame of the animation
		int startY = 0;
		// How fast should this animation play in Frames per seconds
		// This framerate defines the animation speed and doesn't have to match the application's framerate
		int frameRate = 30;
		// Which way the animation progresses
		Progression progression = Progression::LeftToRight;
		// Determines if the animation should be auto-played when a new component instanciates
		bool autoPlay = false;
	};

	enum class Flip { None, Horizontal, Vertical, UseLast };

	class AnimatedTextureComponent : public DrawableComponent, public Updatable
	{
	private:
		using AnimationCache = Cache<std::string, std::map<std::string, Animation>>;
		using AnimationCacheRef = CacheRef<std::string, std::map<std::string, Animation>>;
	public:
		AnimatedTextureComponent(Object* obj, const ComponentInitializationData& data);
		virtual ~AnimatedTextureComponent() override {};

		/* 
		Play the animation that has the corresponding name. 
		Set the duration to a negative value to play infinitly
		*/
		bool playAnimation(const std::string& name, float duration = -1.f, Flip flip = Flip::None);

		virtual void update(float deltaSeconds) override;

	private:

		// Prepare the component using a cache ref
		void loadAnimationFromCache(const std::string& filePath, const CacheRef<std::string, std::map<std::string, Animation>>& cacheRef);

		// Parse an animation file, caches the data and prepares the component with it
		void parseAnimationsAndCache(const std::string& filePath);

		AnimatedTextureComponent& addAnimation(const std::string& name, const Animation& anim);
		
		void nextFrame();

		// Resets current animation to it's first frame
		void resetAnimation();

		// calls initialize() and adds animations using file information
		void initializeFromFile(const std::string& filePath);

		int currentFrame;
		Animation* currentAnim = nullptr;
		float timeForNextFrame;
		float nextFrameTimer;
		bool playing;
		// Contains the name of the animation that has the "play" specifier
		// It is considered to be the default animation.
		std::string defaultAnimName;
		std::unique_ptr<AnimationCacheRef> animations = nullptr;

		bool timedAnimation = false;
		float durationTimer;

		static AnimationCache animCache;
		// Could be changed. 
		// It is there to prevent having to reparse the anim file to get the path to the texture
		static std::unordered_map<std::string, std::string> texturePaths;
	};
}