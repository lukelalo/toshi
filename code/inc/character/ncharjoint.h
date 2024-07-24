#ifndef N_CHARJOINT_H
#define N_CHARJOINT_H
//------------------------------------------------------------------------------
/**
    @class nCharJoint
    
    @brief A joint in a character skeleton.

    (C) 2002 RadonLabs GmbH
*/
#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifndef N_VECTOR_H
#include "mathlib/vector.h"
#endif

#ifndef N_MATRIX_H
#include "mathlib/matrix.h"
#endif

#ifndef N_QUATERNION_H
#include "mathlib/quaternion.h"
#endif

#ifndef N_STRING_H
#include "util/nstring.h"
#endif

//------------------------------------------------------------------------------
class nCharJoint
{
public:
    /// constructor
    nCharJoint();
    /// destructor
    ~nCharJoint();
    /// set name of joint
    void SetName(const char* n);
    /// get name of joint
    const char* GetName() const;
    /// set parent joint index
    void SetParentJointIndex(int index);
    /// get parent joint index
    int GetParentJointIndex() const;
    /// set parent joint pointer
    void SetParentJoint(nCharJoint* p);
    /// get the parent joint
    nCharJoint* GetParentJoint() const;
    /// set the pose translation
    void SetPose(const vector3& t, const quaternion& q);
    /// get pose translation
    const vector3& GetPoseTranslate() const;
    /// get pose rotate
    const quaternion& GetPoseRotate() const;
    /// set translation
    void SetTranslate(const vector3& t);
    /// get translation
    const vector3& GetTranslate() const;
    /// set rotation
    void SetRotate(const quaternion& q);
    /// get rotation
    const quaternion& GetRotate() const;
    /// evaluate joint
    void Evaluate();
    /// get the skinning matrix with translation
    const matrix44& GetSkinMatrix44() const;
    /// get the skinning matrix without translation (for normals)
    const matrix33& GetSkinMatrix33() const;
    /// return the joint's matrix in model space
    const matrix44& GetMatrix44() const;

private:
    vector3 poseTranslate;
    quaternion poseRotate;
    vector3 translate;
    quaternion rotate;
    matrix44 poseMatrix;
    matrix44 invPoseMatrix;
    matrix44 localMatrix;
    matrix44 skinMatrix44;
    matrix33 skinMatrix33;
    nString name;
    int parentJointIndex;
    nCharJoint* parentJoint;
};

//------------------------------------------------------------------------------
/**
*/
inline
nCharJoint::nCharJoint() :
    parentJoint(0),
    parentJointIndex(-1)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nCharJoint::~nCharJoint()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nCharJoint::SetName(const char* n)
{
    n_assert(n);
    this->name = n;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nCharJoint::GetName() const
{
    return (this->name.IsEmpty()) ? 0 : this->name.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nCharJoint::SetParentJoint(nCharJoint* p)
{
    this->parentJoint = p;
}

//------------------------------------------------------------------------------
/**
*/
inline
nCharJoint*
nCharJoint::GetParentJoint() const
{
    return this->parentJoint;
}

//------------------------------------------------------------------------------
/**
    The parent joint index can be -1 if this joint is the root joint.
*/
inline
void
nCharJoint::SetParentJointIndex(int index)
{
    this->parentJointIndex = index;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nCharJoint::GetParentJointIndex() const
{
    return this->parentJointIndex;
}

//------------------------------------------------------------------------------
/**
    The parent's pose matrix MUST be uptodate, so make sure you call
    parent's always before their children.

    19-Apr-02   floh    also initialize translation and rotation with pose
*/
inline
void
nCharJoint::SetPose(const vector3& t, const quaternion& q)
{
    this->poseTranslate = t;
    this->poseRotate = q;
    this->translate = t;
    this->rotate = q;

    this->poseMatrix.set(q);
    this->poseMatrix.translate(t);
    if (this->parentJoint)
    {
        this->poseMatrix.mult_simple(this->parentJoint->poseMatrix);
    }
    this->invPoseMatrix = this->poseMatrix;
    this->invPoseMatrix.invert();
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector3&
nCharJoint::GetPoseTranslate() const
{
    return this->poseTranslate;
}

//------------------------------------------------------------------------------
/**
*/
inline
const quaternion&
nCharJoint::GetPoseRotate() const
{
    return this->poseRotate;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nCharJoint::SetTranslate(const vector3& t)
{
    this->translate = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector3&
nCharJoint::GetTranslate() const
{
    return this->translate;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nCharJoint::SetRotate(const quaternion& q)
{
    this->rotate = q;
}

//------------------------------------------------------------------------------
/**
*/
inline
const quaternion&
nCharJoint::GetRotate() const
{
    return this->rotate;
}

//------------------------------------------------------------------------------
/**
    This computes the skinning matrix from the pose matrix, the translation
    and the rotation of the joint. The parent joint must already be
    uptodate!
*/
inline
void
nCharJoint::Evaluate()
{
    this->localMatrix.set(this->rotate);
    this->localMatrix.translate(this->translate);
    if (this->parentJoint)
    {
        this->localMatrix.mult_simple(this->parentJoint->localMatrix);
    }
    this->skinMatrix44 = this->invPoseMatrix * this->localMatrix;
    this->skinMatrix33.set(this->skinMatrix44.M11, this->skinMatrix44.M12, this->skinMatrix44.M13,
                           this->skinMatrix44.M21, this->skinMatrix44.M22, this->skinMatrix44.M23,
                           this->skinMatrix44.M31, this->skinMatrix44.M32, this->skinMatrix44.M33);
}

//------------------------------------------------------------------------------
/**
*/
inline
const matrix44&
nCharJoint::GetSkinMatrix44() const
{
    return this->skinMatrix44;
}

//------------------------------------------------------------------------------
/**
*/
inline
const matrix33&
nCharJoint::GetSkinMatrix33() const
{
    return this->skinMatrix33;
}

//------------------------------------------------------------------------------
/**
*/
inline
const matrix44&
nCharJoint::GetMatrix44() const
{
    return this->localMatrix;
}

//------------------------------------------------------------------------------
#endif
