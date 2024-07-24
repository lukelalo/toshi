#ifndef N_MESHEFFECT_H
#define N_MESHEFFECT_H
//------------------------------------------------------------------------------
/**
    @class nMeshEffect
    @ingroup NebulaVisnodeModule

    @brief ...


    (C) 2001 RadonLabs GmbH
*/
#ifndef N_ANIMNODE_H
#include "node/nanimnode.h"
#endif

#ifndef N_DYNAUTOREF_H
#include "kernel/ndynautoref.h"
#endif

#ifndef N_DYNVERTEXBUFFER_H
#include "gfx/ndynvertexbuffer.h"
#endif

#undef N_DEFINES
#define N_DEFINES nMeshEffect
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class nMeshNode;
class nMeshEffect : public nAnimNode
{
public:
    /// constructor
    nMeshEffect();
    /// destructor
    virtual ~nMeshEffect();

    /// persistency
    virtual bool SaveCmds(nPersistServer* fileServer);
	/// attach to scene graph
	virtual bool Attach(nSceneGraph2* sceneGraph);
	/// update internal state and render
	virtual void Compute(nSceneGraph2* sceneGraph);

	/// set as read only mesh
	void SetReadOnly(bool b);
	/// get read only mesh state
	bool GetReadOnly();
    /// set source mesh node
    void SetMeshNode(const char* path);
    /// get source mesh node
    const char* GetMeshNode();
	/// set amplitude
	void SetAmplitude(float f);
	/// get amplitude
	float GetAmplitude();
	/// set frequency
	void SetFrequency(float f);
	/// get frequency
	float GetFrequency();
	/// set twinktime
	void SetTwinkTime(float f);
	/// get twinktime
	float GetTwinkTime();
	/// set twink
	void SetTwink(bool b);
	/// get twink
	bool GetTwink();

    static nClass* clazz;
    /// pointer to nKernelServer
    static nKernelServer* kernelServer;

	/// amplitude of mesheffect
	float amplitude;
	/// frequency of mesheffect
	float frequency;

	/// lasttime we twinked
	float lasttime;
	/// Twinktime
	float twinktime;
	/// are wa allowed to twink
	bool twink; 

protected:
    void render(nSceneGraph2* sceneGraph, nVertexBuffer* vbSrc, float time);
    
    nDynAutoRef<nMeshNode> refMesh;         // ref to source mesh
    nDynVertexBuffer dynVBuf;               // dynamic target vertex buffer 
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nMeshEffect::SetReadOnly(bool b)
{
    this->dynVBuf.SetReadOnly(b);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nMeshEffect::GetReadOnly()
{
    return this->dynVBuf.GetReadOnly();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMeshEffect::SetMeshNode(const char* path)
{
    n_assert(path);
    this->refMesh = path;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nMeshEffect::GetMeshNode()
{
    return this->refMesh.getname();
}


//------------------------------------------------------------------------------
/**
*/
inline
void
nMeshEffect::SetAmplitude(float f)
{
    this->amplitude = f;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nMeshEffect::GetAmplitude()
{
    return this->amplitude;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMeshEffect::SetFrequency(float f)
{
    this->frequency = f;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nMeshEffect::GetFrequency()
{
    return this->frequency;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMeshEffect::SetTwinkTime(float f)
{
    this->twinktime = f;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nMeshEffect::GetTwinkTime()
{
    return this->twinktime;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMeshEffect::SetTwink(bool b)
{
    this->twink = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nMeshEffect::GetTwink()
{
    return this->twink;
}

//------------------------------------------------------------------------------
#endif
