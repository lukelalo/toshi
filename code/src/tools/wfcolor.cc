//-------------------------------------------------------------------
//  wfcolor [-help] [-tex]
//
//  Wirft Textur-Koordinaten und Normalen weg, und generiert
//  dafuer Eckpunktfarben. Welche Eckpunktfarben das sind,
//  wird durch einen Textur-Lookup ermittelt.
//
//  (C) 1999 A.Weissflog
//-------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "kernel/nkernelserver.h"
#include "gfx/ntexture.h"
#include "gfx/npixelformat.h"
#include "tools/wfobject.h"
#include "tools/wftools.h"

nKernelServer *ks;

//-------------------------------------------------------------------
//  load_texture()
//  02-Feb-99   floh    created
//-------------------------------------------------------------------
bool load_texture(char *name, char *aname,
                  char *& pix_map,
                  int& w,
                  int& h)
{
    nTexture *tex;
    nPixelFormat *pix_fmt;
    bool retval = false;

    pix_map = NULL;
    w = 0;
    h = 0;
    tex = (nTexture *) ks->New("ntexture","/tex");
    if (tex) {
        if (tex->BeginLoad(name,aname)) {
            pix_fmt = new nPixelFormat(32,
                          0x000000ff,
                          0x0000ff00,
                          0x00ff0000,
                          0xff000000);
            w = tex->bmp_pixels.GetWidth();
            h = tex->bmp_pixels.GetHeight();
            pix_map = (char *) malloc(4*w*h);
            if (pix_map) {
                long l;
                pix_fmt->BeginConv(tex->pf);
                while ((l=tex->LoadLine()) != -1) {
                    char *addr = pix_map + l*4*w;
                    pix_fmt->Conv(tex->line,(uchar *)addr,w);
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
        if (pix_map) free(pix_map);
        pix_map = NULL;
        w = 0;
        h = 0;
    }

    return retval;
}
//-------------------------------------------------------------------
void lookup_texel(char *pix_map,
                  int w, int h,
                  float u, float v,
                  float& r, float& g, float& b, float& a)
{
    int x,y,off;
    unsigned char cr,cg,cb,ca;
    x = (int) (((float)w) * u);
    y = (int) (((float)h) * v);
    if      (x < 0)  x = 0;
    else if (x >= w) x = w-1;
    if      (y < 0)  y = 0;
    else if (y >= h) y = h-1;

    off = 4 * (x + w*y);
    cr = pix_map[off];
    cg = pix_map[off+1];
    cb = pix_map[off+2];
    ca = pix_map[off+3];

    r = ((float)cr) / 255.0f;
    g = ((float)cg) / 255.0f;
    b = ((float)cb) / 255.0f;
    a = ((float)ca) / 255.0f;
}

//-------------------------------------------------------------------
//  gen_colors()
//
//  Replace each uv coordinate by a vertex color which corresponds
//  the uv coordinate in the texture.
//
//  02-Feb-99   floh    created
//  25-Oct-00   floh    rewritten to wfObject
//-------------------------------------------------------------------
void gen_colors(wfObject& src,
                wfObject& dst,
                char *pix_map,
                int w, int h)
{
    // copy vertices, throw away normals and uv's
    dst.v_array = src.v_array;

    // for each vt...
    nArray<vector2>::iterator cur_vt;
    for (cur_vt=src.vt_array.Begin(); cur_vt!=src.vt_array.End(); cur_vt++) {
        // do the texture lookup
        float r,g,b,a;
        lookup_texel(pix_map,w,h,cur_vt->x,cur_vt->y,r,g,b,a);
        
        // append vertex color to destination 
        vector4 c(r,g,b,a);
        dst.c_array.PushBack(c);
    }

    // copy over the face normals, and replace texture indices 
    // by color indices
    nArray<wfFace>::iterator f_src;
    for (f_src=src.f_array.Begin(); f_src!=src.f_array.End(); f_src++) {
        wfFace f_dst;
        nArray<wfPoint>::iterator p_src;
        for (p_src=f_src->points.Begin(); p_src!=f_src->points.End(); p_src++) {
            wfPoint p_dst;
            p_dst.v_index = p_src->v_index;
            p_dst.c_index = p_src->vt_index;
            f_dst.points.PushBack(p_dst);
        }
        dst.f_array.PushBack(f_dst);
    }
}

//-------------------------------------------------------------------
int main(int argc, char *argv[])
{
    bool help;
    char *tex_str, *alpha_str;
    long retval = 0;
    char *pix_map = NULL;
    int w,h;

    ks = new nKernelServer;
    fprintf(stderr,"-> wfcolor\n");

    //  get args
    help      = wf_getboolarg(argc,argv,"-help");
    tex_str   = wf_getstrarg(argc,argv,"-tex","texture.bmp");
    alpha_str = wf_getstrarg(argc,argv,"-alpha",NULL);

    // show help?
    if (help) {
        fprintf(stderr,"wfcolorize [-help] [-tex] [-alpha]\n"
               "(C) 1998 Andre Weissflog\n"
               "Generate vertex color through texture lookup.\n"
               "-help:  show help and exit\n"
               "-tex:   texture filename (def = texture.bmp)\n"
               "-alpha: optional alpha channel\n");
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
    if (!load_texture(tex_str,alpha_str,pix_map,w,h)) {
        fprintf(stderr,"Load texture failed!\n");
        retval = 10; goto ende;
    }

    // generate vertex colors
    fprintf(stderr,"generating vertex colors...\n");
    gen_colors(src,dst,pix_map,w,h);

    // save...
    fprintf(stderr,"saving...\n");
    dst.save(out);
    
    wf_closefiles(in, out);
    
ende:
    if (pix_map) free(pix_map);    
    fprintf(stderr,"<- wfcolor\n");
    return retval;
}  
//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------

