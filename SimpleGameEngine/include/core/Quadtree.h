#pragma once

#include <vector>
#include <SDL_video.h>
#include <stdexcept>
#include <memory>

#include "core/vec2.h"

#define TOP_LEFT 0xf000
#define TOP_RIGHT 0x0f00
#define BOTTOM_RIGHT 0x00f0
#define BOTTOM_LEFT 0x000f

namespace sg
{
	class Object;
	class Quadtree
	{
	public:
		Quadtree(unsigned int maxDepth = 4, unsigned int maxObjectsPerQuarter = 2);
		~Quadtree();

		// For each node calculate a rectangle that matches it
		std::vector<SDL_Rect> computeDrawData() const;

		// Insert object in quadtree, subdividing the tree if necessary
		void insert(const Object* obj);

		// Remove object from quadtree, deleting subdivisions if necessary
		void remove(const Object* obj);

	private:
		Quadtree(const Quadtree&) = delete;
		Quadtree(Quadtree&&) = delete;
		Quadtree& operator=(const Quadtree&) = delete;

		struct Node
		{
			std::unique_ptr<Node[]> children = nullptr;
			std::vector<const Object*> content;
			vec2 middle = { 0.f, 0.f };
			vec2 size = { 0.f, 0.f };
		};

		unsigned short m_depth;
		const unsigned int m_maxDepth;
		const unsigned int m_maxObjectsPerQuarter;
		std::unique_ptr<Node> root = nullptr;

		// find quadrant given object position and node dimensions
		int findQuandrant(const Node* node, const Object* obj);
		// insert object in correct children
		void insertInChild(int quadrants, Node* node, const Object* obj);
		// create 4 children and sizes them correctly given parent's size/middle
		void createChildren(Node* node);

		// called by the wrapper insert(), performs the actual insertion
		void processInsertion(Node* node, const Object* obj);

		// called by the wrapper remove(), performs the actual removal
		int processRemoval(Node* node, const Object* obj);

		// Insert data in tiles and iterate through children recursively
		void fillTiles(std::vector<SDL_Rect>& tiles, const Node* node) const;
	};
}