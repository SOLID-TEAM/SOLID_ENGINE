#ifndef _KDTREE_H__
#define _KDTREE_H__

#include "Globals.h"
#include "GameObject.h"

#include "external/MathGeoLib/include/Geometry/AABB.h"
#include "external/MathGeoLib/include/Geometry/LineSegment.h"
#include "C_Camera.h"

#include <queue>
#include <vector>
#include <typeinfo>


#define BUCKET_MAX 1024 // TODO: Too much??? Test 

class ModuleRenderer3D;

class KDTreeNode
{
public:

	friend ModuleRenderer3D;

public:

	KDTreeNode() {};

	~KDTreeNode();

	void SplitNode(unsigned dimension, float middle, KDTreeNode* left, KDTreeNode* right) const;

public:

	// Hierarchy --------------------------------

	KDTreeNode* parent_node = nullptr;
	KDTreeNode* left_child = nullptr;
	KDTreeNode* right_child = nullptr;

	// Vars -------------------------------------

	AABB aabb;
	uint depth = 0u;
	float middle = 0.0f;
	bool is_leaf = false;
	uint bucket_members = 0u;
	uint dimension = 0u;

	std::vector<GameObject*> bucket;
};

class KDTree
{
public:

	friend ModuleRenderer3D;

public:

	~KDTree();

	void Fill(uint max_depth , uint max_node_bucket, AABB global_abb, std::vector<GameObject*> go_vec);

	void Clear();

	bool Active();

	template<typename T>
	void GetIntersections(T &intersector, std::vector<GameObject*> &intersections, uint& checked_collisions);

private:

	void Create(uint max_depth, uint max_node_bucket);

private:

	KDTreeNode* root = nullptr;
	int max_depth = 6;
	int max_bucket_size = 5;
};

template<typename T>

void KDTree::GetIntersections(T &intersector, std::vector<GameObject*> &intersections, uint& checked_collisions)
{
	std::queue<KDTreeNode*> nodes_queue;

	uint checked_cols = checked_collisions;

	if (root == nullptr)
	{
		return;
	}

	nodes_queue.push(root);

	while (!nodes_queue.empty())
	{
		KDTreeNode* node = nodes_queue.front();

		nodes_queue.pop();

		// Check filed lief nodes intersection -----------------------------------------

		if (node->is_leaf == true && node->bucket_members > 0u)
		{
			bool ret = false;

			// Check collision with node AABB

			if (typeid(C_Camera) == typeid(T))
			{
				C_Camera& camera = (C_Camera&)intersector;

				++checked_cols;

				if (camera.CheckCollisionAABB(node->aabb))
				{
					for (uint i = 0; i < node->bucket_members; ++i)
					{
						// Only push if bounding box collide 

						++checked_cols;

						if (camera.CheckCollisionAABB(node->bucket[i]->bounding_box))
						{
							intersections.push_back(node->bucket[i]);
						}
					}
				}
			}
			else if (typeid(LineSegment) == typeid(T))
			{
				LineSegment& ray = (LineSegment&)intersector;

				++checked_cols;

				if (node->aabb.Intersects(ray))
				{
					for (uint i = 0; i < node->bucket_members; ++i)
					{
						// Only push if bounding box collide 

						++checked_cols;

						if (node->bucket[i]->bounding_box.Intersects(ray))
						{
							intersections.push_back(node->bucket[i]);
						}
					}
				}
			}
			else 
			{
				LOG("[Error] KDtree GetIntersection(). Intersector type not mached")
				return;
			}
		}
		else
		{
			if (node->left_child != nullptr) nodes_queue.push(node->left_child);
			if (node->right_child != nullptr) nodes_queue.push(node->right_child);
		}

		// Equal checked collisions 

		checked_collisions = checked_cols;

	}
}

#endif