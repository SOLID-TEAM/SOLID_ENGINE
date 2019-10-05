#include "Globals.h"
#include "Application.h"
#include "ModuleImporter.h"

#include "GL/glew.h"


ModuleImporter::ModuleImporter(bool start_enabled) : Module(start_enabled) {}

ModuleImporter::~ModuleImporter() {}

bool ModuleImporter::Init(Config& config)
{

	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);

	return true;
}

bool ModuleImporter::Start(Config& config)
{
	LoadFileMesh("Assets/suzanne.blend");
	LoadFileMesh("Assets/warrior.FBX");

	return true;
}

update_status ModuleImporter::Update(float dt)
{
	//std::vector<ModelData*>::iterator start_meshes = startup_meshes.begin();
	//for (; start_meshes != startup_meshes.end(); ++start_meshes)
	//{
	//	(*start_meshes)->GenerateBuffers(); // gen
	//	(*start_meshes)->UpdateBuffers(); // fill
	//}

	//if(startup_meshes.size() > 0)
	//	startup_meshes.clear();

	return UPDATE_CONTINUE;
}

update_status ModuleImporter::PostUpdate(float dt)
{
	std::vector<ModelData*>::iterator model = meshes.begin();

	for (; model != meshes.end(); ++model)
	{
		// TODO NEXT: implement this at level editor to show 2 possibilities:
		// 1: fill models with solid color (user selected plain color with imgui color picker) | on-off
		// 2: wireframe, lines color selectable by user too | on-off
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0f, 0.375f);

		glColor3f(1.0f, 0.0f, 1.0f);

		(*model)->Render();

		glDisable(GL_POLYGON_OFFSET_FILL);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		glColor3f(1.0f, 1.0f, 1.0f);

		(*model)->Render();
		
	}


	return UPDATE_CONTINUE;
}

bool ModuleImporter::CleanUp()
{
	/*std::vector<ModelData*>::iterator it = meshes.begin();

	uint count = 0;
	for (; it != meshes.end(); ++it)
	{
		delete meshes[count];
		++count;
	}*/

	aiDetachAllLogStreams();

	return true;
}

bool ModuleImporter::LoadFileMesh(const char* path)
{
	bool ret = true;

	const aiScene* scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);

	if (scene != nullptr && scene->HasMeshes())
	{
		for (uint i = 0; i < scene->mNumMeshes; ++i)
		{
			ModelData* m = new ModelData();
			//m->_v_size = scene->mMeshes[i]->mNumVertices;

			// TODO: divide this on separate functions

			aiMesh* assMesh = scene->mMeshes[i]; // :)
			
			m->_v_size = assMesh->mNumVertices;
			m->vertices = new float[m->_v_size * 3];
			memcpy(&m->vertices[i * 3], assMesh->mVertices, sizeof(float) * m->_v_size * 3);
			LOG("[Info] Created new mesh with %d vertices", m->_v_size);

			if (assMesh->HasFaces())
			{
				m->_idx_size = assMesh->mNumFaces * 3;
				m->indices = new uint[m->_idx_size];

				for (uint i = 0; i < assMesh->mNumFaces; ++i)
				{
					if (assMesh->mFaces[i].mNumIndices != 3)
					{
						LOG("[Error] Detected face with != 3 indices");
					}
					else
						memcpy(&m->indices[i * 3], assMesh->mFaces[i].mIndices, sizeof(uint) * 3);
				}
			}

			m->GenerateBuffers();
			m->UpdateBuffers();

			meshes.push_back(m);
			//startup_meshes.push_back(&m);
		}

		aiReleaseImport(scene);
	}
	else
		LOG("[Error] Loading scene %s", path);

	return ret;
}
