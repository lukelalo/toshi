#ifndef N_DIRECTORYTEST_H
#define N_DIRECTORYTEST_H
//------------------------------------------------------------------------------
/**
    Test nDirectory functionality.
    15-Feb-2003     cubejk  created
*/
#ifndef N_TESTCASE_H
#include "testsuite/ntestcase.h"
#endif

#ifndef N_FILESERVER2_H
#include "kernel/nfileserver2.h"
#endif

#ifndef N_DIRECTORY_H
#include "kernel/ndirectory.h"
#endif

//------------------------------------------------------------------------------
class nDirectoryTest : public nTestCase
{
public:
    /// constructor
    nDirectoryTest(const char* name) :
        nTestCase(name)
    {
        fs  = NULL;
        dir = NULL;
    }

    /// initialize the test
    virtual void Initialize(nKernelServer* ks);
    /// run the test
    virtual void Run();
    /// shutdown the test
    virtual void Shutdown();
private:
    /// the directory object
    nDirectory *dir;
    /// the fileserver
    nFileServer2 *fs;
};

//------------------------------------------------------------------------------
#endif
