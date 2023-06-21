#include "assistants/Resources.h"

namespace sg
{
	std::string Resources::pathTo(const std::string& relativePath)
	{
		// Debug configuration uses the folder located in the project directory
#ifdef _DEBUG
		return RESOURCES_PATH + relativePath;
#else
		return "resources\\" + relativePath;
#endif
	}

	std::string Resources::relativePath(const std::string& fullPath)
	{
		size_t pos = fullPath.find("resources\\");

		if (pos == std::string::npos) return fullPath;
		else
		{
			return fullPath.substr(pos + 11);
		}
	}
}