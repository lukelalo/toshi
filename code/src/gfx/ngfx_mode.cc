#define N_IMPLEMENTS nGfxServer
//--------------------------------------------------------------------
//  ngfx_mode.cc
//  Display-Mode-Descriptor-Auswertung...
//  (C) 1999 A.Weissflog
//--------------------------------------------------------------------
#include <stdlib.h>
#include <string.h>

#include "kernel/ntypes.h"
#include "gfx/ngfxserver.h"

//--------------------------------------------------------------------
//  getModePart()
//  Isoliert ein gesuchtes Keyword aus dem Mode-Descriptor-String
//  und returniert dessen Wert, oder NULL das Keyword nicht enthalten 
//  ist.
//  In:
//      desc    - der komplette Modedescriptor-String
//      key     - das gesuchte Keyword
//      buf     - Wert des Keywords (der Wert, der in Klammern
//                hinter dem Keyword steht)
//      buf_size - Groesse von 'buf' in Bytes
//  Out:
//      Pointer auf 'buf', oder NULL, wenn das Keyword nicht
//      existiert.
//
//  06-Jun-99   floh    created
//--------------------------------------------------------------------
const char *nGfxServer::getModeTag(const char *desc,
                                   const char *key,
                                   char *buf,
                                   int buf_size)
{
    char tmp[N_MAXPATH];
    char *p = tmp;
    char *frag;
    
    n_strncpy2(tmp,desc,sizeof(tmp));
    while ((frag = strtok(p,"-"))) {
        char *open_brace = strchr(frag,'(');
        char *close_brace = strchr(frag,')');
        char *arg = NULL;
        if (p) p=NULL;
        if (open_brace) {
            *open_brace=0;
            arg = open_brace+1;
        }
        if (close_brace) *close_brace=0;
        if (strcmp(key,frag)==0) {
            // Treffer...
            if (arg) n_strncpy2(buf,arg,buf_size);
            else     buf[0] = 0;
            return buf;
        }
    }
    return NULL;
}                        
        
//--------------------------------------------------------------------
//  SetDisplayMode()
//  Folgende ModeDescriptor-Komponenten sind erlaubt:
//  dev(name)       - Device-Descriptor (default='0')
//  type(win|full)  - Windowed-Mode (default=win)
//  w(width)        - Display-Breite (default=512)
//  h(height)       - Display-Hoehe (default=384)
//  bpp(bpp)        - Bit-Tiefe
//  06-Jun-99   floh    created
//  07-Jun-99   floh    + this->Mode wird ausgefuellt
//  10-Jan-01   floh    + support for zbuf and sbuf tags (zbuffer
//                        and stencil buffer)
//--------------------------------------------------------------------
bool nGfxServer::SetDisplayMode(const char *dmode)
{
    n_assert(dmode);
    const char *s;
    char buf[N_MAXPATH];
    
    // default settings...
    this->disp_w   = 512;
    this->disp_h   = 384;
    this->disp_bpp = 16;
    this->disp_zbufbits = 16;
    this->disp_sbufbits = 0;

    strcpy(this->disp_dev,"0");
    strcpy(this->disp_type,"win");
    
    // DisplayMode-String auseinandernehmen
    s = getModeTag(dmode,"dev",buf,sizeof(buf));
    if (s) n_strncpy2(this->disp_dev,buf,sizeof(this->disp_dev));
    s = getModeTag(dmode,"type",buf,sizeof(buf));
    if (s) n_strncpy2(this->disp_type,buf,sizeof(this->disp_type));
    s = getModeTag(dmode,"w",buf,sizeof(buf));
    if (s) this->disp_w = atoi(s);
    s = getModeTag(dmode,"h",buf,sizeof(buf));
    if (s) this->disp_h = atoi(s);
    s = getModeTag(dmode,"bpp",buf,sizeof(buf));
    if (s) this->disp_bpp = atoi(s);
    s = getModeTag(dmode,"zbuf",buf,sizeof(buf));
    if (s) this->disp_zbufbits = atoi(s);
    s = getModeTag(dmode,"sbuf",buf,sizeof(buf));
    if (s) this->disp_sbufbits = atoi(s);

    sprintf(this->disp_mode,"dev(%s)-type(%s)-w(%d)-h(%d)-bpp(%d)-zbuf(%d)-sbuf(%d)",
            this->disp_dev,this->disp_type,this->disp_w,
            this->disp_h, this->disp_bpp, this->disp_zbufbits, this->disp_sbufbits);
    return true;
}

//--------------------------------------------------------------------
//  GetDisplayMode();
//  24-Dec-99   floh    created
//--------------------------------------------------------------------
const char *nGfxServer::GetDisplayMode(void)
{
    return this->disp_mode;
}

//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------
