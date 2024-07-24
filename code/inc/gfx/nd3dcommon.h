#ifndef N_D3DCOMMON_H
#define N_D3DCOMMON_H
//-------------------------------------------------------------------
//  OVERVIEW
//  Common Direct3D related definitions and classes.
//
//  (C) 1999 A.Weissflog
//-------------------------------------------------------------------
#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

//-------------------------------------------------------------------
class nD3DTextNode : public nNode {
public:
    float x, y;
    const char *text;
    nD3DTextNode(float _x, float _y, const char *_text) {
        this->x = _x;
        this->y = _y;
        this->text = n_strdup(_text);
    };
    ~nD3DTextNode() {
        if (this->text) n_free((void *)this->text);
    };
};

//-------------------------------------------------------------------
#endif