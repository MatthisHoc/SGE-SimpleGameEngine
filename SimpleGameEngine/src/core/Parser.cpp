#include "core/Parser.h"

#include <fstream>
#include <sstream>

namespace sg
{
	ParseException::ParseException(const char* message)
	{
		m_message += "[Parse Exception] ";
		m_message += message;
	}

	const char* ParseException::what() const noexcept
	{
		return m_message.c_str();
	}

	const Block& Block::findBlock(const std::string& blockName) const
	{
		for (const Block& subBlock : subBlocks)
		{
			if (subBlock.name == blockName) return subBlock;
		}

		return *this;
	}

	const std::vector<const Block*> Block::findBlocksNotOf(const std::vector<std::string>& exclusionList) const
	{
		std::vector<const Block*> returnValue;
		
		for (const Block& block : subBlocks)
		{
			bool inExclusionList = false;
			
			for (const std::string& name : exclusionList)
			{
				if (name == block.name)
				{
					inExclusionList = true;
					break;
				}
			}

			if (!inExclusionList)
			{
				returnValue.push_back(&block);
			}
		}

		return returnValue;
	}

	std::vector<Property>::const_iterator Block::lookForProperty(const std::string& propertyName) const
	{
		for (auto it = properties.begin(); it != properties.end(); ++it)
		{
			if ((*it).getName() == propertyName)
			{
				return it;
			}
		}

		return properties.end();
	}

	int Block::getInt(const std::string& propertyName, int def) const
	{
		auto location = lookForProperty(propertyName);
		if (location != properties.end())
		{
			return (*location).getValueInt();
		}
		
		return def;
	}

	float Block::getFloat(const std::string& propertyName, float def) const
	{
		auto location = lookForProperty(propertyName);
		if (location != properties.end())
		{
			return (*location).getValueFloat();
		}

		return def;
	}

	std::string Block::get(const std::string& propertyName, const std::string& def) const
	{
		auto location = lookForProperty(propertyName);
		if (location != properties.end())
		{
			return (*location).getValue();
		}

		return def;
	}

	vec2 Block::getVec2(const std::string& propertyName, const vec2& def) const
	{
		auto location = lookForProperty(propertyName);
		if (location != properties.end())
		{
			return (*location).getValueVec2();
		}

		return def;
	}

	bool Block::has(const std::string& specifierName) const
	{
		std::unordered_set<std::string>::iterator it = specifiers.find(specifierName);

		return it != specifiers.end();
	}

	vec2 Property::getValueVec2() const
	{
		std::string value = getValue();
		size_t separator = value.find_first_of(',');

		if (separator == value.npos)
		{
			return { 0.0f, 0.0f };
		}

		vec2 retVal;
		retVal.x = std::stof( Parser::removeTrailingWhitespaces(std::string(value.begin(), value.begin() + separator)) );
		retVal.y = std::stof( Parser::removeLeadingWhitespaces(std::string(value.begin() + separator + 1, value.end())) );

		return retVal;
	}

	std::string Parser::removeLeadingWhitespaces(const std::string& str)
	{
		size_t firstNonWhitespace = str.find_first_not_of(" \n\r\t");

		if (firstNonWhitespace == 0)
		{
			return str;
		}
		else if (firstNonWhitespace == std::string::npos)
		{
			return "";
		}
		
		return std::string(str.begin() + firstNonWhitespace, str.end());
	}

	std::string Parser::removeTrailingWhitespaces(const std::string& str)
	{
		size_t lastNonWhitespace = str.find_last_not_of(" \n\r\t");

		if (lastNonWhitespace == str.size())
		{
			return str;
		}
		else if (lastNonWhitespace == std::string::npos)
		{
			return "";
		}

		return std::string(str.begin(), str.begin() + lastNonWhitespace + 1);
	}

	std::string Parser::removeWhitespaces(const std::string& str)
	{
		return removeLeadingWhitespaces(removeTrailingWhitespaces(str));
	}

	bool Parser::isBlockBegin(std::string& outContent) const
	{
		size_t opening = line.find_first_of('<');

		if (opening != 0) return false;

		size_t closing = line.find_last_of('>');
		bool foundOpeningAndClosing = opening != std::string::npos && closing != std::string::npos;
		
		/*
			two cases where function returns false:
				- it's a closing bracket
				- closing character is placed before opening character
		*/
		if (line.substr(0, opening + 2) == "</" || closing < opening)
			return false;

		if (foundOpeningAndClosing)
		{
			if (opening + 1 == closing)
			{
				outContent = "";
			}
			else
			{
				outContent = line.substr(opening + 1, closing - opening - 1);
				outContent = removeWhitespaces(outContent);
			}

			return true;
		}
		else
		{
			outContent = "";
			return false;
		}
	}

	bool Parser::isBlockEnd() const
	{
		return line.substr(0, 2) == "</" && line.find_last_of('>') != std::string::npos;
	}

	bool Parser::isComment() const
	{
		return line.substr(0, 2) == "//";
	}

	bool Parser::lineIsEmpty() const
	{
		return line.find_last_not_of(" \n\r\t") == std::string::npos;
	}

	Property Parser::parseProperty(size_t separatorPosition)
	{
		std::string name = removeTrailingWhitespaces(std::string(line.begin(), line.begin() + separatorPosition));
		std::string value = removeLeadingWhitespaces(std::string(line.begin() + separatorPosition + 1, line.end()));

		return Property(name, value);
	}

	Parser::Parser(const std::string& path)
	{
		file.open(path);

		if (!file.is_open())
		{
			throw ParseException("Failed to open file. Make sure the path is valid.");
		}

		std::getline(file, line);
		while (!file.eof())
		{
			if (lineIsEmpty())
			{
				std::getline(file, line);
				continue;
			}

			std::string content;
			if (isBlockBegin(content))
			{
				blocks.push_back(createBlock(content));
			}

			std::getline(file, line);
		}

		file.close();
	}
	
	std::string Parser::getNameFromContent(const std::string& content) const
	{
		// return empty string
		if (content == "") return std::string();

		size_t separator = content.find_last_of('|');
		return removeTrailingWhitespaces(content.substr(0, separator));
	}

	std::unordered_set<std::string> Parser::getSpecifiersFromContent(const std::string& content) const
	{
		std::unordered_set<std::string> specifiers;

		// return specifiers as an empty set
		if (content == "") return specifiers;

		size_t separator = content.find_last_of('|');

		// The line does not contain any specifier
		if (separator == std::string::npos || content.size() <= separator + 1)
			return std::unordered_set<std::string>();

		std::string specifiersLine = content.substr(separator + 1);

		std::stringstream ss(specifiersLine);
		std::string specifier;

		while (std::getline(ss, specifier, ','))
		{
			specifiers.insert(removeWhitespaces(specifier));
		}

		return specifiers;
	}

	Block Parser::createBlock(const std::string& openerContent)
	{
		Block block;

		block.name = getNameFromContent(openerContent);
		block.specifiers = getSpecifiersFromContent(openerContent);

		// Parse properties
		while (std::getline(file, line))
		{
			if (lineIsEmpty()) continue;

			line = removeLeadingWhitespaces(line);
			
			if (isComment()) continue;

			// A new block opens, add a subblock
			std::string content;
			if (isBlockBegin(content))
			{
				block.subBlocks.push_back(createBlock(content));
				continue;
			}
			// Return the block by breaking out of the loop
			else if(isBlockEnd())
			{
				break;
			}

			// Lines inside a block are either properties or put in the "other" category
			size_t separator;
			if ((separator = line.find_first_of("=")) != std::string::npos)
			{
				block.properties.push_back(parseProperty(separator));
			}
			else
			{
				block.other.insert(line);
			}
		}

		return block;
	}

	Parser::~Parser()
	{

	}
}