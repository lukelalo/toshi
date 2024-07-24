//-------------------------------------------------------------------
//  wfbuildgrid.cc
//  Generate Nomads build grid bitmap from island collision model.
//  (C) 2000 RadonLabs GmbH
//-------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "gfx/nbmpfile.h"
#include "mathlib/triangle.h"
#include "mathlib/bbox.h"
#include "tools/wfobject.h"
#include "tools/wftools.h"

#ifndef PI
#define PI 3.14159265f
#endif

enum nGridType
{
    VALID, OUTSIDE, TOOSTEEP
};

static int NumFaces = 0;
static int Width = 0;
static int Height = 0;
static bbox3 BBox; 
static triangle *TriArray = NULL;
static float *HeightArray = NULL;
static nGridType* typeArray = NULL;

//-------------------------------------------------------------------
//  alloc_arrays()
//  Get maximum x/z size of the island and initialize
//  global variables Width, Height, HeightArray.
//  09-Nov-00   floh    created
//-------------------------------------------------------------------
void alloc_arrays(wfObject& src, float grid_size)
{
    n_assert(NULL == HeightArray);

    // compute the mesh's bounding box
    BBox.begin_grow();
    vector<wfCoord>::iterator v_src;
    for (v_src=src.v_array.begin(); v_src!=src.v_array.end(); v_src++) {
        BBox.grow(v_src->v);
    }

    // the midpoint of the heightarray equals the midpoint of the island
    float x0 = n_abs(BBox.vmax.x);
    float x1 = n_abs(BBox.vmin.x);
    float y0 = n_abs(BBox.vmax.z);
    float y1 = n_abs(BBox.vmin.z);

    // adjust bounding box size to multiple of grid_size
    float adjustX0 = (float) ((floor(x0 / grid_size) + 1.0) * grid_size);
    float adjustX1 = (float) ((floor(x1 / grid_size) + 1.0) * grid_size);
    float adjustY0 = (float) ((floor(y0 / grid_size) + 1.0) * grid_size);
    float adjustY1 = (float) ((floor(y1 / grid_size) + 1.0) * grid_size);

    float xsize = 2.0f * ((adjustX0 > adjustX1) ? adjustX0 : adjustX1);
    float ysize = 2.0f * ((adjustY0 > adjustY1) ? adjustY0 : adjustY1);

    // get the required height array size
    Width  = int(xsize / grid_size);
    Height = int(ysize / grid_size);

    HeightArray = new float[Width*Height];
    typeArray   = new nGridType[Width*Height];

    // make sure that width and height are even
    n_assert(((Width>>1)<<1) == Width);
    n_assert(((Height>>1)<<1) == Height);
}

//-------------------------------------------------------------------
//  gen_tri_array()
//  Generate array of vector3 objects with triangle data from
//  wavefront file.
//  09-Nov-00   floh    created
//-------------------------------------------------------------------
void gen_tri_array(wfObject& src)
{
    n_assert(NULL == TriArray);

    fprintf(stderr,"-> generating triangles\n");

    NumFaces = src.f_array.size();
    TriArray = new triangle[NumFaces];
    vector<wfFace>::iterator f_src;
    int fi;
    for (fi=0,f_src=src.f_array.begin(); f_src!=src.f_array.end(); f_src++,fi++) {
        vector3 v[3];
        int i;
        for (i=0; i<3; i++) {
            v[i] = src.v_array.at(f_src->points.at(i).v_index).v;
        }
        TriArray[fi].set(v[0],v[1],v[2]);
    }
}

//-------------------------------------------------------------------
//  gen_height_array()
//  Shoot grid of vertical rays on triangle array, and record 
//  intersection positions in height array.
//  09-Nov-00   floh    created
//-------------------------------------------------------------------
void gen_height_array(float grid_size, float max_h_diff)
{
    fprintf(stderr,"-> doing intersection checks...\n");

    // compute the borders of the range we have to check
    float x0 = float(Width>>1)  * -grid_size;
    float z0 = float(Height>>1) * -grid_size;

    // we do multiple intersections per height array element
    // and compute the average of all checks per height element
    // as well as determine whether the terrain at that position
    // is too steep to build there
    const int res = 4;   
    float advance = grid_size / float(res);
    float sub_array[res+1][res+1];

    int ix,iz;
    for (iz=0; iz<Height; iz++) 
    {
        for (ix=0; ix<Width; ix++) 
        {
            int ixx,izz;
            float cur_z = z0 + float(iz) * grid_size;

            for (izz=0; izz<=res; izz++, cur_z+=advance) 
            {
                float cur_x = x0 + float(ix) * grid_size;
                for (ixx=0; ixx<=res; ixx++, cur_x+=advance) 
                {

                    // shoot ray at current x and z position into island
                    vector3 v0(cur_x,  127.0f, cur_z);
                    vector3 v1(cur_x, -128.0f, cur_z);

                    line3 l(v0,v1);
                    float max_t = 0.0f;
                    bool has_intersected = false;
                    int k;
                    for (k=0; k<NumFaces; k++) 
                    {
                        float t;
                        if (TriArray[k].intersect(l,t)) 
                        {
                            t = 1.0f - t;
                            if (t > max_t) 
                            {
                                max_t = t;
                            }
                            has_intersected = true;
                        }
                    }
                    if (has_intersected) {
                        sub_array[ixx][izz] = max_t;
                    }
                    else
                    {
                        sub_array[ixx][izz] = -1.0f;
                    }
                }
            }

            // validate subgrid:
            //  - if there is at least one non-intersection, the whole area
            //    becomes invalid
            //  - if the height difference is greater then max_height,
            //    the whole area becomes invalid
            //  - otherwise, the resulting height is the average of
            //    all subheights
            bool is_valid = true;
            bool is_toosteep = false;
            float min_h = +2.0f;
            float max_h = -2.0f;
            int num_outside = 0;
            for (ixx=0; ixx<=res; ixx++) 
            {
                for (izz=0; izz<=res; izz++) 
                {
                    float h = sub_array[ixx][izz];
                    if (h < -0.1f) 
                    {
                        // no intersection at this position, increment
                        // "outside" counter 
                        num_outside++;
                    } 
                    else 
                    {
                        // valid intersection position, update minimal
                        // and maximal and average_height
                        if (h < min_h) 
                        {
                            min_h = h;
                        }
                        if (h > max_h) 
                        {
                            max_h = h;
                        }
                    }
                }
            }

            // too many "outside" hits, invalidate grid position
            if (num_outside > 2)
            {
                is_valid    = false;
                is_toosteep = false;
            }
            
            // see if (max_h-min_h) is greater then allowed height difference
            if (is_valid && ((max_h-min_h) > max_h_diff)) 
            {
                is_valid    = false;
                is_toosteep = true;
            }

            // if valid, fill HeightArray element with average height
            if (is_valid) 
            {
                HeightArray[iz*Width + ix] = min_h;
                typeArray[iz*Width + ix]   = VALID;
                fprintf(stderr, "o");
            } 
            else if (is_toosteep) 
            {
                HeightArray[iz*Width + ix] = min_h;
                typeArray[iz*Width + ix]   = TOOSTEEP;
               fprintf(stderr, "x");
            } 
            else 
            {
                HeightArray[iz*Width + ix] = min_h;
                typeArray[iz*Width + ix]   = OUTSIDE;
                fprintf(stderr, ".");
            }
        }
        fprintf(stderr, "\n");
    }
}

//-------------------------------------------------------------------
//  gen_bmp_file()
//  09-Nov-00   floh    created
//-------------------------------------------------------------------
void gen_bmp_file(const char *fname)
{
    n_assert(fname);

    fprintf(stderr, "-> writing bmp file...\n");

    nBmpFile bmp;
    bmp.SetWidth(Width);
    bmp.SetHeight(Height);

    if (bmp.Open(fname,"wb")) {
        int buf_size = Width*3+4;
        uchar *buf = new uchar[buf_size];
        int x,y;
        for (y=0; y<Height; y++) {
            float *heights = &(HeightArray[Width*y]);
            nGridType* types = &(typeArray[Width*y]);
            for (x=0; x<Width; x++) {

                // compute height as ushort
                ushort height16 = (ushort) (heights[x] * ((1<<16)-1)); 

                switch (types[x])
                {
                    case TOOSTEEP:

                        // too steep: blue == 128
                        buf[x*3+0] = 128;   
                        buf[x*3+1] = (uchar) (height16 >> 8);
                        buf[x*3+2] = (uchar) (height16 & 0xff);
                        break;
                        
                    case VALID:

                        // valid position: blue == 0
                        buf[x*3+0] = 0;
                        buf[x*3+1] = (uchar) (height16 >> 8);
                        buf[x*3+2] = (uchar) (height16 & 0xff);
                        break;

                    case OUTSIDE:
                        // not on island: blue == 255
                        buf[x*3+0] = 255;
                        buf[x*3+1] = 0;
                        buf[x*3+2] = 0;
                        break;
                }
            }
            bmp.WriteLine(buf);
        }
        delete buf;
        bmp.Close();
    }
}

//-------------------------------------------------------------------
//  main()
//  09-Nov-00   floh    created
//-------------------------------------------------------------------
int main(int argc, char *argv[]) {
    nKernelServer *ks = new nKernelServer;
    bool help;
    long retval = 0;
    float grid_size;
    float max_height_diff;
    const char *fname;

    fprintf(stderr,"-> wfbuildgrid\n");

    // check args
    help            = wf_getboolarg(argc, argv, "-help");
    grid_size       = wf_getfloatarg(argc, argv, "-gridsize", 5.0f);
    max_height_diff = wf_getfloatarg(argc, argv, "-maxdiff", 1.0f);
    fname = wf_getstrarg(argc,argv, "-fname", "bgrid.bmp");
    if (help) {
        fprintf(stderr, "wfbuildgrid [-help] [-gridsize] [-maxdiff] [-fname]\n"
                        "(C) 2000 Andre Weissflog\n"
                        "Generate height map for spherical terrain renderer from 3d model.\n"
                        "-help     -- show help\n"
                        "-gridsize -- size of grid element in meters (def 5.0)\n"
                        "-maxdiff  -- max height difference in grid element in meters (def 1.0)\n"
                        "-fname    -- filename of output file (24 bpp BMP, def bgrid.bmp\n");
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

    // do stuff...
    alloc_arrays(src, grid_size);
    gen_tri_array(src);
    gen_height_array(grid_size, (max_height_diff/256.0f));
    gen_bmp_file(fname);

    wf_closefiles(in, out);

ende:
    if (TriArray) delete[] TriArray;
    if (HeightArray) delete[] HeightArray;
    fprintf(stderr,"<- wfbuildgrid\n");
    delete ks;
    return retval;
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
