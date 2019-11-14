#ifndef _KDTREE_H__
#define _KDTREE_H__

#include "Globals.h"
#include "GameObject.h"

#include "external/MathGeoLib/include/Geometry/AABB.h"

#include <queue>
#include <vector>


#define BUCKET_MAX 1024 // TODO: Too much??? Test 

class KDTreeNode
{
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

	AABB* aabb = nullptr;
	uint depth = 0u;
	float middle = 0.0f;
	bool is_leaf = false;
	uint bucket_members = 0u;

	std::vector<GameObject*> bucket;

};

class KDTree
{
public:

	KDTree() {};

	~KDTree();

	void Fill(uint max_depth , uint max_node_bucket, AABB global_abb, std::vector<GameObject*> go_vec);

	void Clear();

	template<typename T>
	void GetIntersections(T &intersector, std::vector<GameObject*> &intersections) const;

private:

	void Create(uint max_depth, uint max_node_bucket);

public:

	KDTreeNode* root = nullptr;
	int max_depth = 6;
	int max_bucket_size = 5;
};

template<typename T>

void KDTree::GetIntersections(T &intersector, std::vector<GameObject*> &intersections) const
{
	std::queue<KDTreeNode*> nodes_queue;
	nodes_queue.push(root);

	while (!nodes_queue.empty())
	{
		KDTreeNode* node = nodes_queue.front();

		nodes_queue.pop();

		// Check intersection -----------------------------------------

		if (node->is_leaf == true ) 
		{
			if (node->bucket_members > 0u && node->aabb->Intersects(intersector))
			{
				for (GameObject* go : node->bucket)
				{
					intersections.push_back(go);
				}
			}
		}
		else
		{
			nodes_queue.push(node->left_child);
			nodes_queue.push(node->right_child);
		}
	}
}

#endif