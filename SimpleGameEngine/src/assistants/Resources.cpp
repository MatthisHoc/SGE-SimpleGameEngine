#include "assistants/Resources.h"

namespace sg
{
	std::string Resources::pathTo(const std::string& relativePath)
	{
		// Debug configuration uses the folder located in the project directory
#ifdef _DEBUG
		return RESOURCES_PATH + relativePath;
#elif defined(WIN32)
		return "resources\\" + relativePath;
#else
		return "resources/" + relativePath;
#endif
	}

	std::string Resources::relativePath(const std::string& fullPath)
	{
#ifdef WIN32
		size_t pos = fullPath.find("resources\\");
#else
		size_t pos = fullPath.find("resources/");
#endif
		if (pos == std::string::npos) return fullPath;
		else
		{
			return fullPath.substr(pos + 11);
		}
	}
}