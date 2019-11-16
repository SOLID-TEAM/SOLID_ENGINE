#include "Globals.h"
#include "DynTree.h"
#include "Application.h"
#include "ModuleScene.h"


void DynTree::Init()
{
	unused_nodes = new DynTreeNode*[MAX_DYNTREE_NODES];
	created_nodes = new DynTreeNode*[MAX_DYNTREE_NODES];

	for (uint i = 0u; i < MAX_DYNTREE_NODES; ++i)
	{
		created_nodes[i] = unused_nodes[i] = new DynTreeNode();
	}

	root = GetUnusedNode(nullptr);
}

void DynTree::CleanUp()
{
	if (created_nodes != nullptr)
	{
		for (uint i = 0u; i < MAX_DYNTREE_NODES; ++i)
			RELEASE(created_nodes[i]);
	}

	RELEASE_ARRAY(unused_nodes);
	RELEASE_ARRAY(created_nodes);
	unused_nodes_count = MAX_DYNTREE_NODES - 1;
}


void DynTree::Reset()
{
	CleanUp();
	Init();
}

void DynTree::InsertGO(GameObject* go)
{	
	assert(go != nullptr); //tried to insert a null GameObject in the AABBTree

	std::stack<DynTreeNode*> nodes_stack;
	
	//tree root behaves different it could not be binary

	if ( (root->node_left == nullptr && root->node_right != nullptr) || (root->node_left == nullptr && root->node_right == nullptr) )
	{
		MakeLeaf(root->node_left, root, go, true);
	}
	else if (root->node_left != nullptr && root->node_right == nullptr)
	{
		MakeLeaf(root->node_right, root, go, false);
	}
	else
	{
		AABB tempLeft = AABB(root->node_left->aabb);
		tempLeft.Enclose(go->bounding_box);
		AABB tempRight = AABB(root->node_right->aabb);
		tempRight.Enclose(go->bounding_box);
		if (tempLeft.Size().Length() < tempRight.Size().Length()) //heuristic to keep the tree as spatial balanced as possible
			nodes_stack.push(root->node_left);
		else
			nodes_stack.push(root->node_right);
	}

	//Binary trees always if enters the while

	while (!nodes_stack.empty())
	{
		DynTreeNode* node = nodes_stack.top();
		nodes_stack.pop();
		if (node->is_leaf)
		{
			DynTreeNode* newNode = GetUnusedNode(node->parent); //new no leaf node
			newNode->is_left = node->is_left;
			if (node->is_left)
				node->parent->node_left = newNode;
			else
				node->parent->node_right = newNode;


			newNode->node_right = node; //put the old leaf on the right & create a new leaf on the left with the new GO
			newNode->node_right->is_left = false;
			node->parent = newNode;
			MakeLeaf(newNode->node_left, newNode, go, true);
			Recalculate(newNode);
		}
		else
		{
			AABB tempLeft = AABB(node->node_left->aabb);
			tempLeft.Enclose(go->bounding_box);
			AABB tempRight = AABB(node->node_right->aabb);
			tempRight.Enclose(go->bounding_box);
			if (tempLeft.Size().Length() < tempRight.Size().Length()) //heuristic to keep the tree as balanced as possible
				nodes_stack.push(node->node_left);
			else
				nodes_stack.push(node->node_right);
		}
	}
}

void DynTree::Recalculate(DynTreeNode* root)
{
	std::stack<DynTreeNode*> nodes_stack;

	nodes_stack.push(root);

	while (!nodes_stack.empty())
	{
		DynTreeNode* node = nodes_stack.top();
		nodes_stack.pop();

		if (node->is_leaf && node != root)
		{
			nodes_stack.push(node->parent);
		}
		else
		{
			node->aabb.SetNegativeInfinity();
			node->aabb.Enclose(node->node_left->aabb);
			node->aabb.Enclose(node->node_right->aabb);

			if (node->parent != nullptr)
				nodes_stack.push(node->parent);
		}
	}

	root->aabb.SetNegativeInfinity();
	root->aabb.Enclose(root->node_left->aabb);
	root->aabb.Enclose(root->node_right->aabb);
}

void DynTree::ReleaseNode(DynTreeNode* node)
{
	assert(unused_nodes_count < MAX_DYNTREE_NODES); 
	
	node->go->dyn_node = nullptr;

	if (node->parent == root) 
	{
		node->go->dyn_node = nullptr;

		if (node->is_left)
			node->parent->node_left = nullptr;
		else
			node->parent->node_right = nullptr;	
		unused_nodes[++unused_nodes_count] = node;
		return;
	}
	assert(node->parent->node_left != nullptr && node->parent->node_right != nullptr); //not binary!

	DynTreeNode* brother;
	DynTreeNode* siblingRoot = node->parent;

	if (node->is_left)
	{
		brother = siblingRoot->node_right;
	}
	else
	{
		brother = siblingRoot->node_left;
	}
	
	brother->is_left = siblingRoot->is_left;
	brother->parent = siblingRoot->parent;

	if (brother->is_left)
		brother->parent->node_left = brother;
	else
		brother->parent->node_right = brother;

	if (unused_nodes)
	{
		unused_nodes[++unused_nodes_count] = siblingRoot;
		unused_nodes[++unused_nodes_count] = node;
	}
	
}
void DynTree::ResetReleaseNode(DynTreeNode* node)
{
	assert(unused_nodes_count < MAX_DYNTREE_NODES);
	assert(node != nullptr);

	unused_nodes[++unused_nodes_count] = node;

}
inline DynTreeNode* DynTree::GetUnusedNode(DynTreeNode* parent)
{
	assert(unused_nodes_count > 0u); 

	unused_nodes[unused_nodes_count]->node_left = nullptr;
	unused_nodes[unused_nodes_count]->node_right = nullptr;
	unused_nodes[unused_nodes_count]->parent = parent;
	unused_nodes[unused_nodes_count]->is_leaf = false;
	unused_nodes[unused_nodes_count]->is_left = false;
	unused_nodes[unused_nodes_count]->go = nullptr;

	return unused_nodes[unused_nodes_count--];
}

inline void DynTree::MakeLeaf(DynTreeNode * node, DynTreeNode *parent, GameObject* go, bool is_left)
{
	node = GetUnusedNode(parent);
	node->aabb.SetFromCenterAndSize(go->bounding_box.CenterPoint(), go->bounding_box.Size() * FAT_FACTOR);
	node->go = go;
	go->dyn_node = node;
	node->is_leaf = true;
	node->is_left = is_left;
	if (is_left)
		parent->node_left = node;
	else
		parent->node_right = node;

}
