#define N_IMPLEMENTS nGfxServer
//------------------------------------------------------------------------------
//  nnvxmeshloader.cc
//  (C) 2001 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gfx/nnvxmeshloader.h"
#include "gfx/ngfxserver.h"
#include "shadow/nshadowserver.h"
#include "gfx/nvertexbuffer.h"
#include "gfx/nindexbuffer.h"
#include "shadow/nshadowcaster.h"
#include "kernel/nfileserver2.h"

//------------------------------------------------------------------------------
/**
    Reads a 32 bit int from file. FIXME: Endian correction!
*/
int
nNvxMeshLoader::readInt(nFile* file)
{
    n_assert(file);
    int var;
    file->Read(&var, sizeof(var));
    return var;
}

//------------------------------------------------------------------------------
/**
    Constructs vertex buffer, index buffer and shadow caster objects, and
    fills them with data read from nvx file. All objects will generally
    be shared. So if the mesh has already been loaded, it will not be
    loaded again, instead pointers to shared objects will be returned.

    @param fileName                  file to load mesh data from
*/
bool
nNvxMeshLoader::Load(const char* fileName)
{
    n_assert(fileName);
    n_assert(this->inBegin);
    n_assert(this->gfxServer);
    n_assert(this->shadowServer);

    // first see if the mesh is already loaded
    char rsrc_name[N_MAXPATH];
    n_strncpy2(rsrc_name, fileName, sizeof(rsrc_name) - 3);

    // alter resource name based on readonly flag
    if (this->readOnly) strcat(rsrc_name,"_ro");
    else                strcat(rsrc_name,"_rw");

    // if the mesh file already has been loaded, just get the pointers
    // to the associated objects and increment their ref count
    this->vertexBuffer = this->gfxServer->FindVertexBuffer(rsrc_name);
    if (this->vertexBuffer)
    {
        this->vertexBuffer->AddRef();

        // find index buffer
        this->indexBuffer = this->gfxServer->FindIndexBuffer(rsrc_name);
        n_assert(this->indexBuffer);
        this->indexBuffer->AddRef();

        // find (optional!) shadow caster
        this->shadowCaster = this->shadowServer->FindCaster(rsrc_name);
        if (this->shadowCaster)
        {
            this->shadowCaster->AddRef();
        }
        return true;
    }

    // new mesh, open the file
    nFile* file = this->refFileServer->NewFileObject();
    n_assert(file);
    if (!file->Open(fileName, "rb"))
    {
        n_printf("nNvxMeshLoader: could not open file '%s'!\n", fileName);
        return false;
    }

    // read header (make sure its an NVX1 file!)
    int magicNumber = this->readInt(file);
    if (magicNumber != 'NVX1')
    {
        n_printf("nNvxMeshLoader: '%s' is not a NVX1 file!\n", fileName);
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

    // initialize a bound box
    bbox3 bbox;
    bbox.begin_grow();

    // read the complete data block into a memory buffer and close file
    void* buffer = n_malloc(dataSize);
    file->Seek(dataStart, nFile::START);
    int num = file->Read(buffer, dataSize);
    file->Close();
    delete file;
    file = 0;
    n_assert(num == dataSize);

    // create vertex buffer
    nVBufType vbufType = this->readOnly ? N_VBTYPE_READONLY : N_VBTYPE_STATIC;
    this->vertexBuffer = this->gfxServer->NewVertexBuffer(rsrc_name, vbufType, vertexType, numVertices);
    n_assert(this->vertexBuffer);
    nColorFormat colorFormat = this->vertexBuffer->GetColorFormat();
    this->vertexBuffer->LockVertices();
    
    // create shadow caster
    if (numWingedEdges > 0)
    {
        this->shadowCaster = this->shadowServer->NewCaster(rsrc_name);
        this->shadowCaster->Initialize(numVertices, numWingedEdges);
        this->shadowCaster->Lock();
    }

    // create index buffer
    this->indexBuffer = this->gfxServer->NewIndexBuffer(rsrc_name);
    n_assert(this->indexBuffer);

    char* ptr = (char*) buffer;
    vector3 vec3;
    vector2 vec2;
    int i;
#define read_elm(type) *(type*)ptr; ptr += sizeof(type);   

    for (i = 0; i < numVertices; i++)
    {
        // read coord
        if (vertexType & N_VT_COORD)
        {
            float x = read_elm(float);
            float y = read_elm(float);
            float z = read_elm(float);
            vec3.set(x, y, z);
            bbox.grow(vec3);
            this->vertexBuffer->Coord(i, vec3);
            if (this->shadowCaster)
            {
                this->shadowCaster->Coord(i, vec3);
            }
        }
            
        // read normal
        if (vertexType & N_VT_NORM)
        {
            float x = read_elm(float);
            float y = read_elm(float);
            float z = read_elm(float);
            vec3.set(x, y, z);
            this->vertexBuffer->Norm(i, vec3);
        }

        // read color
        if (vertexType & N_VT_RGBA)
        {
            unsigned int color = read_elm(unsigned int);
            if (colorFormat == N_COLOR_RGBA)
            {
                // swap red and blue components
                unsigned int c = color & 0xff00ff00;
                c |= (color & 0x000000ff) << 16;
                c |= (color & 0x00ff0000) >> 16;
                color = c;
            }
            this->vertexBuffer->Color(i, color);
        }

        // read texture coordinates
        if (vertexType & N_VT_UV0)
        {
            float x = read_elm(float);
            float y = read_elm(float);
            vec2.set(x, y);
            this->vertexBuffer->Uv(i, 0, vec2);
        }
        if (vertexType & N_VT_UV1)
        {
            float x = read_elm(float);
            float y = read_elm(float);
            vec2.set(x, y);
            this->vertexBuffer->Uv(i, 1, vec2);
        }
        if (vertexType & N_VT_UV2)
        {
            float x = read_elm(float);
            float y = read_elm(float);
            vec2.set(x, y);
            this->vertexBuffer->Uv(i, 2, vec2);
        }
        if (vertexType & N_VT_UV3)
        {
            float x = read_elm(float);
            float y = read_elm(float);
            vec2.set(x, y);
            this->vertexBuffer->Uv(i, 3, vec2);
        }

        // read joint indices and vertex weights for skinning
        if (vertexType & N_VT_JW)
        {
            short ji0 = read_elm(short);
            short ji1 = read_elm(short);
            short ji2 = read_elm(short);
            short ji3 = read_elm(short);
            float w0  = read_elm(float);
            float w1  = read_elm(float);
            float w2  = read_elm(float);
            float w3  = read_elm(float);
            
            if (-1 == ji1)
            {
                this->vertexBuffer->JointWeight1(i, ji0, w0);
            }
            else if (-1 == ji2)
            {
                this->vertexBuffer->JointWeight2(i, ji0, w0, ji1, w1);
            }
            else if (-1 == ji3)
            {
                this->vertexBuffer->JointWeight3(i, ji0, w0, ji1, w1, ji2, w2);
            }
            else
            {
                this->vertexBuffer->JointWeight4(i, ji0, w0, ji1, w1, ji2, w2, ji3, w3);
            }
        }
    }

    // read winged edges
    nWingedEdge we;
    for (i = 0; i < numWingedEdges; i++)
    {
        ushort we0 = read_elm(ushort);
        ushort we1 = read_elm(ushort);
        ushort we2 = read_elm(ushort);
        ushort we3 = read_elm(ushort);
        we.Set(we0, we1, we2, we3);
        this->shadowCaster->Edge(i, we);
    }

    // read indices
    if (numIndices > 0)
    {
        this->indexBuffer->Begin(N_IBTYPE_STATIC, N_PTYPE_TRIANGLE_LIST, numIndices);
        for (i = 0; i < numIndices; i++)
        {
            ushort index = read_elm(ushort);
            this->indexBuffer->Index(i, index);
        }
        this->indexBuffer->End();
    }

    // clean up and return
    this->vertexBuffer->UnlockVertices();
    if (this->shadowCaster)
    {
        this->shadowCaster->Unlock();
    }
    this->vertexBuffer->SetBBox(bbox);
    n_free(buffer);
    return true;
}
//------------------------------------------------------------------------------
