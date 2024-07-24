//------------------------------------------------------------------------------
//  narraytest.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "nebulatests/narraytest.h"
#include "util/narray.h"

//------------------------------------------------------------------------------
/**
*/
void
nArrayTest::Initialize(nKernelServer* /*ks*/)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nArrayTest::Run()
{
    // create a default dynamic array
    nArray<int> array0;
    t_assert(array0.Empty());

    // fill with stuff
    int i;
    for (i = 0; i < 10000; i++)
    {
        array0.PushBack(i);
    }

    // check size
    t_assert(10000 == array0.Size());
    t_assert(0 == array0.Front());
    t_assert(9999 == array0.Back());

    // check contents
    for (i = 0; i < 10000; i++)
    {
        int j = array0.At(i);
        t_assert(j == i);
    }

    // insert and erase an element in the middle
    array0.Insert(5000, 1234567);
    t_assert(array0.Size() == 10001);
    t_assert(array0.Back() == 9999);
    t_assert(array0.Front() == 0);
    t_assert(array0.At(5000) == 1234567);
    t_assert(array0.At(4999) == 4999);
    t_assert(array0.At(5001) == 5000);

    array0.Erase(5000);
    t_assert(array0.Size() == 10000);
    t_assert(array0.Back() == 9999);
    t_assert(array0.Front() == 0);
    for (i = 0; i < 10000; i++)
    {
        int j = array0.At(i);
        t_assert(j == i);
    }

    // special cases: insert and erase element at front of array
    array0.Insert(0, 1234567);
    t_assert(array0.Size() == 10001);
    t_assert(array0.Back() == 9999);
    t_assert(array0.Front() == 1234567);
    t_assert(array0.At(0) == 1234567);
    t_assert(array0.At(1) == 0);

    array0.Erase(0);
    t_assert(array0.Size() == 10000);
    t_assert(array0.Back() == 9999);
    t_assert(array0.Front() == 0);
    for (i = 0; i < 10000; i++)
    {
        int j = array0.At(i);
        t_assert(j == i);
    }

    // special cases: insert and erase element at back of array
    array0.Insert(10000, 1234567);
    t_assert(array0.Size() == 10001);
    t_assert(array0.Back() == 1234567);
    t_assert(array0.Front() == 0);
    t_assert(array0.At(10000) == 1234567);
    t_assert(array0.At(9999) == 9999);

    array0.Erase(10000);
    t_assert(array0.Size() == 10000);
    t_assert(array0.Back() == 9999);
    t_assert(array0.Front() == 0);
    for (i = 0; i < 10000; i++)
    {
        int j = array0.At(i);
        t_assert(j == i);
    }

    // create a dynamic array with custom grow parameters
    nArray<ulong> array1(2000, 2000);
    t_assert(array1.Empty());

    ulong k;
    for (k = 0; k < 100000; k++)
    {
        array1.PushBack(k);
    }

    // check size
    t_assert(100000 == array1.Size());
    t_assert(0 == array1.Front());
    t_assert(99999 == array1.Back());

    // check contents
    for (k = 0; k < 100000; k++)
    {
        ulong l = array1.At(k);
        t_assert(l == k);
    }

    // make a copy of an array and run tests
    nArray<ulong> array2 = array1;

    // check size
    t_assert(100000 == array2.Size());
    t_assert(0 == array2.Front());
    t_assert(99999 == array2.Back());

    // check contents
    for (k = 0; k < 100000; k++)
    {
        ulong l = array2.At(k);
        t_assert(l == k);
    }

    // do an iterator test
    nArray<ulong>::iterator cur = array2.Begin();
    nArray<ulong>::iterator end = array2.End();
    for (k = 0; cur < end; cur++, k++)
    {
        t_assert(*cur == k);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nArrayTest::Shutdown()
{
    // empty
}

//------------------------------------------------------------------------------
