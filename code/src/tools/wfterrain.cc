//-------------------------------------------------------------------
//  wfterrain.cc
//  Generate height map for spherical terrain renderer
//  from 3d model.
//  Depends on Nebula stuff.
//
//  26-Oct-00   floh    rewritten to wfObject
//
//  (C) 2000 A.Weissflog
//-------------------------------------------------------------------
#include <math.h>
#include "kernel/nkernelserver.h"
#include "gfx/nbmpfile.h"
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

//-------------------------------------------------------------------
//  gen_tri_array()
//  Generate array of vector3 objects with triangle data from
//  wavefront file. Also determine max radius.
//  07-Apr-00   floh    created
//  26-Oct-00   floh    rewritten to wfobject
//-------------------------------------------------------------------
void gen_tri_array(wfObject& src)
{
    n_assert(TriArray == NULL);
    NumFaces = src.f_array.size();
    TriArray = new triangle[NumFaces];
    MaxRadius = 0.0f;
    vector<wfFace>::iterator f_src;
    int fi;
    for (fi=0,f_src=src.f_array.begin(); f_src!=src.f_array.end(); f_src++,fi++) {
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

//-------------------------------------------------------------------
//  gen_height_array()
//  Generate spherical height array by shooting rays from a sphere
//  into the polygon soup and checking for the first intersection.
//  07-Apr-00   floh    created
//-------------------------------------------------------------------
void gen_height_array(int res)
{
    n_assert(HeightArray == NULL);
    HeightArray = new float[res*res];
    int i,j;
    float theta,rho;
    float dt = PI / float(res);
    float dr = (2.0f*PI) / float(res);

    fprintf(stderr,"-> generating height array...\n");
    for (i=0,rho=0.0f; i<res; i++, rho+=dr) {
        for (j=0,theta=0.0f; j<res; j++, theta+=dt) {
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
            v[0].set(sin_theta*sin_rho*radius, cos_theta*radius, sin_theta*cos_rho*radius);
            
            sin_theta = (float) sin(theta-0.005f);
            cos_theta = (float) cos(theta-0.005f);
            sin_rho   = (float) sin(rho);
            cos_rho   = (float) cos(rho);
            v[1].set(sin_theta*sin_rho*radius, cos_theta*radius, sin_theta*cos_rho*radius);

            sin_theta = (float) sin(theta);
            cos_theta = (float) cos(theta);
            sin_rho   = (float) sin(rho-0.005f);
            cos_rho   = (float) cos(rho-0.005f);
            v[2].set(sin_theta*sin_rho*radius, cos_theta*radius, sin_theta*cos_rho*radius);

            // check for intersection with triangle soup, remember maximum radius
            float max_t = 0.0f;
            int k;
            bool has_intersected = false;
            for (k=0; k<NumFaces; k++) {
                int l;
                for (l=0; l<3; l++) {
                    line3 line(v[l],v0);
                    float t;
                    if (TriArray[k].intersect(line,t)) {
                        t = 1.0f - t;
                        if (t>max_t) max_t=t;
                        has_intersected = true;
                    }
                }
            }
            if (!has_intersected) {
                fprintf(stderr,"NO INTERSECTION!\n");
            }

            HeightArray[i*res+j] = max_t * 2.0f;
        }
        fprintf(stderr,".");
        fflush(stderr);
    }
    fprintf(stderr,"\n");
}

//-------------------------------------------------------------------
//  gen_bitmap()
//  Generate bitmap from height array.
//  10-Apr-00   floh    created
//-------------------------------------------------------------------
bool gen_bitmap(const char *fname, int res)
{
    fprintf(stderr,"-> writing bmp file...\n");
    nBmpFile bmp;
    bmp.SetWidth(res);
    bmp.SetHeight(res);
    if (bmp.Open(fname,"wb")) {
        int buf_size = res*3+4;
        uchar *buf = new uchar[buf_size];
        int x,y;
        for (y=0; y<res; y++) {
            float *heights = &(HeightArray[res*y]);
            for (x=0; x<res; x++) {
                ushort height16 = (ushort) (heights[x] * ((1<<16)-1)); 
                buf[x*3+0] = 0;   
                buf[x*3+1] = (uchar) (height16 >> 8);
                buf[x*3+2] = (uchar) (height16 & 0xff);
            }
            bmp.WriteLine(buf);
        }
        delete buf;
        bmp.Close();
        return true;
    }
    return false;
}

//-------------------------------------------------------------------
int main(int argc, char *argv[]) {
    nKernelServer *ks = new nKernelServer;
    bool help;
    long retval = 0;
    int res;
    const char *fname;

    fprintf(stderr,"-> wfterrain\n");

    // check args
    help  = wf_getboolarg(argc, argv, "-help");
    res   = wf_getintarg(argc,argv, "-res", 128);
    fname = wf_getstrarg(argc,argv, "-fname", "hmap.bmp");
    if (help) {
        fprintf(stderr, "wfterrain [-help] [-res] [-fname]\n"
                        "(C) 2000 Andre Weissflog\n"
                        "Generate height map for spherical terrain renderer from 3d model.\n"
                        "-help  -- show help\n"
                        "-res   -- resolution of heightmap (def 128)\n"
                        "-fname -- filename of output file (24 bpp BMP, def hmap.bmp\n");
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

    // the objects triangle soup
    gen_tri_array(src);
    gen_height_array(res);
    gen_bitmap(fname,res);

    wf_closefiles(in, out);

ende:
    if (TriArray) delete[] TriArray;
    if (HeightArray) delete[] HeightArray;
    fprintf(stderr,"<- wfterrain\n");
    delete ks;
    return retval;
}  

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
