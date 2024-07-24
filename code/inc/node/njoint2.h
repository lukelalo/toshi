#ifndef N_JOINT2_H
#define N_JOINT2_H
//------------------------------------------------------------------------------
/**
    @class nJoint2
    @ingroup NebulaVisnodeModule

    @brief Alternative simpler joint class used by the Maya-Character-Exporter.

    (C) 2001 RadonLabs GmbH
*/
#ifndef N_3DNODE_H
#include "node/n3dnode.h"
#endif

#undef N_DEFINES
#define N_DEFINES nJoint2
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class nJoint2 : public n3DNode
{
public:
    /// constructor
    nJoint2();
    /// destructor
    virtual ~nJoint2();
    /// persistency
    virtual bool SaveCmds(nPersistServer* fileServer);
    /// compute current state
    virtual void Compute(nSceneGraph2*);

    /// set relaxed pose translation
    void OTxyz(float x, float y, float z);
    /// get relaxed pose translation
    void GetOT(float& x, float& y, float& z);
    /// set relaxed pose rotation
    void OQxyzw(float x, float y, float z, float w);
    /// get relaxed pose rotation
    void GetOQ(float& x, float& y, float& z, float& w);
    /// set relaxed pose scale
    void OSxyz(float x, float y, float z);
    /// get relaxed pose scale
    void GetOS(float& x, float& y, float& z);
    /// get the 4x4 skinning matrix
    const matrix44& GetSkinMatrix44();
    /// get the 3x3 skinning matrix
    const matrix33& GetSkinMatrix33();

    /// pointer to nKernelServer
    static nKernelServer* kernelServer;

private:
    /// update the inverse relaxed pose matrix
    void UpdatePoseMatrix();
        
    bool poseDirty;             // poseMatrices are dirty
    vector3 poseTranslate;      // relaxed pose local translation
    vector3 poseScale;          // relaxed pose local scale
    quaternion poseRotate;      // relaxed pose local quaternion
    matrix44 poseMatrix;        // relaxed pose matrix in model space of skin
    matrix44 invPoseMatrix;     // inverse relaxed pose matrix in model space of skin
    matrix44 skinLocalMatrix;   // the current 3d matrix in model space of skin
    matrix44 skinMatrix44;      // resulting skinning matrix for 3d coords
    matrix33 skinMatrix33;      // resulting skinning matrix for normals (no translate)
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nJoint2::OTxyz(float x, float y, float z)
{
    this->poseTranslate.set(x, y, z);
    poseDirty = true;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nJoint2::GetOT(float& x, float& y, float& z)
{
    x = this->poseTranslate.x;
    y = this->poseTranslate.y;
    z = this->poseTranslate.z;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nJoint2::OQxyzw(float x, float y, float z, float w)
{
    this->poseRotate.set(x, y, z, w);
    poseDirty = true;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nJoint2::GetOQ(float& x, float& y, float& z, float& w)
{
    x = this->poseRotate.x;
    y = this->poseRotate.y;
    z = this->poseRotate.z;
    w = this->poseRotate.w;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nJoint2::OSxyz(float x, float y, float z)
{
    this->poseScale.set(x, y, z);
    poseDirty = true;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nJoint2::GetOS(float& x, float& y, float& z)
{
    x = this->poseScale.x;
    y = this->poseScale.y;
    z = this->poseScale.z;
}

//------------------------------------------------------------------------------
/**
*/
inline
const matrix44&
nJoint2::GetSkinMatrix44()
{
    return this->skinMatrix44;
}

//------------------------------------------------------------------------------
/**
*/
inline
const matrix33&
nJoint2::GetSkinMatrix33()
{
    return this->skinMatrix33;
}

//------------------------------------------------------------------------------
#endif
