#include "KDTree.h"
#include "GameObject.h"
#include "C_Transform.h"

KDTreeNode::~KDTreeNode()
{
	RELEASE(aabb);
	RELEASE(left_child);
	RELEASE(right_child);
}

void KDTreeNode::SplitNode(unsigned dimension, float middle, KDTreeNode* left, KDTreeNode* right) const
{
	float3 center_left = aabb->CenterPoint();
	float3 center_right = aabb->CenterPoint();
	float3 size_left, size_right;
	size_left = size_right = aabb->Size();

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

	left->aabb->SetFromCenterAndSize(center_left, size_left);
	right->aabb->SetFromCenterAndSize(center_right, size_right);
}


KDTree::~KDTree()
{
	RELEASE(root);
}
void KDTree::Create(uint max_depth, uint max_node_bucket)
{
	std::queue<KDTreeNode*> nodes_queue; 
	root = new KDTreeNode();
	root->aabb = new AABB();

	this->max_depth = max_depth;
	this->max_bucket_size = max_node_bucket;

	nodes_queue.push(root);

	while (!nodes_queue.empty())
	{
		KDTreeNode* current = nodes_queue.front(); nodes_queue.pop();
		current->bucket.resize(BUCKET_MAX);
		if (current->depth <= max_depth)
		{
			current->left_child = new KDTreeNode();
			current->right_child = new KDTreeNode();
			current->left_child->aabb = new AABB();
			current->right_child->aabb = new AABB();
			current->left_child->depth = current->depth + 1;
			current->right_child->depth = current->depth + 1;
			nodes_queue.push(current->left_child);
			nodes_queue.push(current->right_child);
		}
		else
			current->is_leaf = true;
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

void KDTree::Fill(uint max_depth, uint max_node_bucket, AABB global_aabb , std::vector<GameObject*> go_vec)
{
	Clear();
	Create(max_depth, max_node_bucket);

	root->is_leaf = false;
	*root->aabb = global_aabb;
	root->bucket = go_vec;
	root->bucket_members = go_vec.size();

	std::queue<KDTreeNode*> nodes_queue;
	nodes_queue.push(root);

	while (!nodes_queue.empty())
	{
		KDTreeNode* current = nodes_queue.front(); 
		
		nodes_queue.pop();

		if (current->bucket_members > max_bucket_size && current->depth <= max_depth)
		{
			// Choose dimension --------------------------------

			uint dimension = current->depth % 3;

			// Sort per position dimention ---------------------

			std::sort(current->bucket.begin(), current->bucket.begin() + current->bucket_members, [dimension](const GameObject* go1, const GameObject *go2)
			{
				if (go1 == nullptr || go2 == nullptr)
				{
					return false;
				}

				return go1->transform->position[dimension] > go2->transform->position[dimension];
			});

			// Calculate middle -------------------------------

			if (current->bucket_members % 2 == 0)
			{
				uint middle = (current->bucket_members * 0.5f); 
				current->middle = (current->bucket[middle - 1]->transform->position[dimension] + current->bucket[middle]->transform->position[dimension]) * 0.5f;
			}
			else
			{
				uint middle = (current->bucket_members * 0.5f);
				current->middle = current->bucket[middle]->transform->position[dimension];
			}

			// Split node in right and left -------------------

			current->SplitNode(dimension, current->middle, current->left_child, current->right_child);
			nodes_queue.push(current->left_child);
			nodes_queue.push(current->right_child);

			for (unsigned i = 0; i < current->bucket_members; ++i) 
			{

				if (current->left_child->aabb->Intersects(current->bucket[i]->bounding_box))
				{
					current->left_child->bucket[current->left_child->bucket_members++] = current->bucket[i];
				}
				if (current->right_child->aabb->Intersects(current->bucket[i]->bounding_box))
				{
					current->right_child->bucket[current->right_child->bucket_members++] = current->bucket[i];
				}
			}
			current->is_leaf = false;
		}
		else
		{
			current->is_leaf = true;
		}
	}
}

