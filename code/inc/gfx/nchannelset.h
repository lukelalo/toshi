#ifndef N_CHANNELSET_H
#define N_CHANNELSET_H
//------------------------------------------------------------------------------
/* Copyright (c) 1997-2002 Radon Labs GmbH
 *
 * See the file "license.txt" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */
//------------------------------------------------------------------------------
/**
    @class nChannelSet

    @brief manage "channels of interest"

    nVisNode objects use nChannelSet objects to record the channels they
    are interested in across a visual hierarchy (usually a root
    object of a visual hierarchy will create a nChannelSet object
    and hand it down to its children, which configure the 
    nChannelSet with the channels they want to read or write). The
    nChannelSet object is then used to allocate a unique nChannelContext
    for this visual hierarchy.

    FIXME:
    If we would differentiate between READ and WRITE channels, we can
    do better overwrite checks in a hierarchy (basically, if 2 objects
    in a hierarchy want to write to the same channel, then there's a
    problem). 
*/

#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifndef N_CHANNELSERVER_H
#include "gfx/nchannelserver.h"
#endif

#ifndef N_ARRAY_H
#include "util/narray.h"
#endif

//------------------------------------------------------------------------------
class nChannelSet 
{
public:
    /// the constructor
    nChannelSet();
    /// the destructor
    ~nChannelSet();
    
    /// bump ref counter
    void AddRef();
    /// decrement ref counter
    void RemRef();
    /// get current ref counter
    short GetRef();

    /// clear the channel set
    void Clear();
    /// add a channel, return global channel index
    int AddChannel(nChannelServer* chnServer, const char* chnName);
    /// get the number of channels in this set
    int GetNumChannels();
    /// get global channel index at given local index
    int GetChannelAt(int localIndex);
    /// find a local index by global index
    int Find(int globalIndex);

private:
    short refCount;
    nArray<ushort> chnSet;
};

//------------------------------------------------------------------------------
/*
*/
inline
nChannelSet::nChannelSet() :
    refCount(0),
    chnSet(8, 16)
{
    // empty
}

//------------------------------------------------------------------------------
/*
*/
inline
nChannelSet::~nChannelSet()
{
    n_assert(0 == this->refCount);
}

//------------------------------------------------------------------------------
/*
*/
inline 
void
nChannelSet::AddRef()
{
    this->refCount++;
}

//------------------------------------------------------------------------------
/*
*/
inline
void
nChannelSet::RemRef()
{
    n_assert(refCount > 0);
    this->refCount--;
}

//------------------------------------------------------------------------------
/*
*/
inline
short
nChannelSet::GetRef()
{
    return this->refCount;
}

//------------------------------------------------------------------------------
/*
    Clear channels attached to this object. Leaves ref count untouched!
*/
inline
void
nChannelSet::Clear()
{   
    this->chnSet.Clear();
}

//------------------------------------------------------------------------------
/*
    Add a channel defined by a global index. First checks
    whether the channel is already in the set. If yes a channel index is returned. 
    If the channel does not yet exist, it is added to the channel set.

    @param  chnServer           pointer to channel server
    @param  chnName             channel name
    @return                     a channel index
*/
inline
int 
nChannelSet::AddChannel(nChannelServer* chnServer, const char* chnName)
{
    n_assert(chnServer);
    n_assert(chnName);

    // look up the channel in the channel server
    int gIndex = chnServer->GenChannel(chnName);

    // check if the channel is already in the set
    int num = this->chnSet.Size();
    int i;
    for (i = 0; i < num; i++)
    {
        if (this->chnSet.At(i) == gIndex)
        {
            return gIndex;
        }
    }

    // otherwise add channel to set
    this->chnSet.PushBack(gIndex);
    return gIndex;
}

//------------------------------------------------------------------------------
/*
    Return the current number of channels in the set.

    @return     the number of channels
*/
inline
int
nChannelSet::GetNumChannels()
{
    return this->chnSet.Size();
}

//------------------------------------------------------------------------------
/*
    Get channel at internal index i.
*/
inline
int
nChannelSet::GetChannelAt(int i)
{
    return this->chnSet.At(i);
}

//------------------------------------------------------------------------------
/*
    Find internal index for a global channel.
*/
inline
int
nChannelSet::Find(int gIndex)
{
    // check if the channel is already in the set
    int i;
    int num = this->chnSet.Size();
    for (i = 0; i < num; i++)
    {
        if (this->chnSet.At(i) == gIndex)
        {
            return i;
        }
    }
    n_error("nChannelSet::Find(): channel not in set!\n");
    return 0; // Not executed, here to prevent a warning.
}

//------------------------------------------------------------------------------
#endif
