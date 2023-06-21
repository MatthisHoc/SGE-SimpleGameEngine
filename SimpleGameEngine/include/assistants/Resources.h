#pragma once

#include <string>

namespace sg
{
	class Resources
	{
	public:
		static std::string pathTo(const std::string& relativePath);
		static std::string relativePath(const std::string& fullPath);
	private:
		Resources();
		Resources(const Resources&) = delete;
		Resources& operator=(const Resources&) = delete;
		Resources(Resources&&) = delete;
	};
}