//------------------------------------------------------------------------------
//  wfgrasstool.cc
//
//  Generate emitter mesh for grass renderer by analyzing a textured mesh
//  for specific colors.
//
//  (C) 2001 RadonLabs GmbH
//------------------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#include "kernel/nkernelserver.h"
#include "gfx/ntexture.h"
#include "gfx/npixelformat.h"
#include "tools/wfobject.h"
#include "tools/wftools.h"
#include "mathlib/triangle.h"

nKernelServer *ks;

char* pixMap  = 0;
int pixWidth  = 0;
int pixHeight = 0;

//------------------------------------------------------------------------------
/**
    Load a bmp texture and validate global variables pixMap, pixWidth and
    pixHeight.
*/
bool loadTexture(char *name, char *aname)
{
    nTexture *tex;
    nPixelFormat *pix_fmt;
    bool retval = false;

    tex = (nTexture *) ks->New("ntexture","/tex");
    if (tex) {
        if (tex->BeginLoad(name,aname)) {
            pix_fmt = new nPixelFormat(32,
                          0x000000ff,
                          0x0000ff00,
                          0x00ff0000,
                          0xff000000);
            pixWidth  = tex->bmp_pixels.GetWidth();
            pixHeight = tex->bmp_pixels.GetHeight();
            pixMap = (char *) malloc(4 * pixWidth * pixHeight);
            if (pixMap) {
                long l;
                pix_fmt->BeginConv(tex->pf);
                while ((l=tex->LoadLine()) != -1) {
                    char *addr = pixMap + l * 4 * pixWidth;
                    pix_fmt->Conv(tex->line, (uchar *)addr, pixWidth);
                }
                pix_fmt->EndConv();

                // und fertig...
                tex->EndLoad();
                retval = true;
            } else fprintf(stderr, "Could not allocate pixmap!\n");
        } else fprintf(stderr, "Could not load texture '%s/%s'!\n", name, aname ? aname:"<none>");
        tex->EndLoad();
        tex->Release();
    } else fprintf(stderr, "Could not create ntexture object!\n");

    if (!retval) {
        if (pixMap) free(pixMap);
        pixMap    = NULL;
        pixWidth  = 0;
        pixHeight = 0;
    }

    return retval;
}

//------------------------------------------------------------------------------
/**
    Lookup an unfiltered texel in texture map.
*/
void lookupTexel(float u, float v, unsigned char& r, unsigned char& g, unsigned char& b, unsigned char& a)
{
    int x,y,off;
    x = (int) (((float)pixWidth) * u);
    y = (int) (((float)pixHeight) * v);
    if      (x < 0)  x = 0;
    else if (x >= pixWidth) x = pixWidth-1;
    if      (y < 0)  y = 0;
    else if (y >= pixHeight) y = pixHeight-1;

    off = 4 * (x + pixWidth * y);
    r = pixMap[off];
    g = pixMap[off+1];
    b = pixMap[off+2];
    a = pixMap[off+3];
}

//------------------------------------------------------------------------------
/**
    Check if a color value is a trigger color.
*/
bool isTriggerColor(unsigned char r, unsigned char g, unsigned char b, unsigned char /*a*/)
{
    if ((r == 0) && (g == 255) && (b == 0))
    {
        return true;
    } else {
        return false;
    }
}

//------------------------------------------------------------------------------
/**
    Emit a vertex into the target wavefront object.
*/
void emitVertex(wfObject& tar, const vector3& v, int type)
{
    // the coord contains the 3d position
    wfCoord coord(v.x, v.y, v.z);

    // the grass type is encoded into the green color
    float gf = float(type) / 255.0f;
    vector4 color(0.0f, gf, 0.0f, 0.0f);

    tar.v_array.PushBack(coord);
    tar.c_array.PushBack(color);
}

//------------------------------------------------------------------------------
/**
    Return a pseudo random number between minVal and maxVal.
*/
float getrand(float minVal, float maxVal)
{
    float r = float(rand()) / float(RAND_MAX);
    float val = minVal + ((maxVal - minVal) * r);
    return val;
}

//------------------------------------------------------------------------------
/**
    Return a random integer number between 0 and 15 (including) for up to
    16 different grass types.
*/
int getgrasstype()
{
    return rand() % 16;
}

//------------------------------------------------------------------------------
/**
    Recursively subdivide a triangle by cutting triangle edges by half. 
    Do a texel lookup for the newly created points, if texel is one
    of the trigger colors, emit a new vertex into the target wfObject.
*/
void recurse(const triangle& xyz, 
             const triangle& uv, 
             wfObject& tar, 
             float minLen)
{
    float m0 = 0.5f + getrand(-0.2f, +0.2f);
    float m1 = 0.5f + getrand(-0.2f, +0.2f);
    float m2 = 0.5f + getrand(-0.2f, +0.2f);

    // get 3 new vertices
    vector3 v0 = xyz.point(0) + ((xyz.point(1) - xyz.point(0)) * m0);
    vector3 v1 = xyz.point(0) + ((xyz.point(2) - xyz.point(0)) * m1);
    vector3 v2 = xyz.point(1) + ((xyz.point(2) - xyz.point(1)) * m2);

    // get 3 new uv coords
    vector3 uv0 = uv.point(0) + ((uv.point(1) - uv.point(0)) * m0);
    vector3 uv1 = uv.point(0) + ((uv.point(2) - uv.point(0)) * m1);
    vector3 uv2 = uv.point(1) + ((uv.point(2) - uv.point(1)) * m2);

    // do a texel lookup for each new point
    unsigned char r,g,b,a;    
    lookupTexel(uv0.x, uv0.y, r, g, b, a);
    if (isTriggerColor(r,g,b,a))
    {
        emitVertex(tar, v0, getgrasstype());
    }
    lookupTexel(uv1.x, uv1.y, r, g, b, a);
    if (isTriggerColor(r,g,b,a))
    {
        emitVertex(tar, v1, getgrasstype());
    }
    lookupTexel(uv2.x, uv2.y, r, g, b, a);
    if (isTriggerColor(r,g,b,a))
    {
        emitVertex(tar, v2, getgrasstype());
    }

    // if edge len threshold not reached yet, recurse
    vector3 v10 = v1 - v0;
    vector3 v20 = v2 - v0;
    vector3 v21 = v2 - v1;

    float l10 = v10.len();
    float l20 = v20.len();
    float l21 = v21.len();

    if ((l10 > minLen) && (l20 > minLen) && (l21 > minLen))
    {
        triangle cxyz(v0, v1, v2);
        triangle cuv(uv0, uv1, uv2);
        recurse(cxyz, cuv, tar, minLen);

        cxyz.set(xyz.point(0), v0, v1);
        cuv.set(uv.point(0), uv0, uv1);
        recurse(cxyz, cuv, tar, minLen);

        cxyz.set(xyz.point(1), v0, v2);
        cuv.set(uv.point(1), uv0, uv2);
        recurse(cxyz, cuv, tar, minLen);

        cxyz.set(xyz.point(2), v1, v2);
        cuv.set(uv.point(2), uv1, uv2);
        recurse(cxyz, cuv, tar, minLen);
    }
}

//------------------------------------------------------------------------------
/**
    Check a triangle and recurse.
*/
void checkTriangle(const triangle& xyz, 
                   const triangle& uv, 
                   wfObject& tar, 
                   float minLen)
{
    // do a texel lookup for each triangle point
    unsigned char r,g,b,a;    
    lookupTexel(uv.point(0).x, uv.point(0).y, r, g, b, a);
    if (isTriggerColor(r,g,b,a))
    {
        emitVertex(tar, xyz.point(0), getgrasstype());
    }
    lookupTexel(uv.point(1).x, uv.point(1).y, r, g, b, a);
    if (isTriggerColor(r,g,b,a))
    {
        emitVertex(tar, xyz.point(1), getgrasstype());
    }
    lookupTexel(uv.point(2).x, uv.point(2).y, r, g, b, a);
    if (isTriggerColor(r,g,b,a))
    {
        emitVertex(tar, xyz.point(2), getgrasstype());
    }

    // recurse
    recurse(xyz, uv, tar, minLen);
}

//------------------------------------------------------------------------------
/**
    For each triangle, call checkTriangle.
*/
void doIt(wfObject& src, wfObject& tar, float minLen)
{
    srand((unsigned int) time(0));
    nArray<wfFace>::iterator fSrc;
    for (fSrc = src.f_array.Begin(); fSrc != src.f_array.End(); fSrc++)
    {
        const vector3& v0 = src.v_array.At(fSrc->points.At(0).v_index).v;
        const vector3& v1 = src.v_array.At(fSrc->points.At(1).v_index).v;
        const vector3& v2 = src.v_array.At(fSrc->points.At(2).v_index).v;

        const vector2& uv0 = src.vt_array.At(fSrc->points.At(0).vt_index);
        const vector2& uv1 = src.vt_array.At(fSrc->points.At(1).vt_index);
        const vector2& uv2 = src.vt_array.At(fSrc->points.At(2).vt_index);

        vector3 uvr0(uv0.x, uv0.y, 0.0f);
        vector3 uvr1(uv1.x, uv1.y, 0.0f);
        vector3 uvr2(uv2.x, uv2.y, 0.0f);

        triangle xyz(v0, v1, v2);
        triangle uv(uvr0, uvr1, uvr2);
        checkTriangle(xyz, uv, tar, minLen);
    }
}
    
//------------------------------------------------------------------------------
/**
    Main routine.
*/
int main(int argc, char *argv[])
{
    bool help;
    char *texStr, *alphaStr;
    long retval = 0;
    float minLen;

    ks = new nKernelServer;
    fprintf(stderr,"-> wfgrasstool\n");

    //  get args
    help     = wf_getboolarg(argc, argv, "-help");
    minLen   = wf_getfloatarg(argc, argv, "-minlen", 0.5f);
    texStr   = wf_getstrarg(argc, argv, "-tex", "texture.bmp");
    alphaStr = wf_getstrarg(argc, argv, "-alpha", NULL);

    // show help?
    if (help) {
        fprintf(stderr,"wfgrasstool [-help] [-minlen] [-tex] [-alpha]\n"
               "(C) 1998 Andre Weissflog\n"
               "Generate vertex color through texture lookup.\n"
               "-help:      show help and exit\n"
               "-minlen:    min edge len for subdivision (def=0.5)\n"
               "-tex:       texture filename (def = texture.bmp)\n"
               "-alpha:     optional alpha channel\n");
        return 0;
    }

    wfObject src;
    wfObject dst;

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

    // load texture
    fprintf(stderr,"loading texture...\n");
    if (!loadTexture(texStr, alphaStr)) {
        fprintf(stderr,"Load texture failed!\n");
        retval = 10; goto ende;
    }

    // generate vertex colors
    fprintf(stderr,"generating vertex colors...\n");
    doIt(src, dst, minLen);

    // save...
    fprintf(stderr,"saving...\n");
    dst.save(out);
    
    wf_closefiles(in, out);
    
ende:
    if (pixMap) free(pixMap);    
    fprintf(stderr,"<- wfgrasstool\n");
    return retval;
}  
//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------
