#pragma once

#include <string>
#include <fstream>
#include <vector>
#include <unordered_set>
#include <tuple>

#include "core/vec2.h"

namespace sg
{
	class ParseException : public std::exception
	{
	public:
		ParseException(const char* message);
		virtual const char* what() const override;
	private:
		std::string m_message;
	};

	class Property
	{
	public:
		Property(const std::string& propertyName, const std::string& value)
			: propertyValuePair(propertyName, value) { }

		const std::string& getName() const { return std::get<0>(propertyValuePair); }
		const std::string& getValue() const { return std::get<1>(propertyValuePair); }
		int getValueInt() const { return std::stoi(std::get<1>(propertyValuePair)); }
		float getValueFloat() const { return std::stof(std::get<1>(propertyValuePair)); }
		vec2 getValueVec2() const;

	private:
		std::tuple<std::string, std::string> propertyValuePair;
	};

	class Block
	{
	public:
		std::string name;
		std::vector<Block> subBlocks;
		std::unordered_set<std::string> specifiers;
		std::vector<Property> properties;
		std::unordered_set<std::string> other;

		/*	Returns the block with the provided name if it's in the sub blocks
			Returns the parent block if it cannot be found
			Does not check recursively in children
		*/
		const Block& findBlock(const std::string& blockName) const;

		/* Returns a list of blocks that dont match the provided names.
		   Returns an empty vector if there is no match
		   Does not check recursively in children
		*/
		const std::vector<const Block*> findBlocksNotOf(const std::vector<std::string>& exclusionList) const;

		// Returns the value associated with the property name or def if it cannot be found
		std::string get(const std::string& propertyName, const std::string& def = "") const;
		// Returns the value associated with the property name or def if it cannot be found
		int getInt(const std::string& propertyName, int def = 0) const;
		// Returns the value associated with the property name or def if it cannot be found
		float getFloat(const std::string& propertyName, float def = 0.0f) const;
		// Returns the value associated with the property name or def if it cannot be found
		vec2 getVec2(const std::string& propertyName, const vec2& def = { 0.0f, 0.0f }) const;
		// Returns true if the provided specifier is contained in the block or not
		bool has(const std::string& specifierName) const;
	private:
		std::vector<Property>::const_iterator lookForProperty(const std::string& propertyName) const;
	};

	class Parser
	{
	public:
		Parser(const std::string& path);
		~Parser();

		// Returns main blocks, for each block use the subBlocks array to access it's children
		const std::vector<Block>& getBlocks() const { return blocks; }
		// Returns the name of the first block in the file
		std::string getMainBlockName() const { return blocks[0].name; }
		// Returns the first block in the file
		const Block& getMainBlock() const { return blocks[0]; }

		static std::string removeLeadingWhitespaces(const std::string& str);
		static std::string removeTrailingWhitespaces(const std::string& str);
		static std::string removeWhitespaces(const std::string& str);
	private:
		Block createBlock(const std::string& openerContent);
		Property parseProperty(size_t separatorPosition);
		bool isBlockBegin(std::string& outContent) const;
		bool isBlockEnd() const;
		bool lineIsEmpty() const;
		bool isComment() const;
		std::string getNameFromContent(const std::string& content) const;
		std::unordered_set<std::string> getSpecifiersFromContent(const std::string& content) const;

		std::vector<Block> blocks;
		std::ifstream file;
		std::string line;
	};
}
