#define N_IMPLEMENTS nTerrainNode
//-------------------------------------------------------------------
//  nterrain_wavefront.cc
//  (C) 2000 RadonLabs GmbH -- A.Weissflog
//-------------------------------------------------------------------
#include "terrain/nterrainnode.h"

//-------------------------------------------------------------------
//  SaveToWavefront()
//  Save quadtree geometry to a Wavefront file.
//  20-Jul-00   floh    created
//-------------------------------------------------------------------
bool nTerrainNode::SaveToWavefront(const char *fn)
{
    n_assert(fn);
    n_assert(NULL == this->fp);

    // make sure everything is loaded...
    if (this->map_dirty) this->load_map();
    n_assert(this->root_node);

    // write the baby...
    this->fp = ks->GetFileServer2()->NewFileObject();
    if (fp->Open(fn, "w"))
    {
        // run the update functions a few times over the
        // tree...
        int i;
        vector3 vwr_loc(0.0f,0.0f,0.0f);
        for (i=0; i<10; i++) {
    	    this->root_node->Update(this,this->root_corner_data,vwr_loc,this->detail*0.25f);
        }
        this->root_node->RenderToWF(this,this->root_corner_data);
        fp->Close();
        n_delete fp;
        return true;
    }
    n_delete fp;
    return false;
}

//-------------------------------------------------------------------
//  BeginRenderToWF()
//  20-Jul-00   floh    created
//-------------------------------------------------------------------
void nTerrainNode::BeginRenderToWF(void)
{
    this->num_tris = 0;
}

//-------------------------------------------------------------------
//  AddTriangleToWF()
//  20-Jul-00   floh    created
//-------------------------------------------------------------------
void nTerrainNode::AddTriangleToWF(quadsquare *, VertInfo *v0, VertInfo *v1, VertInfo *v2)
{
    n_assert(this->fp);
    int i;
    VertInfo *v_array[3];
    v_array[0]=v0; v_array[1]=v1; v_array[2]=v2;
    char buf[N_MAXPATH];
    for (i=0; i<3; i++) {
        VertInfo *vi = v_array[i];
        vector3 p = vi->cart * this->vertex_radius;
        sprintf(buf,"v %f %f %f\n",p.x,p.y,p.z);
        fp->PutS(buf);
    }
    this->num_tris++;
}

//-------------------------------------------------------------------
//  EndRenderToWF()
//  20-Jul-00   floh    created
//-------------------------------------------------------------------
void nTerrainNode::EndRenderToWF(void)
{
    n_assert(this->fp);
    int num = this->num_tris * 3;
    int i;
    char buf[N_MAXPATH];
    for (i=1; i<=num; i+=3) {
        sprintf(buf,"f %d %d %d\n",i,i+1,i+2);
        fp->PutS(buf);
    }
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
