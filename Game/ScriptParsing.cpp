#include "core/Object/ObjectData.h"

#include "components/AnimatedTextureComponent.h"
#include "components/BoxComponent.h"
#include "components/TextureComponent.h"
#include "components/TextComponent.h"

#include <functional>
#include <string>
#include <unordered_map>

#define SCRIPT_LAMBDA_SIGNATURE [](__attribute__((unused))Object* target) // <-- Technically target is not an unused variable but because we have an empty lambda for demonstration it will generate a compiler warning if the attribute isn't here

// This file defines the sg::Object::addScriptFromStr function.
// This function uses a static map that is dependant of the game that maps a script name to it's Script class name
// If you want to deserialize *.sgo files that use certain scripts, you must first fill this map otherwise the engine
// won't know which Script class matches which script name

namespace sg
{
	void Object::addScriptFromStr(const std::string& scriptName)
	{
		using StrLambdaMap = std::unordered_map<std::string, std::function<void(Object*)>>;
		static const StrLambdaMap associationMap =
		{
			{ "scriptName", SCRIPT_LAMBDA_SIGNATURE{ /*target->addScript<ScriptName>();*/ } },
		};

		StrLambdaMap::const_iterator kvp = associationMap.find(scriptName);
		if (kvp != associationMap.end())
		{
			kvp->second(this);
		}
	}
}