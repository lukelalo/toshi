#ifndef N_TESTCASE_H
#define N_TESTCASE_H
//-------------------------------------------------------------------
/**
   @class nTestCase

   @brief encapsulate a test case

    A test case has 3 phases: initialization, run, shutdown. All
    3 phases must pass for the test to succeed. An actual test
    case is created by deriving a new subclass from nTestCase
    and overwriting the methods Initialize(), Run(), Shutdown().
*/
//-------------------------------------------------------------------
#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifndef N_STRNODE_H
#include "util/nstrnode.h"
#endif

#ifndef N_KERNELSERVER_H
#include "kernel/nkernelserver.h"
#endif

//-------------------------------------------------------------------
//  t_assert()
//  Use this assert statement instead of "assert()" or
//  "n_assert()" inside the test.
//-------------------------------------------------------------------
#define t_assert(x) n_assert(x)

//-------------------------------------------------------------------
class nTestCase : public nStrNode
{
public:
    nTestCase(const char *name);
    virtual ~nTestCase();

    virtual void Initialize(nKernelServer*) = 0;
    virtual void Run() = 0;
    virtual void Shutdown() = 0;
};

//-------------------------------------------------------------------
//  21-Jan-01   floh    created
//-------------------------------------------------------------------
inline
nTestCase::nTestCase(const char* name)
: nStrNode(name)
{
    // empty
}

//-------------------------------------------------------------------
//  21-Jan-01   floh    created
//-------------------------------------------------------------------
inline 
nTestCase::~nTestCase()
{
    // unlink from our test suite
    if (this->IsLinked()) {
        this->Remove();
    }
}

//-------------------------------------------------------------------
#endif
