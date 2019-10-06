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
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);

	LOG("[Info] Using Assimp v%i.%i.%i", aiGetVersionMajor(), aiGetVersionMinor(), aiGetVersionRevision());

	Load(config);

	return true;
}

bool ModuleImporter::Start(Config& config)
{

	LoadFileMesh("Assets/Models/suzanne.obj");
	//LoadFileMesh("Assets/Models/warrior.FBX");

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
			//m->_v_size = scene->mMeshes[i]->mNumVertices;

			// TODO: divide this on separate functions

			aiMesh* assMesh = scene->mMeshes[i]; // :)
			
			m->_v_size = assMesh->mNumVertices;
			m->vertices = new float[m->_v_size * 3];
			// the part of i * 3 doesnt make sense, we not store more meshes in one "modelData"
			// instead for each mesh we create other modeldata right now
			memcpy(&m->vertices[i * 3], assMesh->mVertices, sizeof(float) * m->_v_size * 3);
			LOG("[Info] Created new mesh with %d vertices", m->_v_size);

			// LOAD triangle faces indices
			if (assMesh->HasFaces())
			{
				m->_idx_size = assMesh->mNumFaces * 3;
				m->indices = new uint[m->_idx_size];

				int t = 0;
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

			// LOAD vertex normals
			if (assMesh->HasNormals())
			{
				m->normals = new float[assMesh->mNumVertices * 3];
				memcpy(&m->normals[i * 3], assMesh->mNormals, sizeof(float) * assMesh->mNumVertices * 3);
				LOG("[Info] Loaded vertex normals for mesh %s", assMesh->mName.data);
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
		LOG("[Error] Loading scene %s", path);

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