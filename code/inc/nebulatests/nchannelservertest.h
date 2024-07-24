#ifndef N_CHANNELSERVERTEST_H
#define N_CHANNELSERVERTEST_H
//------------------------------------------------------------------------------
/**
    Test nChannelServer functionality.

    (C) 2001 A.Weissflog
*/
#ifndef N_TESTCASE_H
#include "testsuite/ntestcase.h"
#endif

//------------------------------------------------------------------------------
class nKernelServer;
class nChannelServer;
class nChannelServerTest : public nTestCase
{
public:
    /// constructor
    nChannelServerTest(const char* name) :
        nTestCase(name),
        ks(0),
        chnServer(0)
    {}

    /// initialize the test
    virtual void Initialize(nKernelServer*);
    /// run the test
    virtual void Run();
    /// uninitialize the test
    virtual void Shutdown();

private:
    nKernelServer* ks;
    nChannelServer* chnServer;
};

//-------------------------------------------------------------------
#endif
