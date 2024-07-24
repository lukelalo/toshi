#define N_IMPLEMENTS nJoint2
//------------------------------------------------------------------------------
//  njoint2_main.cc
//  (C) 2001 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "node/njoint2.h"

nNebulaScriptClass(nJoint2, "n3dnode");

//------------------------------------------------------------------------------
/**
*/
nJoint2::nJoint2() :
    poseDirty(true),
    poseScale(1.0f, 1.0f, 1.0f)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nJoint2::~nJoint2()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Update the internal inverse relaxed pose matrix. Must be called when 
    the "poseDirty" flag is set.
*/
void
nJoint2::UpdatePoseMatrix()
{
    this->poseMatrix.ident();
    this->poseMatrix.scale(this->poseScale);
    this->poseMatrix.mult_simple(matrix44(this->poseRotate));
    this->poseMatrix.translate(this->poseTranslate);

    // multiply with parent inverse pose matrix
    nJoint2* parentJoint = (nJoint2*) this->GetParent();
    if (parentJoint->IsInstanceOf(this->GetClass()))
    {
        this->poseMatrix.mult_simple(parentJoint->poseMatrix);
    }
    this->invPoseMatrix = this->poseMatrix;
    this->invPoseMatrix.invert_simple();

    this->poseDirty = false;
}

//------------------------------------------------------------------------------
/**
    Compute the matrices needed by nMeshCluster to generate the skinned mesh.
*/
void
nJoint2::Compute(nSceneGraph2* sceneGraph)
{
    // update inverse pose matrix if dirty
    if (this->poseDirty)
    {
        this->UpdatePoseMatrix();
    }

    // first let n3DNode compute it's stuff
    n3DNode::Compute(sceneGraph);

    // multiply own local 3d matrix with parent local 3d matrix to get
    // 3d matrix in the skin's model space
    this->skinLocalMatrix = this->mx;
    nJoint2* parentJoint = (nJoint2*) this->GetParent();
    if (parentJoint->IsInstanceOf(this->GetClass()))
    {
        this->skinLocalMatrix.mult_simple(parentJoint->skinLocalMatrix);
    }

    // create the matrices needed by nMeshCluster to transform skin vertices
    // and skin normals
    this->skinMatrix44 = this->invPoseMatrix * this->skinLocalMatrix;
    this->skinMatrix33.set(this->skinMatrix44.M11, this->skinMatrix44.M12, this->skinMatrix44.M13,
                           this->skinMatrix44.M21, this->skinMatrix44.M22, this->skinMatrix44.M23,
                           this->skinMatrix44.M31, this->skinMatrix44.M32, this->skinMatrix44.M33);
}

//------------------------------------------------------------------------------
