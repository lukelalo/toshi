#ifndef N_LIGHTNODE_H
#define N_LIGHTNODE_H
//-------------------------------------------------------------------
/**
    @class nLightNode
    @ingroup NebulaVisnodeModule

    @brief Encapsulates lighting attributes.

    The nLightNode defines a source of light in a n3DNode
    hierarchy. The source of light does not possess its own
    position, but the "current position" is used, which
    is supplied by a superordinate n3DNode.

    ***WARNING***
    The nLightNode class replaces the n3DLight class.
*/
//-------------------------------------------------------------------
#ifndef N_VISNODE_H
#include "node/nvisnode.h"
#endif

#ifndef N_LIGHT_H
#include "gfx/nlight.h"
#endif

//--------------------------------------------------------------------
#undef N_DEFINES
#define N_DEFINES nLightNode
#include "kernel/ndefdllclass.h"

//--------------------------------------------------------------------
class nSceneGraph2;
class N_DLLCLASS nLightNode : public nVisNode {
protected:
    nLight light;    

public:
    static nKernelServer *kernelServer;

    nLightNode() 
    { };
    virtual bool SaveCmds(nPersistServer *);

    virtual bool Attach(nSceneGraph2 *);
    virtual void Compute(nSceneGraph2 *); 
    
    nLight *GetLightObject(void) 
    {
        return &(this->light);
    };
    void SetType(nLightType t) 
    {
        this->light.SetType(t);
    };
    void SetColor(float r, float g, float b, float a) 
    {
        this->light.SetColor(r,g,b,a);
    };
    void SetR(float r) 
    {
        this->light.color[0] = r;
    };
    void SetG(float g) 
    {
        this->light.color[1] = g;
    };
    void SetB(float b) 
    {
        this->light.color[2] = b;
    };
    void SetA(float a) 
    {
        this->light.color[3] = a;
    };
    void SetAttenuation(float c, float l, float q) 
    {
        this->light.SetAttenuation(c,l,q);
    };
    void SetSpot(float c, float e) 
    {
        this->light.SetSpot(c,e);
    };
    nLightType GetType(void) 
    {
        return this->light.GetType();
    };
    void GetColor(float& r, float& g, float& b, float& a) 
    {
        this->light.GetColor(r,g,b,a);
    };
    void GetAttenuation(float& c, float& l, float& q) 
    {
        this->light.GetAttenuation(c,l,q);
    };
    void GetSpot(float& c, float& e) 
    {
        this->light.GetSpot(c,e);
    };
    void SetCastShadows(bool b)
    {
        this->light.SetCastShadows(b);
    }
    bool GetCastShadows()
    {
        return this->light.GetCastShadows();
    }
};
//--------------------------------------------------------------------
#endif
      
