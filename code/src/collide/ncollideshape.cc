#define N_IMPLEMENTS nCollideShape
//------------------------------------------------------------------------------
//  ncollideshape.cc
//  (C) 2000 RadonLabs GmbH -- A.Weissflog
//------------------------------------------------------------------------------
#include "kernel/nfileserver2.h"
#include "collide/ncollideserver.h"
#include "collide/ncollideshape.h"
#include "mathlib/matrix.h"
#include "mathlib/vector.h"
#include "gfx/ngfxtypes.h"

//------------------------------------------------------------------------------
/**
*/
nCollideShape::~nCollideShape()
{
    n_assert(0 == this->refCount);
}

//------------------------------------------------------------------------------
/**
*/
void
nCollideShape::Begin(int /*numVertices*/, int /*numTriangles*/)
{
    n_assert(!this->isInitialized);
    this->radius = 0.0f;
}

//------------------------------------------------------------------------------
/**
*/
void
nCollideShape::SetVertex(int /*index*/, vector3& v)
{
    float l = v.len();
    if (l > this->radius) 
    {
        this->radius = l;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nCollideShape::SetTriangle(int /*index*/, int /*v0Index*/, int /*v1Index*/, int /*v2Index*/)
{
    // empty (actual functionality in subclasses)
}

//------------------------------------------------------------------------------
/**
*/
void
nCollideShape::End()
{
    n_assert(!this->isInitialized);
    this->isInitialized = true;
}

//------------------------------------------------------------------------------
/** 
    Actual collide check is left to subclasses.
*/
bool 
nCollideShape::Collide(nCollType /*ctype*/,
                       matrix44& /*ownMatrix*/, 
                       nCollideShape* /*otherShape*/, 
                       matrix44& /*otherMatrix*/, 
                       nCollideReport& /*collReport*/)
{
    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool
nCollideShape::LineCheck(nCollType /*collType*/,
                         const matrix44& /*ownMatrix*/,
                         const line3& /*line*/,
                         nCollideReport& /*collReport*/)
{
    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool
nCollideShape::SphereCheck(nCollType /*collType*/,
                           const matrix44& /*ownMatrix*/,
                           const sphere& /*ball*/,
                           nCollideReport& /*collReport*/)
{
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void nCollideShape::Visualize(nGfxServer* /*gfxServer*/)
{ 
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool 
nCollideShape::Load(nFileServer2 *fs, const char *fname)
{
    if (strstr(fname, ".n3d"))
    {
        return this->LoadN3D(fs, fname);
    }
    else if (strstr(fname, ".nvx"))
    {
        return this->LoadNVX(fs, fname);
    }
    else
    {
        n_printf("nCollideShape: Unknown file format '%s'\n", fname);
        return false;
    }
}

//------------------------------------------------------------------------------
/**
    Load a geometry file in n3d format as collide shape.
*/
bool
nCollideShape::LoadN3D(nFileServer2* fileServer, const char* filename)
{
    n_assert(fileServer);
    n_assert(filename);

    // load wavefront file
    int numVertices;
    int numFaces;
	char line[1024];

    nFile* file = fileServer->NewFileObject();
    n_assert(file);
    if (!file->Open(filename, "r"))
    {
        n_printf("nCollideShape::Load(): Could not open file '%s'!\n",filename);
        return false;
    }

    // Pass1, count vertices and faces
    numVertices = 0;
    numFaces    = 0;
	while (file->GetS(line, sizeof(line))) 
    {
	    char *kw = strtok(line," \t\n");
	    if (kw) 
        {
	        if (strcmp(kw,"v")==0)      numVertices++;
            else if (strcmp(kw,"f")==0) numFaces++;
        }
    }

    // any errors?
    if (0 == numVertices) 
    {
        n_printf("nCollideShape::Load(): file '%s' has no vertices!\n", filename);
        file->Close();
        delete file;
        return false;
    }
    if (0 == numFaces) 
    {
        n_printf("nCollideShape::Load(): file '%s' has no triangles!\n", filename);
        file->Close();
        delete file;
        return false;
    }

    // read vertices, add triangles...
    this->Begin(numVertices, numFaces);
    file->Seek(0, nFile::START);
    int actVertex = 0;
    int actFace   = 0;
    vector3 vec3;
    while (file->GetS(line, sizeof(line))) 
    {
		char *kw = strtok(line," \t\n");
		if (kw) 
        {
			if (strcmp(kw, "v") == 0) 
            {
                // a vertex definition
                char *xs = strtok(NULL, " \t\n");
                char *ys = strtok(NULL, " \t\n");
                char *zs = strtok(NULL, " \t\n");
                n_assert(xs && ys && zs);
                float x = (float) atof(xs);
                float y = (float) atof(ys);
                float z = (float) atof(zs);
                vec3.set(x, y, z);
                this->SetVertex(actVertex++, vec3);
            } 
            else if (strcmp(kw, "f") == 0) 
            {
                // a face definition only read first index in 
                // corner definition, read only 3 corners, ignore all others
                ulong i;
                int indices[3];
                for (i = 0; i < 3; i++) 
                {
                    char *tmp = strtok(NULL, " \t\n");
                    if (tmp) 
                    {
                        char *slash;
                        if ((slash = strchr(tmp,'/'))) *slash=0;
                        int curIndex = atoi(tmp) - 1;
                        n_assert(curIndex < numVertices);
                        indices[i] = curIndex;
                    }
                }
                // add the triangle to the geometry
                this->SetTriangle(actFace++, indices[0], indices[1], indices[2]);
            }
        }
    }

    // finish adding geometry
    this->End();
    file->Close();
    delete file;
    return true;
}

//------------------------------------------------------------------------------
/**
    Reads a 32 bit int from file. FIXME: Endian correction!
*/
int
nCollideShape::readInt(nFile* file)
{
    n_assert(file);
    int var;
    file->Read(&var, sizeof(var));
    return var;
}

//------------------------------------------------------------------------------
/**
*/
bool
nCollideShape::LoadNVX(nFileServer2* fileServer, const char* filename)
{
    n_assert(fileServer);
    n_assert(filename);

    // open file
    nFile* file = fileServer->NewFileObject();
    n_assert(file);
    if (!file->Open(filename, "rb"))
    {
        n_printf("nCollideShape: could not load file '%s'\n", filename);
        return false;
    }
    
    // read header
    int magicNumber = this->readInt(file);
    if (magicNumber != 'NVX1')
    {
        n_printf("nNvxMeshLoader: '%s' is not a NVX1 file!\n", filename);
        file->Close();
        delete file;
        return false;
    }
    int numVertices        = this->readInt(file);
    int numIndices         = this->readInt(file);
    int numWingedEdges     = this->readInt(file);
    nVertexType vertexType = (nVertexType) this->readInt(file);
    int dataStart          = this->readInt(file);
    int dataSize           = this->readInt(file);

    // compute vertex stride
    n_assert(vertexType & N_VT_COORD);
    int vertexStride = 3;
    if (vertexType & N_VT_NORM)
    {
        vertexStride += 3;
    }
    if (vertexType & N_VT_RGBA)
    {
        vertexStride += 1;
    }
    if (vertexType & N_VT_UV0)
    {
        vertexStride += 2;
    }
    if (vertexType & N_VT_UV1)
    {
        vertexStride += 2;
    }
    if (vertexType & N_VT_UV2)
    {
        vertexStride += 2;
    }
    if (vertexType & N_VT_UV3)
    {
        vertexStride += 2;
    }
    if (vertexType & N_VT_JW)
    {
        vertexStride += 6;
    }

    // read the complete data block into a memory buffer and close file
    char* buffer = (char*) n_malloc(dataSize);
    file->Seek(dataStart, nFile::START);
    int num = file->Read(buffer, dataSize);
    file->Close();
    delete file;
    file = 0;
    n_assert(num == dataSize);

    // get pointer to beginning of vertices and to beginning of indices
    float* vertexData = (float*) buffer;
    int wingedEdgeStride  = 2;
    ushort* indexData = (ushort*) (vertexData + (numVertices * vertexStride) + (numWingedEdges * wingedEdgeStride));

    // iterate over triangle list and add triangles to collide shape
    int numTriangles = numIndices / 3;
    this->Begin(numVertices, numTriangles);

    // add vertices
    vector3 vec3;
    int i;
    for (i = 0; i < numVertices; i++)
    {
        float* vPtr = vertexData + (i * vertexStride);
        vec3.set(vPtr[0], vPtr[1], vPtr[2]);
        this->SetVertex(i, vec3);
    }

    // add triangles
    for (i = 0; i < numTriangles; i++)
    {
        int i0 = *indexData++;
        int i1 = *indexData++;
        int i2 = *indexData++;
        this->SetTriangle(i, i0, i1, i2);
    }

    this->End();
    n_free(buffer);
    return true;
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
