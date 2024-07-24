#ifndef N_CHARACTER_H
#define N_CHARACTER_H
//------------------------------------------------------------------------------
/**
    @class nCharacter
    
    @brief A low level character object.

    (C) 2002 RadonLabs GmbH
*/
#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifndef N_PRIMITIVESERVER_H
#include "gfx/nprimitiveserver.h"
#endif

#ifndef N_CHARSKELETON_H
#include "character/ncharskeleton.h"
#endif

#ifndef N_CHARSTATEARRAY_H
#include "character/ncharstatearray.h"
#endif

//------------------------------------------------------------------------------
class nVertexBuffer;
class nChannelContext;
class nGfxServer;
class nCharacter
{
public:
    /// constructor
    nCharacter();
    /// destructor
    ~nCharacter();

    /// set joint skeleton (copied)
    void SetCharSkeleton(const nCharSkeleton& skel);
    /// get joint skeleton
    const nCharSkeleton& GetCharSkeleton() const;
    /// set pointer to anim state array object (not owned!)
    void SetStateArray(const nCharStateArray* stateArray);
    /// get pointer to anim state array object
    const nCharStateArray* GetStateArray() const;

    /// set the currently active state
    void SetActiveState(int stateIndex, float time);
    /// get the currently active state
    int GetActiveState() const;
    /// evaluate the joint skeleton
    void EvaluateJoints(float time, nChannelContext* chnContext);
    /// render skeleton (debug visualization only)
    void RenderSkeleton(nPrimitiveServer* primServer, vector4& color);

private:
    /// read the current weight channel values
    void ReadWeightChannels(float curTime, nChannelContext* chnContext);
    /// clamp a weight between 0 and 1
    float ClampWeight(float w);
    
    enum
    {
        MAXSTATECHANNELS = 128
    };
    nCharSkeleton charSkeleton;             // the character's skeleton
    const nCharStateArray* charStateArray;  // pointer to character state array (not owned)

    int activateStateCounter;               // incremented during SetActiveState()
    float stateTransitionTime;              // the state transition time

    float activeStateStarted;                       // timestamp of current state activation
    int activeState;                                // the active state
    float activeStateChannels[MAXSTATECHANNELS];    // current channel set

    float lastEvalTime;                     // last time EvaluateJoints() was called
};

//------------------------------------------------------------------------------
/**
    Set the character's joint skeleton. An internal copy of the joint
    skeleton will be created.
*/
inline
void
nCharacter::SetCharSkeleton(const nCharSkeleton& skel)
{
    this->charSkeleton = skel;
}

//------------------------------------------------------------------------------
/**
    Get a reference to the internal character skeleton.
*/
inline
const nCharSkeleton&
nCharacter::GetCharSkeleton() const
{
    return this->charSkeleton;
}

//------------------------------------------------------------------------------
/**
    Set pointer to character state object (not owned)
*/
inline
void
nCharacter::SetStateArray(const nCharStateArray* stateArray)
{
    n_assert(stateArray);
    this->charStateArray = stateArray;
}

//------------------------------------------------------------------------------
/**
    Get pointer to character state array.
*/
inline
const nCharStateArray*
nCharacter::GetStateArray() const
{
    return this->charStateArray;
}

//------------------------------------------------------------------------------
/**
    Set the currently active state.
*/
inline
void
nCharacter::SetActiveState(int stateIndex, float time)
{
    n_assert(this->charStateArray);

    this->activateStateCounter++;
    this->activeStateStarted = time;
    this->activeState = stateIndex;

    // flush weight array
    memset(this->activeStateChannels, 0, sizeof(this->activeStateChannels));
}

//------------------------------------------------------------------------------
/**
    Get the currently active state.
*/
inline
int
nCharacter::GetActiveState() const
{
    return this->activeState;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nCharacter::ClampWeight(float w)
{
    if (w < 0.0f)
    {
        w = 0.0f;
    }
    else if (w > 1.0f)
    {
        w = 1.0f;
    }
    return w;
}

//------------------------------------------------------------------------------
#endif
