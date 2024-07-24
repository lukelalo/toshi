//------------------------------------------------------------------------------
//  wfterrain2.cc
//
//  Generate nTerrainNode directly from Wavefront file without going through
//  intermediate height map bmp file.
//
//  (C) 2001 RadonLabs GmbH 2001
//------------------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "terrain/nterrainnode.h"
#include "mathlib/triangle.h"
#include "tools/wfobject.h"
#include "tools/wftools.h"

#ifndef PI
#define PI 3.14159265f
#endif

static int NumFaces = 0;
static triangle *TriArray = NULL;
static float *HeightArray = NULL;
static float MaxRadius = 0.0f;

//------------------------------------------------------------------------------
/**
    Convert wavefront obj data to array of triangle objects, and get the
    maximum radius (times two).
*/
void 
genTriArray(wfObject& src)
{
    n_assert(TriArray == NULL);
    NumFaces = src.f_array.size();
    TriArray = new triangle[NumFaces];
    MaxRadius = 0.0f;
    vector<wfFace>::iterator f_src;
    int fi;
    for (fi=0,f_src=src.f_array.begin(); f_src!=src.f_array.end(); f_src++,fi++) 
    {
        vector3 v[3];
        int i;
        for (i=0; i<3; i++) {
            v[i] = src.v_array.at(f_src->points.at(i).v_index).v;
            float l = v[i].len();
            if (l > MaxRadius) MaxRadius = l;
        }
        TriArray[fi].set(v[0],v[1],v[2]);
    }

    // increase max radius minimally to avoid rounding errors
    // later on
    MaxRadius *= 2.0f;
    fprintf(stderr,"-> num faces:  %d\n", NumFaces);
    fprintf(stderr,"-> max radius: %f\n", MaxRadius);
}

//------------------------------------------------------------------------------
/**
    Generate a 2d array of height values by shooting rays into the triangle
    soup, and check where they intersect.
*/
void genHeightMap(int res)
{
    n_assert(HeightArray == NULL);
    HeightArray = new float[res*res];
    int i,j;
    float theta,rho;
    float dt = PI / float(res);
    float dr = (2.0f*PI) / float(res);

    fprintf(stderr,"-> generating height array...\n");
    for (i = 0, rho = 0.0f; i < res; i++, rho += dr) 
    {
        for (j = 0,theta = 0.0f; j < res; j++, theta += dt) 
        {
            // generate a bunch of rays from midpoint
            // outwards, in case one of them misses the other
            // ones will take over
            vector3 v0(0.0f,0.0f,0.0f);
            vector3 v[3];
            float sin_theta,cos_theta,sin_rho,cos_rho;
            float radius = MaxRadius;

            sin_theta = (float) sin(theta);
            cos_theta = (float) cos(theta);
            sin_rho   = (float) sin(rho);
            cos_rho   = (float) cos(rho);
            v[0].set(sin_theta * sin_rho * radius, cos_theta * radius, sin_theta * cos_rho * radius);
            
            sin_theta = (float) sin(theta-0.05f);
            cos_theta = (float) cos(theta-0.05f);
            sin_rho   = (float) sin(rho);
            cos_rho   = (float) cos(rho);
            v[1].set(sin_theta * sin_rho * radius, cos_theta * radius, sin_theta * cos_rho * radius);

            sin_theta = (float) sin(theta);
            cos_theta = (float) cos(theta);
            sin_rho   = (float) sin(rho-0.05f);
            cos_rho   = (float) cos(rho-0.05f);
            v[2].set(sin_theta * sin_rho * radius, cos_theta * radius, sin_theta * cos_rho * radius);

            // do intersections
            float avg_t = 0.0f;
            float div   = 0.0f;
            int k;
            for (k = 0; k < NumFaces; k++) 
            {
                int l;
                for (l = 0; l < 3; l++)
                {
                    line3 line(v[l], v0);
                    float t;
                    if (TriArray[k].intersect(line,t)) {
                        t = 1.0f - t;
                        avg_t += t;
                        div   += 1.0f;
                    }
                }
            }

            // make sure that at least one of the rays had an intersection and
            // compute averaged heigth
            n_assert(div >= 0.99f);
            avg_t /= div;

            // undo the scaling of MaxRadius
            avg_t *= 2.0f;
            if (avg_t < 0.0f) avg_t  = 0.01f;
            if (avg_t > 1.0f) avg_t  = 1.00f;
            HeightArray[i * res + j] = avg_t;
        }
        fprintf(stderr,".");
        fflush(stderr);
    }
    fprintf(stderr,"\n");
}

//------------------------------------------------------------------------------
/**
    Create a nTerrainNode object, initialize it's height array, and
    ask it to save an nqs file under a specified location.
*/
bool
genTerrainNode(
    nKernelServer* ks, 
    wfObject& src, 
    int res, 
    const char* nqsFilename,
    float staticError)
{
    n_assert(nqsFilename);
    
    // create height map of float values
    genTriArray(src);
    genHeightMap(res);

    // create and initialize terrain
    nTerrainNode* terrain = (nTerrainNode*) ks->New("nterrainnode", "/terrain");
    terrain->SetDetail(100.0f);
    terrain->SetStaticError(staticError);
    terrain->SetRadius(100.0f);
    
    // initialize terrain's height field
    terrain->SetFloatArrayHeightMap(res, res, HeightArray);

    bool result = terrain->SaveTree(nqsFilename);

    return result;
}

//------------------------------------------------------------------------------
/**
*/
int main(int argc, char* argv[])
{
    nKernelServer *ks = new nKernelServer;
    bool help;
    long retval = 0;
    int res;
    const char *fname;
    float staticError;

    fprintf(stderr,"-> wfterrain2\n");

    // check args
    help        = wf_getboolarg(argc, argv, "-help");
    res         = wf_getintarg(argc, argv, "-res", 128);
    fname       = wf_getstrarg(argc, argv, "-fname", "terrain.nqs");
    staticError = wf_getfloatarg(argc, argv, "-error", 7.0f); 
    if (help) {
        fprintf(stderr, "wfterrain2 [-help] [-res] [-fname]\n"
                        "(C) 2001 Radon Labs GmbH\n"
                        "Generate terrain nqs file from wavefront object.\n"
                        "-help  -- show this help\n"
                        "-res   -- sample resolution (default 128)\n"
                        "-error -- static error threshold to apply (default 7.0)\n"
                        "-fname -- filename of nqs file (default terrain.nqs)\n");
        return 0;
    }

    wfObject src;

    FILE *in, *out;
    if (!wf_openfiles(argc, argv, in, out)) {
        fprintf(stderr,"file open failed!\n");
        retval = 10; goto ende;
    }

    // load source object
    fprintf(stderr,"loading...\n");
    if (!src.load(in)) {
        fprintf(stderr,"Load failed!\n");
        retval = 10; goto ende;
    }

    // do it...
    genTerrainNode(ks, src, res, fname, staticError);

    wf_closefiles(in, out);

ende:
    if (TriArray) delete[] TriArray;
    if (HeightArray) delete[] HeightArray;
    fprintf(stderr, "<- wfterrain\n");
    delete ks;
    return retval;
}  
