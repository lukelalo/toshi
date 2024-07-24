#ifndef N_TESTSUITE_H
#define N_TESTSUITE_H
//-------------------------------------------------------------------
/**
    @class nTestSuite

    @brief manage and analyze test runs

    nTestSuite holds a collection of nTestCase objects, and
    does test runs over them. It also embeds an Nebula
    nKernelServer and nScriptServer which the test cases
    can use (is this a good idea? or should each test case
    set up its own environment completely?)
*/
//-------------------------------------------------------------------
#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifndef N_STRLIST_H
#include "util/nstrlist.h"
#endif

#ifndef N_TESTCASE_H
#include "testsuite/ntestcase.h"
#endif

#ifndef N_KERNELSERVER_H
#include "kernel/nkernelserver.h"
#endif

#include <stdio.h>

//-------------------------------------------------------------------
class nTestSuite
{
public:
    nTestSuite(const char *name);
    ~nTestSuite();

    void AddTestCase(nTestCase& testCase);
    void Run();

protected:
    const char *name;
    nStrList testCaseList;
    nKernelServer *ks;
};

//-------------------------------------------------------------------
//  21-Jan-01   floh    created
//-------------------------------------------------------------------
inline 
nTestSuite::nTestSuite(const char* n)
{
    this->ks = new nKernelServer;
    this->name = n_strdup(n);
}

//-------------------------------------------------------------------
//  21-Jan-01   floh    created
//-------------------------------------------------------------------
inline
nTestSuite::~nTestSuite()
{
    nTestCase* tc;
    while ((tc = (nTestCase *) this->testCaseList.RemHead()));
    n_free((void *) this->name);
    delete this->ks;
}

//-------------------------------------------------------------------
//  21-Jan-01   floh    created
//-------------------------------------------------------------------
inline 
void 
nTestSuite::AddTestCase(nTestCase& testCase)
{
    // add the test case to the test case list
    this->testCaseList.AddTail(&testCase);
}

//-------------------------------------------------------------------
//  21-Jan-01   floh    created
//-------------------------------------------------------------------
inline
void
nTestSuite::Run()
{
    // for each test case, run Initialize(), Run(), Shutdown()
    nTestCase *tc;
    for (tc = (nTestCase *) this->testCaseList.GetHead();
         tc;
         tc = (nTestCase *) tc->GetSucc())
    {
        const char *tc_name = tc->GetName();

        n_printf("---------\n");
        n_printf("running test case: '%s'\n", tc_name);
        
        // initialize test case
        tc->Initialize(this->ks);
        n_printf("initialization ok.\n");

        // run actual test
        tc->Run(); 
        n_printf("test run ok.\n");

        // shutdown
        tc->Shutdown();
        n_printf("shutdown ok.\n");
    }
};

//-------------------------------------------------------------------
#endif
