#include "KDTree.h"
#include "GameObject.h"
#include "C_Transform.h"
#include "ModuleRenderer3D.h"

KDTreeNode::KDTreeNode()
{
	aabb.SetNegativeInfinity();
}

KDTreeNode::~KDTreeNode()
{
	RELEASE(left_child);
	RELEASE(right_child);
}

void KDTreeNode::SplitNode(unsigned dimension, float middle, KDTreeNode* left, KDTreeNode* right) const
{
	float3 center_left = aabb.CenterPoint();
	float3 center_right = aabb.CenterPoint();
	float3 size_left, size_right;
	size_left = size_right = aabb.Size();

	switch (dimension)
	{
		case 0:
		{
			float x_left = center_left.x - size_left.x * 0.5f;
			size_left.x = middle - x_left;
			center_left.x = x_left + (size_left.x) * 0.5f;

			float x_right = center_right.x + size_right.x * 0.5f;
			size_right.x = x_right - middle;
			center_right.x = x_right - (size_right.x) * 0.5f;

			break;
		}
		case 1:
		{
			float y_left = center_left.y - size_left.y * 0.5f;
			size_left.y = middle - y_left;
			center_left.y = y_left + (size_left.y) * 0.5f;

			float y_right = center_right.y + size_right.y * 0.5f;
			size_right.y = y_right - middle;
			center_right.y = y_right - (size_right.y) * 0.5f;

			break;
		}
		case 2:		
		{
			float z_left = center_left.z - size_left.z * 0.5f;
			size_left.z = middle - z_left;
			center_left.z = z_left + (size_left.z) * 0.5f;

			float z_right = center_right.z + size_right.z * 0.5f;
			size_right.z = z_right - middle;
			center_right.z = z_right - (size_right.z) * 0.5f;

			break;
		}
	}

	left->aabb.SetFromCenterAndSize(center_left, size_left);
	right->aabb.SetFromCenterAndSize(center_right, size_right);
}


KDTree::~KDTree()
{
	RELEASE(root);
}

void KDTree::Create(uint max_depth, uint max_node_bucket)
{
	std::queue<KDTreeNode*> nodes_queue; 
	root = new KDTreeNode();
	root->aabb.SetNegativeInfinity();

	this->max_depth = max_depth;
	this->max_bucket_size = max_node_bucket;

	nodes_queue.push(root);

	while (!nodes_queue.empty())
	{
		KDTreeNode* current = nodes_queue.front(); 
		nodes_queue.pop();

		if (current->depth <= max_depth)
		{
			current->left_child = new KDTreeNode();
			current->right_child = new KDTreeNode();
			current->left_child->depth = current->depth + 1;
			current->right_child->depth = current->depth + 1;
			nodes_queue.push(current->left_child);
			nodes_queue.push(current->right_child);
		}

	}
}

void KDTree::Clear()
{
	if (root != nullptr)
	{
		RELEASE(root);
		root = nullptr;
	}
}

bool KDTree::Active()
{
	return (root == nullptr) ? false : true;
}

void KDTree::Fill(uint max_depth, uint max_node_bucket, AABB global_aabb , std::vector<GameObject*> go_vec)
{
	Clear();

	if (go_vec.size() == 0)
	{
		return;
	}

	Create(max_depth, max_node_bucket);

	//root->is_leaf = false;
	root->aabb = global_aabb;
	root->bucket = go_vec;

	std::queue<KDTreeNode*> nodes_queue;
	nodes_queue.push(root);

	while (!nodes_queue.empty())
	{
		KDTreeNode* node = nodes_queue.front(); 
		
		nodes_queue.pop();

		if (node->bucket.size() > max_bucket_size && node->depth <= max_depth)
		{
			// Choose dimension --------------------------------

			uint dimension = node->depth % 3;

			// Sort per position dimention ---------------------

			std::sort(node->bucket.begin(), node->bucket.end() , [dimension](const GameObject* go1, const GameObject *go2)
			{
				if (go1 == nullptr || go2 == nullptr)
				{
					return false;
				}

				return go1->transform->position[dimension] > go2->transform->position[dimension];
			});

			// Calculate middle -------------------------------

			if (node->bucket.size() % 2 == 0)
			{
				uint middle = (node->bucket.size() * 0.5f);
				node->middle = (node->bucket[middle - 1]->transform->position[dimension] + node->bucket[middle]->transform->position[dimension]) * 0.5f;
			}
			else
			{
				uint middle = (node->bucket.size() * 0.5f);
				node->middle = node->bucket[middle]->transform->position[dimension];
			}

			// Split node in right and left -------------------

			node->SplitNode(dimension, node->middle, node->left_child, node->right_child);
			nodes_queue.push(node->left_child);
			nodes_queue.push(node->right_child);

			for (GameObject* go : node->bucket)
			{
				bool intersect_left = false;
				bool intersect_right = false;

				if (node->left_child->aabb.Intersects(go->bounding_box))
				{
					intersect_left = true;
					
				}
				if (node->right_child->aabb.Intersects(go->bounding_box))
				{
					intersect_right = true;
					
				}

				if (intersect_left && intersect_right)
				{
					continue;
				}
				else if (intersect_left)
				{
					node->left_child->bucket.push_back(go);
				}
				else
				{
					node->right_child->bucket.push_back(go);
				}
			}
		
		}

	}
}
