#define N_IMPLEMENTS nCharacterServer
//------------------------------------------------------------------------------
//  nCharacterServer
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "character/ncharacterserver.h"
#include "character/ncharacter.h"
#include "kernel/nkernelserver.h"

nNebulaClass(nCharacterServer, "nroot");

//------------------------------------------------------------------------------
/**
*/
nCharacterServer::nCharacterServer() :
    charArray(64, 64),
    uniqueKey(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nCharacterServer::~nCharacterServer()
{
    // delete all characters in character array
   int numElms = this->charArray.Size();
    int i;
    for (i = 0; i < numElms; i++)
    {
        nCharacter* curChar = this->charArray.GetElementAt(i);
        delete curChar;
    }
}

//------------------------------------------------------------------------------
/**
    Find nCharacter object by render context key.
*/
nCharacter*
nCharacterServer::FindCharacter(int key)
{
    nCharacter* chr;
    if (this->charArray.Find(key, chr))
    {
        return chr;
    }
    else
    {
        return 0;
    }
}

//------------------------------------------------------------------------------
/**
    Create a new nCharacter object with a given key (key must be unique).
*/
nCharacter*
nCharacterServer::NewCharacter(int key)
{
    nCharacter* chr;
    n_assert(!this->charArray.Find(key, chr));

    chr = new nCharacter;
    this->charArray.Add(key, chr);

    return chr;
}

//------------------------------------------------------------------------------
/**
    Release all characters which match a masked key. This is used by
    nCharacterNode to delete all characters which have been created
    by this node.

    @return     returns number of released character objects
*/
int
nCharacterServer::ReleaseCharacters(int key, int keyMask)
{
    int i;
    int numRemoved = 0;
   for (i = 0; i < this->charArray.Size();)
    {
        int curKey = this->charArray.GetKeyAt(i);
        if ((curKey & keyMask) == key)
        {
            nCharacter* chr = this->charArray.GetElementAt(i);
            this->charArray.RemByIndex(i);
            delete chr;
            numRemoved++;
        }
        else
        {
            i++;
        }
    }
    return numRemoved;
}

//------------------------------------------------------------------------------
/**
*/
int
nCharacterServer::GetUniqueKey()
{
    return this->uniqueKey++;
}

//------------------------------------------------------------------------------
