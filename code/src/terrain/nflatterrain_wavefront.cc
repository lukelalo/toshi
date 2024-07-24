#define N_IMPLEMENTS nFlatTerrainNode
//-------------------------------------------------------------------
//  nflatterrain_wavefront.cc
//  (C) 2000 RadonLabs GmbH -- A.Weissflog
//-------------------------------------------------------------------
//
// -- modded nTerrainNode to render flat terrain, ripped directly from 
//    A.Weissflog.  All terrain generation credits go to Ulrich Thatcher.
//						7/29/00 Ken Melms
//
#include "terrain/nflatterrainnode.h"

//-------------------------------------------------------------------
/**
    @brief Save quadtree geometry to a Wavefront file.

    20-Jul-00   floh    created
*/
//-------------------------------------------------------------------
bool nFlatTerrainNode::SaveToWavefront(const char *fn)
{
    n_assert(fn);
    n_assert(NULL == this->fp);

    // make sure everything is loaded...
    if (this->map_dirty) this->load_map();
    n_assert(this->root_node);

    // write the baby...
    this->fp = ks->GetFileServer2()->NewFileObject();
    if (this->fp->Open(fn, "w"))
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
        return true;
    }
    n_delete this->fp;
    return false;
}

//-------------------------------------------------------------------
/**
    20-Jul-00   floh    created
*/
//-------------------------------------------------------------------
void nFlatTerrainNode::BeginRenderToWF(void)
{
    this->num_tris = 0;
}

//-------------------------------------------------------------------
/**
    20-Jul-00   floh    created
*/
//-------------------------------------------------------------------
void nFlatTerrainNode::AddTriangleToWF(flatquadsquare * /* usused qs */, 
                                   nFlatTriangleCorner *tc0, 
                                   nFlatTriangleCorner *tc1,
                                   nFlatTriangleCorner *tc2)
{
    n_assert(this->fp);
    nFlatTriangleCorner *tc_array[3];
    tc_array[0] = tc0;
    tc_array[1] = tc1;
    tc_array[2] = tc2;
    float r = this->vertex_radius;
    int i;
    char buf[N_MAXPATH];
    for (i=0; i<3; i++) {
        nFlatTriangleCorner *tc = tc_array[i];
        vector3 p = vector3(float(tc->x),float(tc->z),tc->vi->y);
        p *= r;
        sprintf(buf,"v %f %f %f\n",p.x,p.y,p.z);
        fp->PutS(buf);
    }
    this->num_tris++;
}

//-------------------------------------------------------------------
/**
    20-Jul-00   floh    created
*/
//-------------------------------------------------------------------
void nFlatTerrainNode::EndRenderToWF(void)
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
