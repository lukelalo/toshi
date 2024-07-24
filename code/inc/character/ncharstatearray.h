#ifndef N_CHARSTATEARRAY_H
#define N_CHARSTATEARRAY_H
//------------------------------------------------------------------------------
/**
    @class nCharStateArray
    
    @brief Holds character animation states.

    (C) 2002 RadonLabs GmbH
*/
#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifndef N_CHARSTATE_H
#include "character/ncharstate.h"
#endif

//------------------------------------------------------------------------------
class nAnimCurveArray;
class nCharSkeleton;
class nChannelContext;

class nCharStateArray
{
public:
    /// constructor
    nCharStateArray();
    /// destructor
    ~nCharStateArray();
    /// set pointer to anim curve array (not owned)
    void SetAnimCurveArray(const nAnimCurveArray* curveArray);
    /// get pointer to anim curve array (not owned)
    const nAnimCurveArray* GetAnimCurveArray() const;
    /// begin states (states are collections of weight-mixed anims)
    void BeginStates(int num);
    /// add a named state
    void AddState(int stateIndex, const char* name);
    /// begin adding anims to a state
    void BeginStateAnims(int stateIndex, int numAnims);
    /// add a weighted animation to a state
    void AddStateAnim(int stateIndex, int animIndex, const nCharSkeleton* charSkeleton, const char* animName, const char* weightChannel);
    /// finish adding weighted anims to a state
    void EndStateAnims(int stateIndex);
    /// finish defining states
    void EndStates();
    /// get number of states
    int GetNumStates() const;
    /// get ref to internal nCharState object
    const nCharState& GetState(int stateIndex) const;
    /// compute current state of a joint skeleton
    void UpdateSkeleton(nCharSkeleton* charSkeleton,
                        int curStateIdnex, 
                        float curStateTime, 
                        float* curStateChannels, 
                        int numStateChannels,
                        float transitionLerp) const;
    /// get state index by its name
    int GetStateIndexByName(const char* name) const;
    /// get state name by its index
    const char* GetStateNameByIndex(int stateIndex) const;

private:
    const nAnimCurveArray* animCurveArray;        // not owned
    int numStates;
    nCharState* stateArray;
};

//------------------------------------------------------------------------------
/**
    Get number of states.
*/
inline
int
nCharStateArray::GetNumStates() const
{
    return this->numStates;
}

//------------------------------------------------------------------------------
/**
    Get nCharState object of given state index.
*/
inline
const nCharState&
nCharStateArray::GetState(int stateIndex) const
{
    n_assert((stateIndex >= 0) && (stateIndex < this->numStates));
    return this->stateArray[stateIndex];
}

//------------------------------------------------------------------------------
/**
    @return     state index, or -1 if not found
*/
inline
int
nCharStateArray::GetStateIndexByName(const char* name) const
{
    n_assert(name);
    int i;
    for (i = 0; i < this->numStates; i++)
    {
        if (0 == strcmp(name, stateArray[i].GetName()))
        {
            return i;
        }
    }
    return -1;
}

//------------------------------------------------------------------------------
/**
    @return     state name for given index
*/
inline
const char*
nCharStateArray::GetStateNameByIndex(int stateIndex) const
{
    n_assert((stateIndex >= 0) && (stateIndex < this->numStates));
    return this->stateArray[stateIndex].GetName();
}

//------------------------------------------------------------------------------
#endif
