#define N_IMPLEMENTS nGlServer
//-------------------------------------------------------------------
//  ngl_text.cc
//  GL text rendering functions.
//  (C) 1999 A.Weissflog
//-------------------------------------------------------------------
#include "gfx/nglserver.h"

//-------------------------------------------------------------------
//  BeginText()
//  21-Feb-99   floh    created
//  26-Mar-00   floh    Fog turned off...
//-------------------------------------------------------------------
bool nGlServer::BeginText(void)
{
    if (this->text_initialized) {
        this->in_begin_text = true;

        glPushAttrib(GL_ALL_ATTRIB_BITS);
        glDisable(GL_TEXTURE_2D); 
        glDisable(GL_BLEND);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_LIGHTING);
        glDisable(GL_ALPHA_TEST);
        glDisable(GL_FOG);
        glColor4f(1.0f, 1.0f, 0.0f, 1.0f); 

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        glFrustum(-1.0f,1.0f,-1.0f,1.0f,1.0f,10.0f);

        return true;
    }
    return false;
}

//-------------------------------------------------------------------
//  EndText()
//  21-Feb-99   floh    created
//-------------------------------------------------------------------
bool nGlServer::EndText(void)
{
    n_assert(this->in_begin_text);

    if (this->text_initialized) {
        // OpenGL iss schu geil...
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
        glPopAttrib();
    }    
    return true;
}

//-------------------------------------------------------------------
//  TextPos()
//  21-Feb-99   floh    created
//-------------------------------------------------------------------
void nGlServer::TextPos(float x, float y)
{
    n_assert(this->in_begin_text);
    this->text_x = x;
    this->text_y = y + 2.0f*(this->text_height / ((float)this->render_height));
}

//-------------------------------------------------------------------
//  Text()
//  21-Feb-99   floh    created
//-------------------------------------------------------------------
bool nGlServer::Text(const char *s)
{
    n_assert(this->in_begin_text);
    bool scanning = true;

    if (this->text_initialized) {
        glListBase(this->text_listbase);

        // FIXME! NICHT DBCS SAFE!!!
        char c;
        const char *s_start = s; 
        const char *str     = s;
        int num_chars = 0;
        glRasterPos3f(this->text_x*5.0f, -this->text_y*5.0f, -5.0f);
        while (scanning) {
            c = *str++;
            if (c < 32) {
                // ein Sonderzeichen, erstmal alles vorher anzeigen...
                if (num_chars > 0) glCallLists(num_chars, GL_UNSIGNED_BYTE, s_start);
                s_start   = str;
                num_chars = 0;
                switch (c) {
                    case 0:
                        scanning=false;
                        break;
                    case '\n':
                        if (this->render_height > 0) {
                            this->text_y += 2.0f*(this->text_height / ((float)this->render_height));
                            glRasterPos3f(this->text_x*5.0f, -this->text_y*5.0f, -5.0f);
                        }
                        break;
                }
            } else num_chars++;
        }
        glListBase(0);
    }
    return true;
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
        
