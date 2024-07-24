//-------------------------------------------------------------------
//  genmousevents.cc
//  Mini-Helper-Routine fuer die Generierung der 5 Mouse-Events
//  aus der aktuellen und der letzten Mouse-Position.
//  Wird von den Window-Message-Handlern der Gfx-Server benutzt.
//  (C) 1999 A.Weissflog
//-------------------------------------------------------------------
#include "input/ninputserver.h"

//-------------------------------------------------------------------
//  gen_mouse_events()
//  Generiert Achsen-Events fuer die akkumulierte Mousebewegung
//  des letzten Frames. Es werden 4 Events generiert fuer
//  die negative/positive x/y-Achse.
//
//  Side effects:
//  old_x = cur_x
//  old_y = cur_y
//
//  23-Jul-99   floh    created
//-------------------------------------------------------------------
void gen_mouse_events(nInputServer *is, short cur_x, short cur_y, short& old_x, short& old_y)
{
    #define N_MOUSE_RES (25.0f)
        
    short rel_x,rel_y;
    nInputEvent *ie;
    if (old_x == 0) rel_x = 0;
    else            rel_x = cur_x - old_x;
    if (old_y == 0) rel_y = 0;
    else            rel_y = cur_y - old_y;
    old_x = cur_x;
    old_y = cur_y;
    float fx = ((float)rel_x) / N_MOUSE_RES;
    float fy = ((float)rel_y) / N_MOUSE_RES;

    // Achsen-Events
    ie = is->NewEvent();
    if (ie) {
        ie->type     = N_INPUT_AXIS_MOVE;
        ie->dev_id   = N_INPUT_MOUSE(0);
        ie->axis     = 0;     // -x
        ie->axis_val = (fx < 0) ? ((float)-fx) : 0.0f;
        is->LinkEvent(ie);
    }
    ie = is->NewEvent();
    if (ie) {
        ie->type     = N_INPUT_AXIS_MOVE;
        ie->dev_id   = N_INPUT_MOUSE(0);
        ie->axis     = 1;     // +x
        ie->axis_val = (fx > 0) ? ((float)+fx) : 0.0f;
        is->LinkEvent(ie);
    }
    ie = is->NewEvent();
    if (ie) {
        ie->type     = N_INPUT_AXIS_MOVE;
        ie->dev_id   = N_INPUT_MOUSE(0);
        ie->axis     = 2;     // -y
        ie->axis_val = (fy < 0) ? ((float)-fy) : 0.0f;
        is->LinkEvent(ie);
    }
    ie = is->NewEvent();
    if (ie) {
        ie->type     = N_INPUT_AXIS_MOVE;
        ie->dev_id   = N_INPUT_MOUSE(0);
        ie->axis     = 3;     // +y
        ie->axis_val = (fy > 0) ? ((float)+fy) : 0.0f;
        is->LinkEvent(ie);
    }
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------

