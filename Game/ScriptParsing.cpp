#include "core/Object/ObjectData.h"

#include "components/AnimatedTextureComponent.h"
#include "components/BoxComponent.h"
#include "components/TextureComponent.h"
#include "components/TextComponent.h"

#include <functional>
#include <string>
#include <unordered_map>

#define SCRIPT_LAMBDA_SIGNATURE [](Object* target)

namespace sg
{
	void Object::addScriptFromStr(const std::string& scriptName)
	{
		using StrLambdaMap = std::unordered_map<std::string, std::function<void(Object*)>>;
		static const StrLambdaMap associationMap =
		{
			{ "scriptName", SCRIPT_LAMBDA_SIGNATURE{ /*addScript<ScriptName>();*/ } },
		};

		StrLambdaMap::const_iterator kvp = associationMap.find(scriptName);
		if (kvp != associationMap.end())
		{
			kvp->second(this);
		}
	}
}