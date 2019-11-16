#ifndef _DYNAMIC_TREE_H__
#define _DYNAMIC_TREE_H__

#include "GameObject.h"

#include <vector>
#include <stack>
#include <unordered_set>

#include "external/MathGeoLib/include/Geometry/AABB.h"

#define MAX_DYNTREE_NODES 2048
#define FAT_FACTOR	1.1f

// http://box2d.org/files/GDC2019/ErinCatto_DynamicBVH_GDC2019.pdf

class DynTreeNode
{
public:

	// Hierarchy -----------------------------

	DynTreeNode* parent = nullptr;
	DynTreeNode* node_left = nullptr;
	DynTreeNode* node_right = nullptr;

	bool is_leaf = false;
	bool is_left = false;

	AABB aabb;
	GameObject* go = nullptr;
	
};

class DynTree
{
public:

	void Init();
	void CleanUp();
	void Reset();
	void InsertGO(GameObject* go);
	void ReleaseNode(DynTreeNode* node);
	void Draw() const;
	
	inline void ResetReleaseNode(DynTreeNode* node);
	void Recalculate(DynTreeNode* node);
	inline DynTreeNode* GetUnusedNode(DynTreeNode* parent);
	inline void MakeLeaf(DynTreeNode *node, DynTreeNode *parent, GameObject* go, bool is_left);
	template<typename T>
	void GetIntersections(T &intersector, std::unordered_set<GameObject*> &intersections);

	//members

	DynTreeNode* root = nullptr;
	DynTreeNode** unused_nodes = nullptr;
	DynTreeNode** created_nodes = nullptr; //all created nodes container to cleaning pourposes
	bool showOnEditor = false;
	unsigned unused_nodes_count = MAX_DYNTREE_NODES - 1;
};

template<typename T>
void DynTree::GetIntersections(T &intersector, std::unordered_set<GameObject*> &intersections) 
{
	BROFILER_CATEGORY("AABBTree intersections", Profiler::Color::Azure);
	std::stack<DynTreeNode*> S;
	S.push(root);
	while (!S.empty())
	{
		DynTreeNode* node = S.top();
		S.pop();		
		if (node->is_leaf && node->go->isActive() && node->aabb.ContainsQTree(intersector)) //check if is not outside
		{
			intersections.insert(node->go);
		}
		if (!node->is_leaf)
		{
			//keep the aabb as small as possible
			node->aabb.SetNegativeInfinity(); 
			if (node->node_left != nullptr)
				node->aabb.Enclose(node->node_left->aabb);
			if (node->node_right != nullptr)
				node->aabb.Enclose(node->node_right->aabb);

			if (node->node_left != nullptr && node->node_left->aabb.ContainsQTree(intersector))
			{
				S.push(node->node_left);
			}

			if (node->node_right != nullptr && node->node_right->aabb.ContainsQTree(intersector))
			{
				S.push(node->node_right);
			}

		}
	}
}
#endif //! _DYNAMIC_TREE_H__