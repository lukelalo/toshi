#ifndef N_CHARACTERSERVER_H
#define N_CHARACTERSERVER_H
//------------------------------------------------------------------------------
/**
    @class nCharacterServer
    
    @brief A Character server creates and manages nCharacter objects.

    (C) 2002 RadonLabs GmbH
*/
#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif

#ifndef N_KEYARRAY_H
#include "util/nkeyarray.h"
#endif

#undef N_DEFINES
#define N_DEFINES nCharacterServer
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class nCharacter;
class nCharacterServer : public nRoot
{
public:
    /// constructor
    nCharacterServer();
    /// destructor
    virtual ~nCharacterServer();

    /// create a character object
    nCharacter* NewCharacter(int key);
    /// find a character object
    nCharacter* FindCharacter(int key);
    /// release all character objects that match a masked subkey
    int ReleaseCharacters(int key, int keyMask);
    /// get a unique key
    int GetUniqueKey();

    /// pointer to nKernelServer
    static nKernelServer* kernelServer;

private:
    nKeyArray<nCharacter*> charArray;       // array for fast find by key
    int uniqueKey;
};
//------------------------------------------------------------------------------
#endif

