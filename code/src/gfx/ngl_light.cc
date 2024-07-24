#define N_IMPLEMENTS nGlServer
//-------------------------------------------------------------------
//  ngl_light.cc -- Lighting-Zeuch...
//  (C) 1998 A.Weissflog
//-------------------------------------------------------------------
#include "gfx/nglserver.h"

//-------------------------------------------------------------------
//  SetLight()
//  03-Dec-98   floh    created
//  21-Jun-99   floh    Specular ist raus, Directionals haben
//                      keine Attenuation mehr
//-------------------------------------------------------------------
bool
nGlServer::SetLight(nLight& l)
{
    static float black[4]   = { 0.0f, 0.0f, 0.0f, 1.0f };
    static float def_dir[3] = { 0.0f, 0.0f, -1.0f };
    static float null_pos[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
    static float directional_nullpos[4] = { 0.0f, 0.0f, 1.0f, 0.0f };

    int index = this->GetNumLights();
    if (nGfxServer::SetLight(l))
    {
        // set the light's modelview matrix
        const float *glm = &(l.GetModelView().M11);
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(glm);

        GLenum gl_light = (GLenum) (GL_LIGHT0 + index);
        switch (l.type) {
            case N_LIGHT_AMBIENT:
                glLightModelfv(GL_LIGHT_MODEL_AMBIENT,(const float *)&(l.color));
                glDisable(gl_light);
                break;
        
            case N_LIGHT_POINT:
                glLightfv(gl_light,GL_AMBIENT,black);
                glLightfv(gl_light,GL_DIFFUSE,(const float *) l.color);
                glLightfv(gl_light,GL_SPECULAR,black);
                glLightfv(gl_light,GL_POSITION,(const float *) null_pos);
                glLightfv(gl_light,GL_SPOT_DIRECTION,(const float *) def_dir);
                glLightf(gl_light,GL_SPOT_CUTOFF,180.0f);   // kugelfoermig
                glLightf(gl_light,GL_SPOT_EXPONENT,0.0f);   // gleichfoermige Distribution
                glLightf(gl_light,GL_CONSTANT_ATTENUATION, l.att[0]);
                glLightf(gl_light,GL_LINEAR_ATTENUATION, l.att[1]);
                glLightf(gl_light,GL_QUADRATIC_ATTENUATION, l.att[2]);
                glEnable(gl_light);
                break;

            case N_LIGHT_SPOT:
                glLightfv(gl_light,GL_AMBIENT,black);
                glLightfv(gl_light,GL_DIFFUSE,(const float *) l.color);
                glLightfv(gl_light,GL_SPECULAR,black);
                glLightfv(gl_light,GL_POSITION,(const float *) null_pos);
                glLightfv(gl_light,GL_SPOT_DIRECTION,(const float *) def_dir);
                glLightf(gl_light,GL_SPOT_CUTOFF,l.cutoff);
                glLightf(gl_light,GL_SPOT_EXPONENT,l.exponent);
                glLightf(gl_light,GL_CONSTANT_ATTENUATION, l.att[0]);
                glLightf(gl_light,GL_LINEAR_ATTENUATION, l.att[1]);
                glLightf(gl_light,GL_QUADRATIC_ATTENUATION, l.att[2]);
                glEnable(gl_light);
                break;

            case N_LIGHT_DIRECTIONAL:
                glLightfv(gl_light,GL_AMBIENT,black);
                glLightfv(gl_light,GL_DIFFUSE,(const float *) l.color);
                glLightfv(gl_light,GL_SPECULAR,black);
                glLightfv(gl_light,GL_POSITION,(const float *) directional_nullpos);
                glLightfv(gl_light,GL_SPOT_DIRECTION,(const float *) def_dir);
                glLightf(gl_light,GL_SPOT_CUTOFF,180.0f);
                glLightf(gl_light,GL_SPOT_EXPONENT,0.0f);
                glLightf(gl_light,GL_CONSTANT_ATTENUATION,  1.0f);
                glLightf(gl_light,GL_LINEAR_ATTENUATION,    0.0f);
                glLightf(gl_light,GL_QUADRATIC_ATTENUATION, 0.0f);
                glEnable(gl_light);
                break;
        }
        return true;
    } 
    return false;
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
