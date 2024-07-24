#ifndef N_CHARSKELETON_H
#define N_CHARSKELETON_H
//------------------------------------------------------------------------------
/**
    @class nCharSkeleton
    
    @brief Implements a character skeleton made of nCharJoint objects.

    (C) 2002 RadonLabs GmbH
*/
#ifndef N_CHARJOINT_H
#include "character/ncharjoint.h"
#endif

//------------------------------------------------------------------------------
class nCharSkeleton
{
public:
    /// constructor
    nCharSkeleton();
    /// destructor
    ~nCharSkeleton();
    /// clear content
    void Clear();
    /// assignment operator
    nCharSkeleton& operator=(const nCharSkeleton& rhs);
    /// begin adding joints
    void BeginJoints(int num);
    /// add a joint to the joint skeleton
    void AddJoint(int index, const char* jointName, int parentIndex, const vector3& poseTranslate, const quaternion& poseRotate);
    /// finish joint skeleton
    void EndJoints();
    /// get number of joint
    int GetNumJoints() const;
    /// get joint index by name (slow)
    int GetJointIndexByName(const char* name) const;
    /// get joint by index
    nCharJoint& GetJoint(int index) const;

private:
    int numJoints;                          // number of joints
    nCharJoint* jointArray;                 // the joint palette
    bool inBeginJoints;
};

//------------------------------------------------------------------------------
/**
*/
inline
nCharSkeleton::nCharSkeleton() :
    numJoints(0),
    jointArray(0),
    inBeginJoints(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nCharSkeleton::Clear()
{
    if (this->jointArray)
    {
        delete[] this->jointArray;
        this->jointArray = 0;
    }
    this->numJoints = 0;
}

//------------------------------------------------------------------------------
/**
*/
inline
nCharSkeleton::~nCharSkeleton()
{
    this->Clear();
}

//------------------------------------------------------------------------------
/**
*/
inline
nCharSkeleton& 
nCharSkeleton::operator=(const nCharSkeleton& rhs)
{
    this->Clear();

    int num = rhs.GetNumJoints();
    if (num > 0)
    {
        this->BeginJoints(num);
        int i;
        for (i = 0; i < rhs.GetNumJoints(); i++)
        {
            const nCharJoint& joint = rhs.GetJoint(i);
            this->AddJoint(i, joint.GetName(), joint.GetParentJointIndex(), joint.GetPoseTranslate(), joint.GetPoseRotate());
        }
        this->EndJoints();
    }
    return *this;
}

//------------------------------------------------------------------------------
/**
    Start joint skeleton definition.

    @param  num     number of joints in skeleton
*/
inline
void
nCharSkeleton::BeginJoints(int num)
{
    n_assert(num > 0);
    n_assert(!this->jointArray);
    n_assert(!this->inBeginJoints);

    this->jointArray = new nCharJoint[num];
    this->numJoints = num;
    this->inBeginJoints = true;
}

//------------------------------------------------------------------------------
/**
    Add a joint to the skeleton.

    @param  index           joint index
    @param  name            name of joint (used to link joint to the right anim curve)
    @param  parentIndex     index of parent joint (-1 for root joint)
    @param  poseTranslate   the pose translation
    @param  poseRotate      the pose rotation
*/
inline
void
nCharSkeleton::AddJoint(int index, 
                        const char* jointName, 
                        int parentIndex, 
                        const vector3& poseTranslate,
                        const quaternion& poseRotate)
{
    n_assert((index >= 0) && (index < this->numJoints));
    n_assert(jointName);
    n_assert((parentIndex == -1) || (parentIndex < index));
    n_assert(this->jointArray);
    n_assert(this->inBeginJoints);

    nCharJoint* parentJoint = 0;
    if (parentIndex != -1)
    {
        parentJoint = &(this->jointArray[parentIndex]);
    }

    nCharJoint& joint = this->jointArray[index];
    joint.SetName(jointName);
    joint.SetParentJoint(parentJoint);
    joint.SetParentJointIndex(parentIndex);
    joint.SetPose(poseTranslate, poseRotate);
}

//------------------------------------------------------------------------------
/**
    Finish defining joints.
*/
inline
void
nCharSkeleton::EndJoints()
{
    n_assert(this->inBeginJoints);
    
    this->inBeginJoints = false;

    // make sure all joints have been initialized
    int i;
    for (i = 0; i < this->numJoints; i++)
    {
        n_assert(this->jointArray[i].GetName());
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nCharSkeleton::GetNumJoints() const
{
    return this->numJoints;
}

//------------------------------------------------------------------------------
/**
    @return     joint index, or -1 if joint not found
*/
inline
int
nCharSkeleton::GetJointIndexByName(const char* name) const
{
    n_assert(name);
    n_assert((this->numJoints > 0) && (this->jointArray));

    int i;
    for (i = 0; i < this->numJoints; i++)
    {
        if (strcmp(this->jointArray[i].GetName(), name) == 0)
        {
            return i;
        }
    }
    return -1;
}

//------------------------------------------------------------------------------
/**
*/
inline
nCharJoint&
nCharSkeleton::GetJoint(int index) const
{
    n_assert((index >= 0) && (index < this->numJoints));
    return this->jointArray[index];
}

//------------------------------------------------------------------------------
#endif
