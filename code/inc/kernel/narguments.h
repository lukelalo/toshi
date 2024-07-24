#ifndef N_ARGUMENTS_H
#define N_ARGUMENTS_H
//-----------------------------------------------------------------------------
/**
   @class nArguments
   @brief Command line arguments accessable through index.
   @ingroup NebulaKernelUtilities
*/

#undef N_DEFINES
#define N_DEFINES nArguments
#include "kernel/ndefdllclass.h"
class N_PUBLIC nArguments 
{
public:
    /// Create from string.
    nArguments(char* args);
    /// Create from list of strings.
    nArguments(const char* args[], int num);
    /// Destruct.
    ~nArguments();

    /// Number of arguments given to command that started 
    /// system execution.
    int ArgumentCount() const;
    /// i-th argument of command that started system execution.
    const char* Argument(int i) const;

private:
    char** arguments;
    int count;
};


#endif

