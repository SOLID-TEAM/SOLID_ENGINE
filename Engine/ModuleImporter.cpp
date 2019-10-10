#define PAR_SHAPES_IMPLEMENTATION
#include "external/par_shapes.h"

#include "Globals.h"
#include "Application.h"
#include "ModuleImporter.h"

#include "GL/glew.h"

#include "external/Assimp/include/version.h"


ModuleImporter::ModuleImporter(bool start_enabled) : Module(start_enabled) 
{
	name.assign("ModuleImporter");

	// default colors
	/*fill_color = { 1.0f, 0.35f, 1.0f };
	wire_color;
	d_vertex_color;
	d_vertex_n_color;
	d_vertex_face_color;
	d_vertex_face_n_color;*/
}

ModuleImporter::~ModuleImporter() {}

bool ModuleImporter::Init(Config& config)
{
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);

	LOG("[Info] Using Assimp v%i.%i.%i", aiGetVersionMajor(), aiGetVersionMinor(), aiGetVersionRevision());

	Load(config);

	return true;
}

bool ModuleImporter::Start(Config& config)
{
	//LoadFileMesh("Assets/Models/BakerHouse.fbx");
	//LoadFileMesh("Assets/Models/hammer_low.fbx");
	//LoadFileMesh("Assets/Models/suzanne.solid");
	////LoadFileMesh("Assets/Models/warrior.FBX");


	/*meshes.push_back(CreatePrimitive(CUBE, { 2,0,0 }, { 1,1,1 }));
	meshes.push_back(CreatePrimitive(SPHERE, { -3,0,0 }, { 1,1,1 }));
	meshes.push_back(CreatePrimitive(TETRAHEDRON, { 0,1,0 }, { 1,1.2f,1 }));
	meshes.push_back(CreatePrimitive(ICOSAHEDRON, { 4.5f,0,0 }, { 1,1,1 }));*/

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
		// TODO NEXT: implement new render functionality to pass all this shit (colors, draw modes etc)
		if (fill_mode && wireframe_mode)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glEnable(GL_POLYGON_OFFSET_FILL);
			glPolygonOffset(1.0f, 0.375f);

			glColor4fv((float*)&fill_color);

			(*model)->Render();

			glLineWidth(1.0f);
			glDisable(GL_POLYGON_OFFSET_FILL);
		}
		else if (fill_mode)
		{
			glColor4fv((float*)&fill_color);
			(*model)->Render();
		}

		if (wireframe_mode)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

			glLineWidth(wire_line_width);
			glColor4fv((float*)&wire_color);

			(*model)->Render();

			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		if (debug_vertex_normals)
		{
			glColor4fv((float*)&d_vertex_color);
			(*model)->DebugRenderVertex(v_point_size);
			glColor4fv((float*)&d_vertex_n_color);
			(*model)->DebugRenderVertexNormals(v_n_line_width);
		}
		//glColor3f(1.0f, 1.0f, 0.0f);
		if (debug_face_normals)
		{
			glColor4fv((float*)&d_vertex_face_color);
			(*model)->DebugRenderFacesVertex(f_v_point_size);
			glColor4fv((float*)&d_vertex_face_n_color);
			(*model)->DebugRenderFacesNormals(f_n_line_width);
		}

		/*for (int i = 0; i < (*model)->_idx_size; ++i)
			LOG("%u", (*model)->indices[i]);*/
		
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

			// TODO: divide this on separate functions
			// AND: improve to load all needed data only in one array for vertex,normals,uvs, and another for indices
			// to later use stride before filled the buffers

			aiMesh* assMesh = scene->mMeshes[i]; // :)

			m->name.assign(assMesh->mName.data);
			m->_v_size = assMesh->mNumVertices;
			m->vertices = new float[m->_v_size * 3];
			// the part of i * 3 doesnt make sense, we not store more meshes in one "modelData"
			// instead for each mesh we create other modeldata right now -> removed &m->vertices[i*3]
			memcpy(m->vertices, assMesh->mVertices, sizeof(float) * m->_v_size * 3);
			LOG("[Info] Created new mesh %s with %d vertices", assMesh->mName.data, m->_v_size);
			// LOAD triangle faces indices
			if (assMesh->HasFaces())
			{
				m->_idx_size = assMesh->mNumFaces * 3;
				m->indices = new uint[m->_idx_size];

				for (uint i = 0; i < assMesh->mNumFaces; ++i)
				{
					if (assMesh->mFaces[i].mNumIndices != 3)
					{
						LOG("[Error] Detected face with != 3 indices");
						// TODO: rework this, we need to research how to load models with != 3 vertex per face
						// prevention for index accessor from computevertex and faces normals functions
						memset(&m->indices[i * 3], 0, sizeof(uint) * 3);
					}
					else
						memcpy(&m->indices[i * 3], &assMesh->mFaces[i].mIndices[0], sizeof(uint) * 3);
				}

				LOG("[Info] Containing %i indices for %i triangles", m->_idx_size, assMesh->mNumFaces);
			}

			// LOAD vertex normals
			if (assMesh->HasNormals())
			{
				m->normals = new float[assMesh->mNumVertices * 3];
				memcpy(m->normals, assMesh->mNormals, sizeof(float) * assMesh->mNumVertices * 3);
				LOG("[Info] Loaded vertex normals for %i vertices", assMesh->mNumVertices);
			}

			// LOAD uv coords
			// TODO 1: SEARCH CRASH HERE when loading very complex models, maybe uvs relatives ( with 3 components)
			uint num_uv_channels = assMesh->GetNumUVChannels();
			if (num_uv_channels > 0)
			{
				m->uvs = new float[num_uv_channels * m->_v_size * 2]; // only supports 2 uv for now
				//uint num_uv_components = 0;
				for (uint i = 0; i < num_uv_channels; ++i)
				{
					// for meshes containing more than 1 uv/w channel for vertex 
					if (assMesh->HasTextureCoords(i))
					{
						if (assMesh->mNumUVComponents[i] != 2) // U, UV, UVW of each channel
						{
							// TODO 2: START HERE
							LOG("[Error] Currently only supports 2 components for each UV channel");
							//memset(&m->uvs[(i * assMesh->mNumUVComponents[i] * m->_v_size) + m->_v_size * 2], 0, sizeof(float) * 2);
							//memset(&m->uvs[(i * 2 * m->_v_size)], 0, sizeof(float) * 2 * m->_v_size);
							delete[] m->uvs;
							m->uvs = nullptr;
							LOG("");
						}
						else
						{
							for (int j = 0; j < m->_v_size; ++j)
							{
								memcpy(&m->uvs[(i * assMesh->mNumUVComponents[i] * m->_v_size) + j * 2], &assMesh->mTextureCoords[i][j], sizeof(float) * 2);
							}
							LOG("[Info] Loaded texture coords for Channel %i", i);
						}
						
					}
				}
			}

			// LOAD material texture/s
			// TODO: ONLY diffuse for now
			if (scene->HasMaterials())
			{
				for (uint i = 0; i < scene->mNumMaterials; ++i)
				{
					uint diffuse_tex_count = scene->mMaterials[assMesh->mMaterialIndex]->GetTextureCount(aiTextureType_DIFFUSE);

					assert(diffuse_tex_count <= 1); // WARNING: not tested with models with more than 1 diffuse, needs to improve and test*

					for (int j = 0; j < diffuse_tex_count; ++j)
					{
						aiString tex_path;
						scene->mMaterials[assMesh->mMaterialIndex]->GetTexture(aiTextureType_DIFFUSE, j, &tex_path); // * not tested j
						m->texture_gl_id = App->textures->LoadTexture(tex_path.data); // * more gl index
					}
				}
			}

			m->GenerateBuffers();

			m->ComputeVertexNormals(v_n_line_length); // for debug draw purposes | BEFORE UPDATE BUFFERS!! to compute debugdraw normals before we fill the buffers
			m->ComputeFacesNormals(f_n_line_length);

			m->UpdateBuffers();

			meshes.push_back(m);
			//startup_meshes.push_back(&m);
		}

		aiReleaseImport(scene);
	}
	else
		LOG("[Error] Loading scene %s / no meshes", path);

	
	
	if (scene != nullptr && scene->HasTextures()) // embedded textures into file
	{
		LOG("[Error] scene contains embedded textures, improve the loader to load it!");
	}

	
	

	return ret;
}

bool ModuleImporter::ReComputeVertexNormals(float length)
{
	bool ret = true;

	std::vector<ModelData*>::const_iterator models = meshes.begin();

	for (; models != meshes.end(); ++models)
	{
		(*models)->ComputeVertexNormals(length);
		(*models)->RefillDebugVertexNormalsBuffers();
	}

	v_n_line_length = length;

	return ret;
}

bool ModuleImporter::ReComputeFacesNormals(float length) 
{
	bool ret = true;

	std::vector<ModelData*>::const_iterator models = meshes.begin();

	for (; models != meshes.end(); ++models)
	{
		(*models)->ComputeFacesNormals(length);
		(*models)->RefillDebugFacesNormalsBuffers();
	}

	f_n_line_length = length;

	return ret;
}

ModelData* ModuleImporter::CreatePrimitive(PrimitiveType type, vec3 position, vec3 size)
{
	ModelData* ret = nullptr;
	bool isPlatonic = false; // par_shapes weld its vertices on creation and needs to be unwelded to compute normals
	std::string name;

	par_shapes_mesh* p_mesh = nullptr;

	// TODO: from the panel decide the division slices and stacks of all parametric primitives
	int slices = 20;
	int stacks = 20;

	switch (type)
	{
	case CUBE:
		p_mesh = par_shapes_create_cube();
		isPlatonic = true;
		name.assign("CUBE");
		break;
	case DODECAHEDRON:
		p_mesh = par_shapes_create_dodecahedron();
		isPlatonic = true;
		name.assign("DODECAHEDRON");
		break;
	case TETRAHEDRON:
		p_mesh = par_shapes_create_tetrahedron();
		isPlatonic = true;
		name.assign("TETRAHEDRON");
		break;
	case OCTOHEDRON:
		p_mesh = par_shapes_create_octahedron();
		isPlatonic = true;
		name.assign("TETRAHEDRON");
		break;
	case ICOSAHEDRON:
		p_mesh = par_shapes_create_icosahedron();
		isPlatonic = true;
		name.assign("ICOSAHEDRON");
		break;
	case PLANE:
		p_mesh = par_shapes_create_plane(slices, stacks);
		name.assign("PLANE");
		break;
	case SPHERE:
		p_mesh = par_shapes_create_parametric_sphere(slices, stacks);
		name.assign("SPHERE");
		break;
	case CYLINDER:
		p_mesh = par_shapes_create_cylinder(slices, stacks);
		name.assign("CYLINDER");
		break;
	case CONE:
		p_mesh = par_shapes_create_cone(slices, stacks);
		name.assign("CONE");
		break;
	case TORUS:
		p_mesh = par_shapes_create_torus(slices, stacks, size.x * 0.5f); // Gets the radius from the size x component
		name.assign("TORUS");
		break;
	case MAX:
		break;
	default:
		break;
	}

	if (p_mesh == nullptr)
		return nullptr;

	if (isPlatonic)
	{
		par_shapes_unweld(p_mesh, true);
		par_shapes_compute_normals(p_mesh);
	}

	par_shapes_scale(p_mesh, size.x, size.y, size.z);
	par_shapes_translate(p_mesh, position.x, position.y, position.z);

	// pass data to own class

	ModelData* model = new ModelData(p_mesh->points, p_mesh->triangles, p_mesh->normals, p_mesh->tcoords, p_mesh->npoints, p_mesh->ntriangles);

	if (model != nullptr) model->name.assign(name.data());

	par_shapes_free_mesh(p_mesh);

	return model == nullptr ? nullptr : model;


}

bool ModuleImporter::Save(Config& config)
{
	bool ret = true;

	ret = config.AddBool("wireframe_mode", wireframe_mode);
	ret = config.AddBool("fill_mode", fill_mode);
	ret = config.AddBool("debug_vertex_normals", debug_vertex_normals);
	ret = config.AddBool("debug_face_normals", debug_face_normals);

	ret = config.AddFloatArray("fill_color", (float*)&fill_color, 4);
	ret = config.AddFloatArray("wire_color", (float*)&wire_color, 4);
	ret = config.AddFloatArray("d_vertex_color", (float*)&d_vertex_color, 4);
	ret = config.AddFloatArray("d_vertex_n_color", (float*)&d_vertex_n_color, 4);
	ret = config.AddFloatArray("d_vertex_face_color", (float*)&d_vertex_face_color, 4);
	ret = config.AddFloatArray("d_vertex_face_n_color", (float*)&d_vertex_face_n_color, 4);

	ret = config.AddFloat("wire_line_width", wire_line_width);
	ret = config.AddFloat("v_point_size", v_point_size);
	ret = config.AddFloat("f_v_point_size", f_v_point_size);
	ret = config.AddFloat("v_n_line_width", v_n_line_width);
	ret = config.AddFloat("f_n_line_width", f_n_line_width);

	ret = config.AddFloat("v_n_line_length", v_n_line_length);
	ret = config.AddFloat("f_n_line_length", f_n_line_length);

	return ret;
}

void ModuleImporter::Load(Config& config)
{

	wireframe_mode = config.GetBool("wireframe_mode", wireframe_mode);
	fill_mode = config.GetBool("fill_mode", fill_mode);
	debug_vertex_normals = config.GetBool("debug_vertex_normals", debug_vertex_normals);
	debug_face_normals = config.GetBool("debug_face_normals", debug_face_normals);

	// load colors ----------------------------------------------------------------------------------
	fill_color.x = config.GetFloat("fill_color", fill_color.x, 0);
	fill_color.y = config.GetFloat("fill_color", fill_color.y, 1);
	fill_color.z = config.GetFloat("fill_color", fill_color.z, 2);
	fill_color.w = config.GetFloat("fill_color", fill_color.w, 3);

	wire_color.x = config.GetFloat("wire_color", wire_color.x, 0);
	wire_color.y = config.GetFloat("wire_color", wire_color.y, 1);
	wire_color.z = config.GetFloat("wire_color", wire_color.z, 2);
	wire_color.w = config.GetFloat("wire_color", wire_color.w, 3);

	d_vertex_color.x = config.GetFloat("d_vertex_color", d_vertex_color.x, 0);
	d_vertex_color.y = config.GetFloat("d_vertex_color", d_vertex_color.y, 1);
	d_vertex_color.z = config.GetFloat("d_vertex_color", d_vertex_color.z, 2);
	d_vertex_color.w = config.GetFloat("d_vertex_color", d_vertex_color.w, 3);

	d_vertex_n_color.x = config.GetFloat("d_vertex_n_color", d_vertex_n_color.x, 0);
	d_vertex_n_color.y = config.GetFloat("d_vertex_n_color", d_vertex_n_color.y, 1);
	d_vertex_n_color.z = config.GetFloat("d_vertex_n_color", d_vertex_n_color.z, 2);
	d_vertex_n_color.w = config.GetFloat("d_vertex_n_color", d_vertex_n_color.w, 3);

	d_vertex_face_color.x = config.GetFloat("d_vertex_face_color", d_vertex_face_color.x, 0);
	d_vertex_face_color.y = config.GetFloat("d_vertex_face_color", d_vertex_face_color.y, 1);
	d_vertex_face_color.z = config.GetFloat("d_vertex_face_color", d_vertex_face_color.z, 2);
	d_vertex_face_color.w = config.GetFloat("d_vertex_face_color", d_vertex_face_color.w, 3);

	d_vertex_face_n_color.x = config.GetFloat("d_vertex_face_n_color", d_vertex_face_color.x, 0);
	d_vertex_face_n_color.y = config.GetFloat("d_vertex_face_n_color", d_vertex_face_color.y, 1);
	d_vertex_face_n_color.z = config.GetFloat("d_vertex_face_n_color", d_vertex_face_color.z, 2);
	d_vertex_face_n_color.w = config.GetFloat("d_vertex_face_n_color", d_vertex_face_color.w, 3);
	// -----------------------------------------------------------------------------------------------

	wire_line_width = config.GetFloat("wire_line_width", wire_line_width);
	v_point_size = config.GetFloat("v_point_size", v_point_size);
	f_v_point_size = config.GetFloat("f_v_point_size", f_v_point_size);
	v_n_line_width = config.GetFloat("v_n_line_width", v_n_line_width);
	f_n_line_width = config.GetFloat("f_n_line_width", f_n_line_width);

	// if we have meshes, and line length doesnt match, recompute normals ----
	float temp_v = v_n_line_length;
	v_n_line_length = config.GetFloat("v_n_line_length", v_n_line_length);
	if (temp_v != v_n_line_length)
		ReComputeVertexNormals(v_n_line_length);

	float temp_f = f_n_line_length;
	f_n_line_length = config.GetFloat("f_n_line_length", f_n_line_length);
	if (temp_f != f_n_line_length)
		ReComputeFacesNormals(f_n_line_length);
	// -----------------------------------------------------------------------
	
}

std::vector<ModelData*>& ModuleImporter::GetModels()
{
	return meshes;
}

void ModuleImporter::ImportFileFromPath(const char* path)
{
	std::string normalized_path = App->file_sys->NormalizePath(path);
	std::string final_path;

	// Duplicate file to the indicated internal path 
	if (App->file_sys->DuplicateFile(normalized_path.c_str(),  "Project/Assets" , final_path))
	{
		// Load asset and create meta data 
		// If is a mesh import and add to scene 
		LoadFileMesh(normalized_path.c_str()); // Model test
	}
}