#ifndef N_JOINT_H
#define N_JOINT_H
//------------------------------------------------------------------------------
/**
    @class nJoint
    @ingroup NebulaVisnodeModule

    @brief Implement a bone in a skeleton.

    (C) 2001 A.Weissflog
*/
#ifndef N_3DNODE_H
#include "node/n3dnode.h"
#endif

#ifndef N_MATRIX_H
#include "mathlib/matrix.h"
#endif

//------------------------------------------------------------------------------
#undef N_DEFINES
#define N_DEFINES nJoint
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
//  Steps to get the 'skinned' modelview vertex position:
//
//  c'  = inverse_relaxed_matrix * c
//  c0  = joint_matrix * c'
//  c1  = parent_joint_matrix * c'
//  c'' = c1 + ((c0-c1)*w)
//
//  c'' is in modelspace and goes to rendering
//------------------------------------------------------------------------------
class N_PUBLIC nJoint : public n3DNode {
public:
    /// constructor
    nJoint();
    /// persistency
    virtual bool SaveCmds(nPersistServer* fileServer);
    /// update internal state and set scene graph node transform
    virtual void Compute(nSceneGraph2*);

    /// set orientation as quaternion
    void OQxyzw(float x, float y, float z, float w);
    /// get orientation as quaternaion
    void GetOQ(float& x, float& y, float& z, float& w);
    /// set orientation as euler angles
    void Oxyz(float x, float y, float z);
    /// get orientation as euler angles
    void GetO(float& x, float& y, float& z);
    /// set default (normal) translation
    void Nxyz(float x, float y, float z);
    /// get default (normal) translation
    void GetN(float& x, float& y, float& z);
    /// evaluate relaxes state of joint
    void EvalRelaxedState(nJoint* parentJoint);
    /// evaluate current state of joint
    void EvalCurrentState(nJoint* parentJoint);

    static nClass *local_cl;
    static nKernelServer *ks;

protected:
    quaternion oq;      // alternative to 'o': orientation quaternion
    vector3 o;          // orientation angles
    vector3 n;          // normal translation

	matrix44 mo;        // the orientation matrix
	
    matrix44 rm;        // the relaxed matrix (constructed from o and n)
    matrix44 irm;       // inverted relaxed matrix
    
	matrix44 m;         // the resulting joint matrix
    matrix44 nirm;      // irm without translation (for normals)
    matrix44 nm;        // m without translation (for normal)

    matrix44 pm;
    matrix44 npm;

public:
	// the following matrices are only used by the nmeshcluster class
    matrix44 irm_times_m;   // inverse relaxed matrix times matrix
    matrix44 nirm_times_nm; // same as above without translation

private:
    void updateMatrix(nJoint *);
};

//------------------------------------------------------------------------------
/**
*/
inline
nJoint::nJoint()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    @param  x       x component of quaternion
    @param  y       y component of quaternion
    @param  z       z component of quaternion
    @param  w       w component of quaternion
*/
inline
void
nJoint::OQxyzw(float x, float y, float z, float w)
{
    set(MXDIRTY);
    set(USEQUAT);
    oq.set(x,y,z,w);
    mo.set(oq);
}

//------------------------------------------------------------------------------
/**
    @param  x       [out] x component of quaternion
    @param  y       [out] y component of quaternion
    @param  z       [out] z component of quaternion
    @param  w       [out] w component of quaternion
*/
inline
void
nJoint::GetOQ(float& x, float& y, float& z, float& w)
{
    x = oq.x;
    y = oq.y;
    z = oq.z;
    w = oq.w;
}

//------------------------------------------------------------------------------
/**
    Rotation order is as always x->y->z.   

    @param  x       x angle in degree 
    @param  y       y angle in degree
    @param  z       z angle in degree
*/
inline
void
nJoint::Oxyz(float x, float y, float z)
{
    set(MXDIRTY);
    unset(USEQUAT);
    o.set(n_deg2rad(x), n_deg2rad(y), n_deg2rad(z));
    mo.ident();
    mo.rotate_x(o.x);
    mo.rotate_y(o.y);
    mo.rotate_z(o.z);
}

//------------------------------------------------------------------------------
/**
    @param  x       [out] x angle in degree 
    @param  y       [out] y angle in degree
    @param  z       [out] z angle in degree
*/
inline
void
nJoint::GetO(float& x, float& y, float& z) 
{
    x = n_rad2deg(o.x);
    y = n_rad2deg(o.y);
    z = n_rad2deg(o.z);
}

//------------------------------------------------------------------------------
/**
    @param  x       x component of default (relaxed) translation
    @param  y       y component of default (relaxed) translation
    @param  z       z component of default (relaxed) translation
*/
inline
void
nJoint::Nxyz(float x, float y, float z)
{
    n.set(x,y,z);
}

//------------------------------------------------------------------------------
/**
    @param  x       [out] x component of default (relaxed) translation
    @param  y       [out] y component of default (relaxed) translation
    @param  z       [out] z component of default (relaxed) translation
*/
inline
void
nJoint::GetN(float& x, float& y, float& z)
{
    x = n.x;
    y = n.y;
    z = n.z;
}
//------------------------------------------------------------------------------
#endif
