#ifndef __RESOURCE_MODEL_H__
#define __RESOURCE_MODEL_H__

#include "Resource.h"
//#include "Math.h"

struct aiScene;
struct aiNode;
//struct aiMatrix4x4;

#include "external/Assimp/include/matrix4x4.h"

class R_Model : public Resource
{
public:

	struct Node
	{
		std::string			name;
		float4x4            transform = float4x4::identity;
		uint                parent = 0;
		UID                 mesh = 0;
		UID                 material = 0;
	};

public:

	R_Model(UID id);
	virtual ~R_Model();

	//void        Save(Config& config) const override;
	//void        Load(const Config& config) override;

	bool        LoadInMemory() override;
	void        ReleaseFromMem() override;
	void		LoadDependencies();

	bool        SaveToFile(UID uid);
	//bool		LoadFromFile(const char* file);
	//bool        Save(std::string& output) const;
	static bool Import(const char* full_path, UID& output, std::string from_path);

	/*unsigned    GetNumNodes() const { return nodes.size(); }
	const Node& GetNode(uint index) const { return nodes[index]; }*/

	uint GetNumNodes() const { return nodes.size(); }
	const Node& GetNode(uint idx) const { return nodes[idx]; }

private:

	void        GenerateNodes(const aiScene* model, const aiNode* node, uint parent, const std::vector<UID>& meshes, const std::vector<UID>& materials, aiMatrix4x4 accTransform);
	void        GenerateMaterials(const aiScene* scene, const char* file, std::vector<UID>& materials, std::string from_path);
	void        GenerateMeshes(const aiScene* scene, const char* file, std::vector<UID>& meshes);
	//void        SaveToStream(simple::mem_ostream<std::true_type>& write_stream) const;

	bool LoadNodesFromFile();

private:

	std::vector<Node> nodes;

};

#endif /* __RESOURCE_MODEL_H__ */
