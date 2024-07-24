#
# Unit Tests for Nebula Python extension
# Created April, 2002, Jason L. Asbahr
# 

import os
import sys
import new
import copy
import unittest
import traceback
import random
import weakref

import npython    # Nebula Python extension


#
# Test Utilities
# 

# Utility to setup and teardown

class LocalTestBase(unittest.TestCase):

  def setUp(self):
    pass

  def tearDown(self):
    pass


#
# Actual Tests
#

# Exercise the basic Nebula functions locally

class LocalTestCase(LocalTestBase):

  # Test Psel

  def testPselCommand(self):
    result = npython.psel()
    assert result == '/'  

  # Test Ndir

  def testNdirCommand(self):
    result = npython.ndir()
    assert result == ['sys']

  # Test Sel
  
  def testSelCommand(self):
    # Returns an opaque 'Nebula Object'
    result = npython.sel('sys')
    # TODO: Test result object itself
    path = npython.psel()
    assert path == '/sys'

  def testUpSelCommand(self):
    # Returns an opaque 'Nebula Object'
    result = npython.sel('..')
    # TODO: Test result object itself
    path = npython.psel()
    assert path == '/'

  def testFailedSelCommand(self):
    # Should throw an exception
    try:
      result = npython.sel('')
    except:
      # Exception thrown
      pass
    else:
      assert 1 == 0, 'No exception thrown'

  def testEmptySelCommand(self):
    # Returns an opaque 'Nebula Object'
    result = npython.sel()
    pass

  # Test Exists

  def testExistsCommand(self):
    # Returns a 1 or 0
    result = npython.exists('/sys')
    assert result == 1

  def testFailedExistsCommand(self):
    # Returns a 1 or 0
    result = npython.exists('UnlikelyToExistObject')
    assert result == 0

  # Test Log

  def testLogCommand(self):
    # First, see if the test file already exists
    if os.access('test.log', os.W_OK):
      os.remove('test.log')
      if os.access('test.log', os.W_OK):
        assert 1 == 0, 'Unable to clean up test.log file'
    # Set the log file
    #npython.log('test.log') - deprecated
    # Use utility logging function
    npython.puts('foo')
    # See if the file exists
    result = os.access('test.log', os.R_OK)
    assert result
    # TODO: Confirm contents of log
    # TODO: Close log, remove file


  # Tests Puts

  def testPutsCommand(self):
    # First, see if the test file already exists
    if os.access('test2.log', os.W_OK):
      os.remove('test2.log')
      if os.access('test2.log', os.W_OK):
        assert 1 == 0, 'Unable to clean up test2.log file'
    # Set the log file
    #npython.log('test2.log') - deprecated
    # Use utility logging function
    npython.nprint('bar')
    # See if the file exists
    result = os.access('test2.log', os.R_OK)
    assert result, 'Log file test2.log does not exist!'
    # TODO: Confirm contents of log
    # TODO: Close log, remove file
    pass

        
  # Test New

  def testNewCommand(self):
    # Returns an opaque 'Nebula Object'
    object = npython.new('n3dnode', 'one')
    # Test for its presence
    result = npython.exists('one')
    assert locals().has_key('object'), 'Object not created' # TODO : need something else
    assert result, 'Object cannot be located'

  def testMultipleNewCommand(self):
    # Make a few of these...
    print 'Making!'
    objectSet = []
    for num in xrange(10):
      name = str(num)
      # Returns an opaque 'Nebula Object'
      object = npython.new('n3dnode', name)
      # Test for its presence
      result = npython.exists(name)
      assert locals().has_key('object'), 'Object not created'
      assert result, 'Object cannot be located'
      objectSet.append(object)
    # Cleanup
    print 'Cleaning up...'
    for num in xrange(10):
      name = str(num)
      npython.delete(name)
    
  def testFailedNewCommand(self):
    try:
      print 'Trying to create non-existent class...'
      object = npython.new('UnlikelyToExistClass', 'two')
    except:
      # Yep, that's what we want
      # NOTE: Perhaps new() should throw a NameError when it can't
      #       acquire a class
      print '...success!!'
    else:
      assert 1 == 0, 'NameError exception not thrown on failed New'

  # Test Delete

  def testDeleteCommand(self):
    # Returns an opaque 'Nebula Object'
    object = npython.new('n3dnode','three')
    # Test for its presence
    result = npython.exists('three')
    assert locals().has_key('object'), 'Object not created'
    assert result, 'Object cannot be located'
    # Delete object
    npython.delete('three')
    # Test for its presence
    result = npython.exists('three')
    assert result == 0, 'Object cannot be deleted'

  # Test Set

  def testSetCommand(self):
    # Returns an opaque 'Nebula Object'
    object = npython.new('n3dnode','four')
    # Select the object
    npython.sel('four')
    # Set a value on the object
    npython.set('txyz',0,30,0)
    # Clean up and delete object
    npython.sel('..')
    npython.delete('four')


  # Test dictionary lookup
  def testDictLookup(self):
    root = npython.sel('/')
    testobj = root['sys']
    assert locals().has_key('testobj'), "Can't lookup object by [] syntax"
    
  def testLoopDictLookup(self):
    root = npython.sel('/')
    npython.new('nroot', '/ee')
    for obj in root:
      assert locals().has_key('obj'), "Can't lookup object by iterator"
    npython.delete('/ee')

  def testExceptionsInDictAccess(self):
    s = npython.sel('/sys')
    try:
        s['UnlikelyToExistObject']
    except KeyError:
    # all right
      pass
    else:
      assert 1 == 0, 'Key error not thrown'
    
    npython.sel('/')




  def testSameObjectReturn(self):
    root = npython.sel('/')
    ee = npython.new('nroot', '/ee')
    ee_same = npython.sel('/ee')
    ee_same2 = root['ee']

    assert ee is ee_same, "Returned diffrent objects. Must be same object"
    assert ee is ee_same2, "Returned diffrent objects. Must be same object"

    npython.sel('/')
    npython.delete('/ee')

  def testRefCounts(self):
    refcountstest = npython.new('nroot', '/refcountstest')
    recount = refcountstest.getrefcount()

    assert recount, "Refcount for object not incremenated"

    npython.delete('/refcountstest')

  def testWekRefs(self):
    sysobj = npython.sel('/sys')
    try:
      ref = weakref.ref(sysobj)
    except:
      assert 1 == 0, "Can't create weakref"
    weakrefscount = weakref.getweakrefcount(sysobj)
    assert weakrefscount, "Something strange with weak refs"

    try:
      ref().getcmds()
    except:
      assert 1 == 0, "Can't access weakrefed object"


  def testDeleteObjectAccess(self):
    x = npython.new('nroot','/x')
    x_child = npython.new('nroot','/x/x3')
    x_child_same = x['x3']

    assert x_child is x_child_same, "Error in same object return"
    npython.delete('/x')

    try:
      x_child.getfulname()
    except npython.Npython_Error:
      pass # all right
    else:
      assert 1 == 0, "Exception not thrown then accessing deleted nebula object"
      

  def testCreationOfNewObject(self):
    x = npython.new('nroot','/x')
    npython.delete('/x')
    x2 = npython.new('nroot','/x')

    assert not (x is x2), "Error in same object return - returned same object, must be diff"


  def testGetCwdObject(self):
    x = npython.new('nroot','/x')
    x3 = npython.sel('/x')
    x2 = npython.GetCwdObject()

    assert x is x2, "Error diff object returned, must be same"
    npython.delete('/x')
  
  def testLookup(self):
    root_obj = npython.SetCwd('/')
    sys_obj = npython.Lookup('/sys')
    must_be_root = npython.GetCwdObject()

    assert root_obj is must_be_root, "Error diff object returned, must be same"

  
  # Test Get
  def testGetCommand(self):
    # Load a Python file from disk and execute it
    pass



# Define a suite of tests to call

def GetSuite():

  # tests for suite
  testSuite = unittest.TestSuite()

  testSuite.addTest(LocalTestCase("testPselCommand"))
  testSuite.addTest(LocalTestCase("testNdirCommand"))
  testSuite.addTest(LocalTestCase("testSelCommand"))
  testSuite.addTest(LocalTestCase("testUpSelCommand"))
  testSuite.addTest(LocalTestCase("testFailedSelCommand"))
  testSuite.addTest(LocalTestCase("testEmptySelCommand"))  # Bug in Sel()
  testSuite.addTest(LocalTestCase("testExistsCommand"))
  testSuite.addTest(LocalTestCase("testFailedExistsCommand"))

  #testSuite.addTest(LocalTestCase("testLogCommand"))
  #testSuite.addTest(LocalTestCase("testPutsCommand"))

  testSuite.addTest(LocalTestCase("testNewCommand"))
  testSuite.addTest(LocalTestCase("testMultipleNewCommand"))
  testSuite.addTest(LocalTestCase("testFailedNewCommand"))
  testSuite.addTest(LocalTestCase("testDeleteCommand"))
  testSuite.addTest(LocalTestCase("testSetCommand"))

  testSuite.addTest(LocalTestCase("testDictLookup"))
  testSuite.addTest(LocalTestCase("testLoopDictLookup"))
  testSuite.addTest(LocalTestCase("testExceptionsInDictAccess"))

  testSuite.addTest(LocalTestCase("testSameObjectReturn"))
  testSuite.addTest(LocalTestCase("testRefCounts"))
  testSuite.addTest(LocalTestCase("testWekRefs"))
  testSuite.addTest(LocalTestCase("testDeleteObjectAccess"))
  testSuite.addTest(LocalTestCase("testCreationOfNewObject"))
  testSuite.addTest(LocalTestCase("testGetCwdObject"))
  testSuite.addTest(LocalTestCase("testLookup"))
  

  return testSuite


if __name__ == "__main__":
  runner = unittest.TextTestRunner()
  runner.run(GetSuite())
