#define N_IMPLEMENTS nGfxServer
//------------------------------------------------------------------------------
//  nn3dmeshloader.cc
//  (C) 2001 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gfx/nn3dmeshloader.h"
#include "gfx/ngfxserver.h"
#include "shadow/nshadowserver.h"
#include "gfx/nvertexbuffer.h"
#include "gfx/nindexbuffer.h"
#include "shadow/nshadowcaster.h"
#include "kernel/nfileserver2.h"

//------------------------------------------------------------------------------
/**
    qsort() hook for sorting triangle indices.

    @param  elm0    ushort pointer to first index
    @param  elm1    ushort pointer to second index
    @return         comparison result
*/
static
int 
triangle_sort_hook(const void *elm0, const void *elm1)
{
    ushort *i0 = (ushort *) elm0;
    ushort *i1 = (ushort *) elm1;
    return int(*i0) - int(*i1);
}

//------------------------------------------------------------------------------
/**
    Fills the index buffer with the triangle indices.

    @param  indices         pointer to indices
    @param  num_indices     number of indices
*/
void
nN3dMeshLoader::createTriList(ushort* indices, int num_indices)
{
    n_assert(this->indexBuffer);

    if (num_indices > 0)
    {
        // sort the triangles to improve cache coherence
        qsort(indices, num_indices/3, 3*sizeof(ushort), triangle_sort_hook);
    
        this->indexBuffer->Begin(N_IBTYPE_STATIC, N_PTYPE_TRIANGLE_LIST, num_indices);
	    int i;
	    for (i = 0; i < num_indices; i++) 
        {
            this->indexBuffer->Index(i, indices[i]);
        }
        this->indexBuffer->End();
    }
}

//------------------------------------------------------------------------------
/**
    Constructs vertex buffer, index buffer and shadow caster objects, and
    fills them with data read from n3d file. All objects will generally
    be shared. So if the mesh has already been loaded, it will not be
    loaded again, instead pointers to shared objects will be returned.
*/
bool
nN3dMeshLoader::Load(const char* fileName)
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
    if (!file->Open(fileName, "r"))
    {
        n_printf("nN3dMeshLoader: could not open file '%s'!\n", fileName);
        return false;
    }

    // pass 1: count lods, number of vertices and faces
    int num_v    = 0;
    int num_vn   = 0;
    int num_rgba = 0;
    int num_vt   = 0;
    int num_vt1  = 0;
    int num_vt2  = 0;
    int num_vt3  = 0;
    int num_primitives = 0;
    int num_indices    = 0;
    int num_jw = 0;
    int num_we = 0;

    char line[1024];
    while (file->GetS(line, sizeof(line)))
    {
	    char *kw = strtok(line, " \t\n");
        if (kw) 
        {
            if      (strcmp(kw, "v")    == 0)   num_v++;
            else if (strcmp(kw, "vn")   == 0)   num_vn++;
            else if (strcmp(kw, "rgba") == 0)   num_rgba++;
            else if (strcmp(kw, "vt")   == 0)   num_vt++;
            else if (strcmp(kw, "vt1")  == 0)   num_vt1++;
            else if (strcmp(kw, "vt2")  == 0)   num_vt2++;
            else if (strcmp(kw, "vt3")  == 0)   num_vt3++;
            else if (strcmp(kw, "jw1")  == 0)   num_jw++;
            else if (strcmp(kw, "jw2")  == 0)   num_jw++;
            else if (strcmp(kw, "jw3")  == 0)   num_jw++;
            else if (strcmp(kw, "jw4")  == 0)   num_jw++;
            else if (strcmp(kw, "we")   == 0)   num_we++;
            else if (strcmp(kw, "f")    == 0) 
            {
                // a triangle definition
                num_primitives = 1;
                num_indices += 3;
            } 
        }
    }

    // catch obvious errors and prepare loading
    bool error = false;
    int i;
    if (num_v == 0) 
    {
        n_printf("nN3dMeshLoader: no vertices in '%s'!\n", fileName);
        error = true;
    }
    if ((num_vn > 0) && (num_vn != num_v)) 
    {
        n_printf("nN3dMeshLoader: normal count doesn't match vertex count in '%s' (not flattened?)!\n", fileName);
        error = true;
    }
    if ((num_rgba > 0) && (num_rgba != num_v)) 
    {
        n_printf("nN3dMeshLoader: rgba count doesn't match vertex count in '%s' (not flattened?)!\n", fileName);
        error = true;
    }
    if ((num_vt > 0) && (num_vt != num_v)) 
    {
        n_printf("nN3dMeshLoader: vt count doesn't match vertex count in '%s' (not flattened?)!\n", fileName);
        error = true;
    }
    if ((num_vt1 > 0) && (num_vt1 != num_v)) 
    {
        n_printf("nN3dMeshLoader: vt1 count doesn't match vertex count in '%s' (not flattened?)!\n", fileName);
        error = true;
    }
    if ((num_vt2 > 0) && (num_vt2 != num_v)) 
    {
        n_printf("nN3dMeshLoader: vt2 count doesn't match vertex count in '%s' (not flattened?)!\n", fileName);
        error = true;
    }
    if ((num_vt3 > 0) && (num_vt3 != num_v)) 
    {
        n_printf("nN3dMeshLoader: vt3 count doesn't match vertex count in '%s' (not flattened?)!\n", fileName);
        error = true;
    }
    if ((num_jw > 0) && (num_jw != num_v)) 
    {
        n_printf("nN3dMeshLoader: jw1/jw2/jw3/jw4 count doesn't match vertex count in '%s'!\n", fileName);
        error = true;
    }

    if (error) 
    {
        file->Close();
        delete file;
        return false;
    }

    // get the vertex type
    int vertexType;
    vertexType = N_VT_COORD;
    if (num_vn   > 0) vertexType |= N_VT_NORM;
    if (num_rgba > 0) vertexType |= N_VT_RGBA;
    if (num_vt   > 0) vertexType |= N_VT_UV0;
    if (num_vt1  > 0) vertexType |= N_VT_UV1;
    if (num_vt2  > 0) vertexType |= N_VT_UV2;
    if (num_vt3  > 0) vertexType |= N_VT_UV3;
    if (num_jw   > 0) vertexType |= N_VT_JW;

    // create vertex buffer
    nVBufType vbufType = this->readOnly ? N_VBTYPE_READONLY : N_VBTYPE_STATIC;
    this->vertexBuffer = this->gfxServer->NewVertexBuffer(rsrc_name, vbufType, vertexType, num_v);
    n_assert(this->vertexBuffer);
    nColorFormat color_format = this->vertexBuffer->GetColorFormat();
    this->vertexBuffer->LockVertices();
    
    // create index buffer
    this->indexBuffer = this->gfxServer->NewIndexBuffer(rsrc_name);
    n_assert(this->indexBuffer);

    // create and initialize shadow caster
    if (num_we > 0)
    {
        this->shadowCaster = this->shadowServer->NewCaster(rsrc_name);
        this->shadowCaster->Initialize(num_v, num_we);
        this->shadowCaster->Lock();
    }

    // create a bounding box for the vertex buffer
    bbox3 bbox;
    bbox.begin_grow();

    // create temporary index buffer
    ushort *indices = (ushort *) n_malloc(num_indices * sizeof(ushort));

	// pass 2 read vertices and primitives
    num_indices  = 0;
    int act_v    = 0;
    int act_vn   = 0;
    int act_rgba = 0;
    int act_vt   = 0;
    int act_vt1  = 0;
    int act_vt2  = 0;
    int act_vt3  = 0;
    int act_jw   = 0;
    int act_we   = 0;
    file->Seek(0, nFile::START);
	while (file->GetS(line, sizeof(line))) 
    {
		char *kw = strtok(line, " \t\n");
		if (kw) 
        {
			if (strcmp(kw, "v") == 0) 
            {
	            char *xs = strtok(NULL, " \t\n");
                char *ys = strtok(NULL, " \t\n");
                char *zs = strtok(NULL, " \t\n");
                if (xs && ys && zs) 
                {
                    vector3 v((float)atof(xs), (float)atof(ys), (float)atof(zs));
                    // grow bounding box
                    bbox.grow(v);
                    // write coord to vertex buffer
                    this->vertexBuffer->Coord(act_v, v);
                    if (this->shadowCaster)
                    {
                        // write coord to shadow caster
                        this->shadowCaster->Coord(act_v, v);
                    }
                    act_v++;
				} 
                else 
                {
                    n_error("Broken 'v' line in '%s'!\n", fileName);
				}
			} 
            else if ((strcmp(kw, "vn") == 0) && (vertexType & N_VT_NORM)) 
            {
	            char *nxs = strtok(NULL, " \t\n");
                char *nys = strtok(NULL, " \t\n");
                char *nzs = strtok(NULL, " \t\n");
                if (nxs && nys && nzs) 
                {
                    vector3 v((float)atof(nxs), (float)atof(nys), (float)atof(nzs));
					this->vertexBuffer->Norm(act_vn++, v);
				} 
                else 
                {
                    n_error("Broken 'vn' line in '%s'!\n", fileName);
				}
			} 
            else if ((strcmp(kw, "rgba")==0) && (vertexType & N_VT_RGBA)) 
            {
				char *rs = strtok(NULL, " \t\n");
                char *gs = strtok(NULL, " \t\n");
                char *bs = strtok(NULL, " \t\n");
				char *as = strtok(NULL, " \t\n");
				if (rs && gs && bs && as) 
                {
                    float r = (float) atof(rs);
                    float g = (float) atof(gs);
                    float b = (float) atof(bs);
				    float a = (float) atof(as);
                    if (N_COLOR_RGBA == color_format) 
                    {
                        this->vertexBuffer->Color(act_rgba++, n_f2rgba(r, g, b, a));
                    } 
                    else 
                    {
                        this->vertexBuffer->Color(act_rgba++,n_f2bgra(r, g, b, a));
                    }
				} 
                else 
                {
                    n_error("Broken 'rgba' line in '%s'!\n", fileName);
				}
			} 
            else if ((strcmp(kw, "vt") == 0) && (vertexType & N_VT_UV0)) 
            {
	            char *us = strtok(NULL, " \t\n");
                char *vs = strtok(NULL, " \t\n");
                if (us && vs) 
                {
                    vector2 v((float)atof(us), (float)atof(vs));
					this->vertexBuffer->Uv(act_vt++, 0, v);
				} 
                else 
                {
                    n_error("Broken 'vt' line in '%s'!\n", fileName);
				}
			} 
            else if ((strcmp(kw,"vt1") == 0) && (vertexType & N_VT_UV1)) 
            {
	            char *us = strtok(NULL, " \t\n");
                char *vs = strtok(NULL, " \t\n");
                if (us && vs) 
                {
                    vector2 v((float)atof(us), (float)atof(vs));
					this->vertexBuffer->Uv(act_vt1++,1, v);
				} 
                else 
                {
                    n_error("Broken 'vt1' line in '%s'!\n", fileName);
				}
			} 
            else if ((strcmp(kw, "vt2") == 0) && (vertexType & N_VT_UV2)) 
            {
	            char *us = strtok(NULL, " \t\n");
                char *vs = strtok(NULL, " \t\n");
                if (us && vs) 
                {
                    vector2 v((float)atof(us), (float)atof(vs));
					this->vertexBuffer->Uv(act_vt2++, 2, v);
				} 
                else 
                {
                    n_error("Broken 'vt2' line in '%s'!\n", fileName);
				}
			} 
            else if ((strcmp(kw, "vt3") == 0) && (vertexType & N_VT_UV3)) 
            {
	            char *us = strtok(NULL, " \t\n");
                char *vs = strtok(NULL, " \t\n");
                if (us && vs) 
                {
                    vector2 v((float)atof(us), (float)atof(vs));
					this->vertexBuffer->Uv(act_vt3++, 3, v);
				} 
                else 
                {
                    n_error("Broken 'vt3' line in '%s'!\n", fileName);
				}
            } 
            else if ((strcmp(kw, "jw1") == 0) && (vertexType & N_VT_JW)) 
            {
                char *j0s = strtok(NULL, " \t\n");
                char *w0s = strtok(NULL, " \t\n");
                if (j0s && w0s) 
                {
                    int j0    = atoi(j0s);
                    float w0  = float(atof(w0s));
                    this->vertexBuffer->JointWeight1(act_jw++, j0, w0);
                } 
                else 
                {
                    n_error("Broken 'jw1' line in '%s'!\n", fileName);
				}
            } 
            else if ((strcmp(kw, "jw2")==0) && (vertexType & N_VT_JW)) 
            {
                char *j0s = strtok(NULL, " \t\n");
                char *w0s = strtok(NULL, " \t\n");
                char *j1s = strtok(NULL, " \t\n");
                char *w1s = strtok(NULL, " \t\n");
                if (j0s && w0s && j1s && w1s) 
                {
                    int j0   = atoi(j0s);
                    float w0 = float(atof(w0s));
                    int j1   = atoi(j1s);
                    float w1 = float(atof(w1s));
                    this->vertexBuffer->JointWeight2(act_jw++, j0, w0, j1, w1);
                } 
                else 
                {
                    n_error("Broken 'jw2' line in '%s'!\n", fileName);
				}
            } 
            else if ((strcmp(kw, "jw3") == 0) && (vertexType & N_VT_JW)) 
            {
                char *j0s = strtok(NULL, " \t\n");
                char *w0s = strtok(NULL, " \t\n");
                char *j1s = strtok(NULL, " \t\n");
                char *w1s = strtok(NULL, " \t\n");
                char *j2s = strtok(NULL, " \t\n");
                char *w2s = strtok(NULL, " \t\n");
                if (j0s && w0s && j1s && w1s && j2s && w2s) 
                {
                    int j0   = atoi(j0s);
                    float w0 = float(atof(w0s));
                    int j1   = atoi(j1s);
                    float w1 = float(atof(w1s));
                    int j2   = atoi(j2s);
                    float w2 = float(atof(w2s));
                    this->vertexBuffer->JointWeight3(act_jw++, j0, w0, j1, w1, j2, w2);
                } 
                else 
                {
                    n_error("Broken 'jw3' line in '%s'!\n", fileName);
				}
            } 
            else if ((strcmp(kw, "jw4") == 0) && (vertexType & N_VT_JW)) 
            {
                char *j0s = strtok(NULL, " \t\n");
                char *w0s = strtok(NULL, " \t\n");
                char *j1s = strtok(NULL, " \t\n");
                char *w1s = strtok(NULL, " \t\n");
                char *j2s = strtok(NULL, " \t\n");
                char *w2s = strtok(NULL, " \t\n");
                char *j3s = strtok(NULL, " \t\n");
                char *w3s = strtok(NULL, " \t\n");
                if (j0s && w0s && j1s && w1s && j2s && w2s && j3s && w3s) 
                {
                    int j0   = atoi(j0s);
                    float w0 = float(atof(w0s));
                    int j1   = atoi(j1s);
                    float w1 = float(atof(w1s));
                    int j2   = atoi(j2s);
                    float w2 = float(atof(w2s));
                    int j3   = atoi(j3s);
                    float w3 = float(atof(w3s));
                    this->vertexBuffer->JointWeight4(act_jw++, j0, w0, j1, w1, j2, w2, j3, w3);
                } 
                else 
                {
                    n_error("Broken 'jw4' line in '%s'!\n", fileName);
				}
            } 
            else if (strcmp(kw, "we")==0)
            {
                // a winged edge definition
                char* v0s  = strtok(NULL, " \t\n");
                char* v1s  = strtok(NULL, " \t\n");
                char* vp0s = strtok(NULL, " \t\n"); 
                char* vp1s = strtok(NULL, " \t\n");
                if (v0s && v1s && vp0s && vp1s)
                {
                    int v0  = atoi(v0s);
                    int v1  = atoi(v1s);
                    int vp0 = atoi(vp0s);
                    int vp1 = atoi(vp1s);
                    nWingedEdge we(v0, v1, vp0, vp1);
                    this->shadowCaster->Edge(act_we++, we);
                }
                else
                {
                    n_error("Broken 'we' line '%s'!\n", fileName);
                }
            }
            else if (strcmp(kw,"f")==0) 
            {
				for (i=0; i<3; i++) 
                {
					char *tmp = strtok(NULL," \t\n");
					if (tmp) 
                    {
						char *slash;
						if ((slash=strchr(tmp,'/'))) *slash=0;
						indices[num_indices++] = atoi(tmp)-1;
					}
				}
            }
        }
	}

    // create the trilist in the index buffer
    this->createTriList(indices, num_indices);

    // clean up and exit
    this->vertexBuffer->UnlockVertices();
    if (this->shadowCaster)
    {
        this->shadowCaster->Unlock();
    }
    this->vertexBuffer->SetBBox(bbox);
    n_free(indices);
    file->Close();
    delete file;
    return true;
}
//------------------------------------------------------------------------------








