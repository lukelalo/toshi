#define N_IMPLEMENTS nGlServer
//-------------------------------------------------------------------
//  ngl_init.cc
//  (C) 1999 A.Weissflog
//-------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "kernel/nenv.h"
#include "gfx/nglserver.h"
#include "il/il.h"
#include "il/ilu.h"

extern char *nGlServer_version;

#ifdef __WIN32__
#define glCullParameterfvEXT this->procCullParameterfvEXT
#endif

//-------------------------------------------------------------------
//  hasExtension()
//  19-Jun-99   floh    aus "Using OpenGL Extensions"
//-------------------------------------------------------------------
bool nGlServer::hasExtension(const char *extName)
{
    char *p = (char *) glGetString(GL_EXTENSIONS);
    char *end;
    int extNameLen;
    extNameLen = strlen(extName);
    end = p + strlen(p);
    while (p < end) {
        int n = strcspn(p, " ");
        if ((extNameLen == n) && (strncmp(extName, p, n) == 0)) {
            return true;
        }
        p += (n + 1);
    }
    return false;
}

//-------------------------------------------------------------------
//  initExtensions()
//  Fragt Extensions ab und bereitet deren Benutzung vor.
//  19-Jun-99   floh    created
//  20-Jun-99   floh    + EXT_clip_volume_hint
//  21-Jun-99   floh    + EXT_cull_vertex
//  23-Aug-00   floh    - EXT_cull_vertex
//                      + ARB_multitexture
//                      + EXT_texture_env_combine
//  28-Aug-00   floh    + EXT_texture_env_add
//  08-Sep-00   floh    + NV_vertex_array_range
//  02-Oct-00   floh    + Matrox & 3Dfx workaround: multitexturing disabled
//  04-Oct-00   floh    + pixelshader display list generation workaround flag
//                      + EXT_blend_color
//  15-Dec-00   floh    + removed support for NV_vertex_array_range
//  22-Jul-01   floh    + EXT_texture_env_dot3
//  29-Aug-01   floh    + EXT_bgra
//-------------------------------------------------------------------
void nGlServer::initExtensions(void)
{
    // EXT_paletted_texture
    #ifdef GL_EXT_paletted_texture
    if (hasExtension("GL_EXT_paletted_texture")) {
        this->ext_paletted_texture = true;
        n_printf("using GL_EXT_paletted_texture\n");
        #ifdef __WIN32__
        this->procColorTableEXT = (PFNGLCOLORTABLEEXTPROC) wglGetProcAddress("glColorTableEXT");
        if (NULL == this->procColorTableEXT) {
            n_printf("failed to obtain proc address for glColorTableEXT()\n");
            this->ext_paletted_texture = false;
        }
        #endif
    } else {
        this->ext_paletted_texture = false;
        n_printf("no support for GL_EXT_paletted_texture\n");
    }
    #else
    #error "GL_EXT_paletted_texture not supported by build environment"
    #endif

    // EXT_compiled_vertex_array
    #ifdef GL_EXT_compiled_vertex_array
    if (hasExtension("GL_EXT_compiled_vertex_array")) {
        this->ext_compiled_vertex_array = true;
        n_printf("using GL_EXT_compiled_vertex_array\n");
        #ifdef __WIN32__
        this->procLockArraysEXT   = (PFNGLLOCKARRAYSEXTPROC)   wglGetProcAddress("glLockArraysEXT");
        this->procUnlockArraysEXT = (PFNGLUNLOCKARRAYSEXTPROC) wglGetProcAddress("glUnlockArraysEXT");
        if (NULL == this->procLockArraysEXT) {
            n_printf("failed to obtain proc address for glLockArraysEXT()\n");
            this->ext_compiled_vertex_array = false;
        }           
        if (NULL == this->procUnlockArraysEXT) {
            n_printf("failed to obtain proc address for glUnlockArraysEXT()\n");
            this->ext_compiled_vertex_array = false;
        }
        #endif
    } else {
        this->ext_compiled_vertex_array = false;
        n_printf("no support for GL_EXT_compiled_vertex_array\n");
    }
    #else
    #error "GL_EXT_compiled_vertex_array not supported by build environment"
    #endif

    // EXT_clip_volume_hint
    #ifdef GL_EXT_clip_volume_hint
    if (hasExtension("GL_EXT_clip_volume_hint")) {
        this->ext_clip_volume_hint = true;
        n_printf("using GL_EXT_clip_volume_hint\n");
    } else {
        this->ext_clip_volume_hint = false;
        n_printf("no support for GL_EXT_clip_volume_hint\n");
    }    
    #else
    #error "GL_EXT_clip_volume_hint not support by build environment"
    #endif

    // ARB_multitexture
    #ifdef GL_ARB_multitexture
    if (hasExtension("GL_ARB_multitexture")) {
        this->arb_multitexture = true;
        n_printf("using GL_ARB_multitexture\n");
        #ifdef __WIN32__
        this->procActiveTextureARB =       (PFNGLACTIVETEXTUREARBPROC) wglGetProcAddress("glActiveTextureARB");
        this->procClientActiveTextureARB = (PFNGLCLIENTACTIVETEXTUREARBPROC) wglGetProcAddress("glClientActiveTextureARB");
        if (NULL == this->procActiveTextureARB) {
            n_printf("failed to obtain proc address for glActiveTextureARB()\n");
            this->arb_multitexture = false;
        }
        if (NULL == this->procClientActiveTextureARB) {
            n_printf("failed to obtain proc address for glClientActiveTextureARB()\n");
            this->arb_multitexture = false;
        }
        #endif
    } else {
        this->arb_multitexture = false;
        n_printf("no support for GL_ARB_multitexture\n");
    }
    #else
    #error "GL_ARB_multitexture not supported by build environment"
    #endif

    // EXT_texture_env_combine
    #if defined(GL_EXT_texture_env_combine) || defined(GL_ARB_texture_env_combine)
    if (hasExtension("GL_EXT_texture_env_combine")) {
        this->ext_texture_env_combine = true;
        n_printf("using GL_EXT_texture_env_combine\n");
    } else if (hasExtension("GL_ARB_texture_env_combine")) {
        this->ext_texture_env_combine = true;
        n_printf("using GL_ARB_texture_env_combine\n");
    } else {
        this->ext_texture_env_combine = false;
        n_printf("no support for GL_EXT_texture_env_combine\n");
    }
    #else
    #error "GL_EXT_texture_env_combine not supported by build environment"
    #endif

    // EXT_texture_env_add
    #if defined(GL_EXT_texture_env_add) || defined(GL_ARB_texture_env_add)
    if (hasExtension("GL_EXT_texture_env_add")) {
        this->ext_texture_env_add = true;
        n_printf("using GL_EXT_texture_env_add\n");
    } else if (hasExtension("GL_ARB_texture_env_add")) {
        this->ext_texture_env_add = true;
        n_printf("using GL_ARB_texture_env_add\n");
    } else {
        this->ext_texture_env_add = false;
        n_printf("no support for GL_EXT_texture_env_add\n");
    }
    #else
    #error "GL_EXT_texture_env_add not supported by build environment"
    #endif

    // EXT_texture_env_dot3
    #if defined(GL_EXT_texture_env_dot3) || defined(GL_ARB_texture_env_dot3)
    if (hasExtension("GL_EXT_texture_env_dot3")) {
        this->ext_texture_env_dot3 = true;
        n_printf("using GL_EXT_texture_env_dot3\n");
    } else if (hasExtension("GL_ARB_texture_env_dot3")) {
        this->ext_texture_env_dot3 = true;
        n_printf("using GL_ARB_texture_env_dot3\n");
    } else {
        this->ext_texture_env_dot3 = false;
        n_printf("no support for GL_EXT_texture_env_dot3\n");
    }
    #else
    #error "GL_EXT_texture_env_dot3 not supported by build environment"
    #endif

    // EXT_blend_color
    #ifdef GL_EXT_blend_color
    if (hasExtension("GL_EXT_blend_color")) {
        this->ext_blend_color = true;
        n_printf("using GL_EXT_blend_color\n");
        #ifdef __WIN32__
        this->procBlendColorEXT = (PFNGLBLENDCOLOREXTPROC) wglGetProcAddress("glBlendColorEXT");
        #endif
    } else {
        this->ext_blend_color = false;
        n_printf("no support for GL_EXT_blend_color\n");
    }
    #else
    #error "GL_EXT_blend_color not supported by build environment"
    #endif

    // EXT_bgra
    #ifdef GL_EXT_bgra
    if (hasExtension("GL_EXT_bgra"))
    {
        this->ext_bgra = true;
        n_printf("using GL_EXT_bgra\n");
    }
    else
    {
        this->ext_bgra = false;
        n_printf("no support for GL_EXT_bgra\n");
    }
    #else
    #error "GL_EXT_bgra not support by build environment"
    #endif
}    

//-------------------------------------------------------------------
//  initGl()
//  Initialisiert frisch erzeugten GL-Kontext.
//  16-Nov-98   floh    created
//  13-May-99   floh    oops, hier war noch glNormalize() angeschaltet 
//  18-Jun-99   floh    + Alphatest
//  18-Jul-99   floh    + /sys/share/display Database cleaner initialisiert
//  23-Aug-00   floh    + query number of multitexture stages
//  08-Sep-00   floh    + initialize vertex pool managers
//  28-Sep-00   floh    + PushCwd()/PopCwd()
//  18-Mar-01   floh    + initialize OpenIL stuff
//-------------------------------------------------------------------
void nGlServer::initGl(void)
{
    const char *ext;
    char buf[N_MAXPATH];
    char c;
    int i;
    nEnv *env;
    
    // Versions-Info schreiben
    n_printf("gl_vendor:   %s\n", glGetString(GL_VENDOR));
    n_printf("gl_renderer: %s\n", glGetString(GL_RENDERER));
    n_printf("gl_version:  %s\n", glGetString(GL_VERSION));
    n_printf("gl_extensions:\n");
    ext = (const char *) glGetString(GL_EXTENSIONS);
    i = 0;
    do {
        c = *ext++;
        if ((c == ' ') || (c == 0)) {
            buf[i] = 0;
            n_printf("%s\n",buf);
            i = 0;
        } else buf[i++] = c;
    } while (c);

    // initialize extensions
    this->initExtensions();

    // query number of texture units
    #ifdef GL_ARB_multitexture
        if (this->arb_multitexture) {
            GLint num_tus;
            glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB,&num_tus);
            this->num_texture_units = num_tus;
        } else {
            this->num_texture_units = 1;
        }
    #else
        this->num_texture_units = 1;
    #endif
    n_printf("texture units: %d\n",this->num_texture_units);

    // fill display database
    kernelServer->PushCwd(this->ref_devdir.get());
    env = (nEnv *) kernelServer->New("nenv","name");
    env->SetS((const char *) glGetString(GL_RENDERER));
    env = (nEnv *) kernelServer->New("nenv","desc");
    env->SetS((const char *) glGetString(GL_VERSION));
    env = (nEnv *) kernelServer->New("nenv","extensions");
    env->SetS((const char *) glGetString(GL_EXTENSIONS));
    kernelServer->PopCwd();

    // set initial render states
    glClearColor(0.0f,0.0f,0.4f,0.0f);
    glColor4f(1.0f,1.0f,1.0f,1.0f);
    glEnable(GL_CULL_FACE);
    glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_NORMALIZE);
    glDisable(GL_FOG);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);

    // initialize IL and ILU
    ilInit();
    iluInit();
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
