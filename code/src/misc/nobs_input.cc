#define N_IMPLEMENTS nObserver
//-------------------------------------------------------------------
//  nobs_input.cc
//  (C) 1999 A.Weissflog
//-------------------------------------------------------------------
#include "node/n3dnode.h"
#include "input/ninputserver.h"
#include "misc/nobserver.h"

//-------------------------------------------------------------------
/**
    @brief Update the values for mouse_act_x, mouse_act_y, mouse_rel_x,
    mouse_rel_y.

    - 26-Jul-99   floh    created
*/
//-------------------------------------------------------------------
void nObserver::get_mouse_input(nInputServer *is)
{
    nInputEvent *ie;
    if ((ie = is->FirstEvent())) do {
        if (ie->GetType() == N_INPUT_MOUSE_MOVE) {
            mouse_cur_x = ie->GetAbsXPos();
            mouse_cur_y = ie->GetAbsYPos();
        }
    } while ((ie = is->NextEvent(ie)));
    if (mouse_old_x != 0) mouse_rel_x = mouse_cur_x - mouse_old_x;
    if (mouse_old_y != 0) mouse_rel_y = mouse_cur_y - mouse_old_y;
    mouse_old_x = mouse_cur_x;
    mouse_old_y = mouse_cur_y;
}

//-------------------------------------------------------------------
/**
    @brief Place and orient the camera relative to the lookat node,
    @c look.

    The rotation matrix is copied, and the position is placed along
    the Z-axis at distance @c dist.

    - 26-Jul-99   floh    created
*/
//-------------------------------------------------------------------
void nObserver::place_camera(matrix44& cam, matrix44& look, float dist)
{
    cam = look;
    cam.M41 = look.M41 + (look.M31 * dist);
    cam.M42 = look.M42 + (look.M32 * dist);
    cam.M43 = look.M43 + (look.M33 * dist);
}

//-------------------------------------------------------------------
/**
    - 21-Jul-99   floh    created
    - 30-Jul-99   floh    + Kanaele umbenannt: move -> pan, zoom -> dolly
                          + diverse Richtungen invertiert, damit identisch
                            mit Maya
*/
//-------------------------------------------------------------------
void nObserver::handle_input(void)
{
    nInputServer *is = this->ref_is.get();
    matrix44 cam,look,m;
    vector3 v;

    // Determine the camera movement
    bool orbit  = is->GetButton("orbit");
    bool pan    = is->GetButton("pan");
    bool dolly  = is->GetButton("dolly");

    this->get_mouse_input(is);    
    cam  = this->ref_camera->GetM();
    look = this->ref_lookat->GetM();

    // Determine the distance between camera and lookat.
    vector3 c(cam.pos_component());
    vector3 l(look.pos_component());
    vector3 d = c - l;
    float dist  = d.len();

    // Translation shifts only lookat, because the camera is
    // placed again anyway.
    if (pan) {
        v.set(0.0f,0.0f,0.0f);
        v.x = -((float)this->mouse_rel_x) * 0.025f;
        v.y = +((float)this->mouse_rel_y) * 0.025f;
        m.ident();
        m.translate(v);
        m.mult_simple(look);
        look = m;
    }

    // Orbiting, dazu wird ebenfalls nur Lookat rotiert,
    // und die Kamera erst spaeter ausgerichtet
    if (orbit) {
        v.set(0.0f,0.0f,0.0f);
        v.x = -((float)this->mouse_rel_y) * 0.005f;
        v.y = -((float)this->mouse_rel_x) * 0.005f;
        if (v.y != 0.0f) {
            m.ident();
            m.rotate_y(v.y);
            vector3 bu(look.M41,look.M42,look.M43);
            look.M41=0.0f; look.M42=0.0f; look.M43=0.0f;
            look.mult_simple(m);
            look.M41=bu.x; look.M42=bu.y; look.M43=bu.z;
        }
        if (v.x != 0.0f) {
            m.ident();
            m.rotate_x(v.x);
            m.mult_simple(look);
            look = m;
        }
    }

    // Zoom shot, vertical mouse zoomed.
    if (dolly) {
        v.set(0.0f,0.0f,0.0f);
        v.z = -((float)this->mouse_rel_y) * 0.025f;
        dist += v.z;
        if (dist < 0.01f) dist=0.01f;
    }

    // Place the camera...
    this->place_camera(cam,look,dist);

    // Write back the resulting matrices.
    this->ref_camera->M(cam);
    this->ref_lookat->M(look);
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------

