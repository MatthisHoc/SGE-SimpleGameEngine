#include "core/Quadtree.h"
#include "core/Game.h"
#include "core/Object/Object.h"

namespace sg
{
	Quadtree::Quadtree(unsigned int maxDepth, unsigned int maxObjectsPerQuarter) :
		m_depth(0),
		m_maxDepth(maxDepth),
		m_maxObjectsPerQuarter(maxObjectsPerQuarter)
	{
		root = std::make_unique<Node>(Node());
		root->size.x = 1000.f;
		root->size.y = 1000.f;
	}

	Quadtree::~Quadtree()
	{
		//TODO: Cleanup
	}

	void Quadtree::fillTiles(std::vector<SDL_Rect>& tiles, const Node* node) const
	{

		SDL_Rect rect;
		rect.x = (int)(node->middle.x - node->size.x / 2.f);
		rect.y = (int)(node->middle.y - node->size.y / 2.f);
		rect.w = (int)(node->size.x);
		rect.h = (int)(node->size.y);
		tiles.push_back(rect);

		if (node->children != nullptr)
		{
			for (int i = 0; i < 4; ++i)
			{
				fillTiles(tiles, &(node->children[i]));
			}
		}
	}

	std::vector<SDL_Rect> Quadtree::computeDrawData() const
	{
		std::vector<SDL_Rect> tiles;
		fillTiles(tiles, root.get());
		return tiles;
	}

	int Quadtree::findQuandrant(const Node* node, const Object* obj)
	{
		int result = 0;
		if (obj->getPosition().x <= node->middle.x)
		{
			if (obj->getPosition().y <= node->middle.y)
			{
				result |= TOP_LEFT;
			}

			if(obj->getPosition().y >= node->middle.y)
			{
				result |= BOTTOM_LEFT;
			}
		}
		
		if (obj->getPosition().x >= node->middle.x)
		{
			if (obj->getPosition().y <= node->middle.y)
			{
				result |= TOP_RIGHT;
			}

			if (obj->getPosition().y >= node->middle.y)
			{
				result |= BOTTOM_RIGHT;
			}
		}

		return result;
	}

	void Quadtree::insertInChild(int quadrants, Node* node, const Object* obj)
	{
		if (quadrants & TOP_LEFT)
		{
			processInsertion(&(node->children[0]), obj);
		}
		if (quadrants & TOP_RIGHT)
		{
			processInsertion(&(node->children[1]), obj);
		}
		if (quadrants & BOTTOM_RIGHT)
		{
			processInsertion(&(node->children[2]), obj);
		}
		if (quadrants & BOTTOM_LEFT)
		{
			processInsertion(&(node->children[3]), obj);
		}
	}

	void Quadtree::createChildren(Node* node)
	{
		node->children = std::make_unique<Node[]>(4);

		float newSize = node->size.x / 2.f;

		for (int i = 0; i < 4; ++i)
		{
			node->children[i].size.x = newSize;
			node->children[i].size.y = newSize;
		}

		float newSizeDividedByTwo = newSize / 2.f;
		node->children[0].middle.x = node->middle.x - newSizeDividedByTwo;
		node->children[0].middle.y = node->middle.y - newSizeDividedByTwo;

		node->children[1].middle.x = node->middle.x + newSizeDividedByTwo;
		node->children[1].middle.y = node->middle.y - newSizeDividedByTwo;

		node->children[2].middle.x = node->middle.x + newSizeDividedByTwo;
		node->children[2].middle.y = node->middle.y + newSizeDividedByTwo;

		node->children[3].middle.x = node->middle.x - newSizeDividedByTwo;
		node->children[3].middle.y = node->middle.y + newSizeDividedByTwo;
	}

	void Quadtree::insert(const Object* obj)
	{
		processInsertion(root.get(), obj);
	}

	void Quadtree::processInsertion(Node* node, const Object* obj)
	{
		// When node has children, traverse tree
		if (node->children != nullptr)
		{
			insertInChild(findQuandrant(node, obj), node, obj);
		}
		// Divide node
		else if (node->content.size() == m_maxObjectsPerQuarter && m_depth < m_maxDepth)
		{
			++m_depth;

			createChildren(node);

			// Remove objects from current node and insert in child nodes
			for (const Object* object : node->content)
			{
				int quadrants = findQuandrant(node, object);
				insertInChild(quadrants, node, object);
			}

			node->content.clear();

			// Insert new object
			insertInChild(findQuandrant(node, obj), node, obj);
		}
		// Insert object in node directly
		else
		{
			node->content.push_back(obj);
		}
	}

	void Quadtree::remove(const Object* obj)
	{
		processRemoval(root.get(), obj);
	}

	int Quadtree::processRemoval(Node* node, const Object* obj)
	{
		if (node->children != nullptr)
		{
			unsigned int numElements = 0;
			for (int i = 0; i < 4; ++i)
			{
				numElements += processRemoval(&(node->children[i]), obj);
			}

			// Remove children, merge their content in parent node
			if (numElements <= m_maxObjectsPerQuarter)
			{
				for (int i = 0; i < 4; ++i)
				{
					std::vector<const Object*>& childContent = node->children[i].content;
					for (auto it = childContent.begin(); it < childContent.end(); ++it)
					{
						node->content.push_back(*it);
					}
					childContent.clear();
				}

				node->children = nullptr;
			}

			return numElements;
		}
		else
		{
			unsigned int numElements = static_cast<unsigned int>(node->content.size());
			for (auto it = node->content.begin(); it < node->content.end(); ++it)
			{
				if (*it == obj)
				{
					node->content.erase(it);

					// an element has been removed
					return numElements - 1;
				}
			}

			return numElements;
		}
		
	}
}
