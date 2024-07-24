//-------------------------------------------------------------------
//  nebulatests.cc
//
//  Test runner executable.
//
//  (C) 2001 RadonLabs GmbH -- A.Weissflog
//-------------------------------------------------------------------
#include "testsuite/ntestsuite.h"
#include "nebulatests/nchannelservertest.h"
#include "nebulatests/narraytest.h"
#include "nebulatests/ndirectorytest.h"

//-------------------------------------------------------------------
int
main(int /*argc*/, char* /*argv[]*/)
{
    // create a new test suite object
    nTestSuite testSuite("Nebula Test Cases");

    // add test cases here...
    nArrayTest arrayTest("nArrayTest");
    testSuite.AddTestCase(arrayTest);

    nChannelServerTest channelServerTest("nChannelServerTest");
    testSuite.AddTestCase(channelServerTest);

    nDirectoryTest directoryTest("nDirectoryTest");
    testSuite.AddTestCase(directoryTest);
    
    // run the tests
    testSuite.Run();

    return 0;
}
//-------------------------------------------------------------------

