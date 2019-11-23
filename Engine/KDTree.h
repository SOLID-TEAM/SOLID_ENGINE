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

class ModuleRenderer3D;

class KDTreeNode
{
public:

	friend ModuleRenderer3D;

public:

	KDTreeNode();

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

	C_Camera& camera = (C_Camera&)intersector;
	LineSegment& ray = (LineSegment&)intersector;

	if (typeid(C_Camera) == typeid(T))
	{
		
		++checked_cols;
		if (camera.CheckCollisionAABB(root->aabb)) nodes_queue.push(root);
	}
	else if (typeid(LineSegment) == typeid(T))
	{
		++checked_cols;
		if (root->aabb.Intersects(ray)) nodes_queue.push(root);
	}
	else
	{
		LOG("[Error] KDtree GetIntersection(). Intersector type not mached")
			return;
	}
	
	while (!nodes_queue.empty())
	{
		KDTreeNode* node = nodes_queue.front();

		nodes_queue.pop();

		// Check filed lief nodes intersection -----------------------------------------

		if (node->bucket.size() > 0u)
		{
			bool ret = false;

			// Check collision with node AABB

			if (typeid(C_Camera) == typeid(T))
			{
				for (GameObject* go : node->bucket)
				{
					// Only push if bounding box collide 

					++checked_cols;

					if (camera.CheckCollisionAABB(go->bounding_box))
					{
						intersections.push_back(go);
					}
				}
			}
			else if (typeid(LineSegment) == typeid(T))
			{
				for (GameObject* go : node->bucket)
				{
					// Only push if bounding box collide 

					++checked_cols;

					if (go->bounding_box.Intersects(ray))
					{
						intersections.push_back(go);
					}
				}
			}
		}

		if (typeid(C_Camera) == typeid(T))
		{
			if (node->left_child != nullptr && node->left_child->bucket.size() > 0 &&camera.CheckCollisionAABB(node->left_child->aabb)) nodes_queue.push(node->left_child);
			if (node->right_child != nullptr && node->left_child->bucket.size() > 0  && camera.CheckCollisionAABB(node->right_child->aabb)) nodes_queue.push(node->right_child);
		}
		else if (typeid(LineSegment) == typeid(T))
		{
			if (node->left_child != nullptr && node->left_child->bucket.size() > 0  && node->left_child->aabb.Intersects(ray)) nodes_queue.push(node->left_child);
			if (node->right_child != nullptr && node->left_child->bucket.size() > 0  && node->right_child->aabb.Intersects(ray)) nodes_queue.push(node->right_child);
		}
	}

	checked_collisions = checked_cols;
}

#endif