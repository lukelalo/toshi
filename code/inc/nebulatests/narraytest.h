#ifndef N_ARRAYTEST_H
#define N_ARRAYTEST_H
//------------------------------------------------------------------------------
/**
    Test nArray functionality.

    (C) 2002 RadonLabs GmbH
*/
#ifndef N_TESTCASE_H
#include "testsuite/ntestcase.h"
#endif

//------------------------------------------------------------------------------
class nArrayTest : public nTestCase
{
public:
    /// constructor
    nArrayTest(const char* name) :
        nTestCase(name)
    {
        // empty
    }

    /// initialize the test
    virtual void Initialize(nKernelServer* ks);
    /// run the test
    virtual void Run();
    /// shutdown the test
    virtual void Shutdown();
};

//------------------------------------------------------------------------------
#endif

