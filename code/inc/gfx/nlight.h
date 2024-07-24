#ifndef N_LIGHT_H
#define N_LIGHT_H
//-------------------------------------------------------------------
/**
    @class nLight

    @brief holds light source attributes

    (C) 1999 A.Weissflog
*/
//-------------------------------------------------------------------
#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifndef N_GFXTYPES_H
#include "gfx/ngfxtypes.h"
#endif

#ifndef N_MATHLIB_H
#include "mathlib/matrix.h"
#endif

//-------------------------------------------------------------------
class nLight {
public:
    matrix44 modelView;
    nLightType type;
    float color[4];
    float att[3];
    float cutoff;
    float exponent;
    bool castShadows;

    nLight()
    {
        ulong i;
        this->type  = N_LIGHT_AMBIENT;
        for (i=0; i<4; i++) this->color[i] = 1.0f;
        this->att[0] = 0.0f;
        this->att[1] = 1.0f;
        this->att[2] = 0.0f;
        this->cutoff = 45.0f;
        this->exponent = 0.0f;
        this->castShadows = true;
    };
    void SetModelView(const matrix44& mv)
    {
        this->modelView = mv;
    };
    void SetType(nLightType t) 
    {
        this->type  = t;
    };
    void SetColor(float r, float g, float b, float a) 
    {
        this->color[0] = r;
        this->color[1] = g;
        this->color[2] = b;
        this->color[3] = a;
    };
    void SetAttenuation(float c, float l, float q) 
    {
        this->att[0] = c;
        this->att[1] = l;
        this->att[2] = q;
    };
    void SetSpot(float c, float e) 
    {
        this->cutoff   = c;
        this->exponent = e;
    };
    const matrix44& GetModelView() const
    {
        return this->modelView;
    };
    nLightType GetType(void) const
    {
        return this->type;
    };
    void GetColor(float& r, float& g, float& b, float& a) const
    {
        r = this->color[0];
        g = this->color[1];
        b = this->color[2];
        a = this->color[3];
    };
    void GetAttenuation(float& c, float& l, float& q) const
    {
        c = this->att[0];
        l = this->att[1];
        q = this->att[2];
    };
    void GetSpot(float& c, float& e) const
    {
        c = this->cutoff;
        e = this->exponent;
    };
    void SetCastShadows(bool b)
    {
        this->castShadows = b;
    };
    bool GetCastShadows() const
    {
        return this->castShadows;
    };
};
//-------------------------------------------------------------------
#endif
        
