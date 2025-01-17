#define N_IMPLEMENTS nPythonServer
//--------------------------------------------------------------------
//  npythonserver.cc
//  Created by Jason Asbahr, 2001
//  Derived from ntclserver.cc by Andre Weissflog
//  Object interface support by Andy Miller, 2002
//  Updates by Tivonenko Ivan (aka Dark Dragon), 2003
//--------------------------------------------------------------------

#include <stdlib.h>
#include <stdio.h>

#include "kernel/nkernelserver.h"
#include "kernel/npersistserver.h"
#include "kernel/nfile.h"
#include "kernel/nfileserver2.h"
#include "python/npythonserver.h"

// Initialize static members to NULL
nClass *nPythonServer::local_cl        = NULL;
nKernelServer *nPythonServer::kernelServer       = NULL;
nPythonServer *nPythonServer::Instance = NULL;


extern char *npythonserver_version;

#ifdef __cplusplus
extern "C" {
#endif


PyObject *CreatedObjectsList_;
PyObject *CreatedObjectsList_weak_refs_;
PyObject *CreatedObjectsList_weakref_callback_;

// Nebula class interface
extern bool N_EXPORT n_init(nClass *, nKernelServer *);
extern void N_EXPORT n_fini(void);
extern N_EXPORT void *n_create(void);
extern N_EXPORT char *n_version(void);

// Python "Nebula type" and error object
extern N_EXPORT PyTypeObject Nebula_Type;
extern PyObject *Npy_ErrorObject;

// External declaration of Nebula commands
extern PyObject* pythoncmd_New(PyObject *self, PyObject *args);
extern PyObject* pythoncmd_Delete(PyObject *self, PyObject *args);
extern PyObject* pythoncmd_Sel(PyObject *self, PyObject *args);
extern PyObject* pythoncmd_Lookup( PyObject * /*self*/, PyObject *args);
extern PyObject* pythoncmd_Psel(PyObject *self, PyObject *args);
extern PyObject* pythoncmd_Get(PyObject *self, PyObject *args);
extern PyObject* pythoncmd_GetCwdObject( PyObject * /*self*/, PyObject *args);
extern PyObject* pythoncmd_Exit(PyObject *self, PyObject *args);
extern PyObject* pythoncmd_Connect(PyObject *self, PyObject *args);
extern PyObject* pythoncmd_Disconnect(PyObject *self, PyObject *args);
extern PyObject* pythoncmd_Server(PyObject *self, PyObject *args);
extern PyObject* pythoncmd_Set(PyObject *self, PyObject *args);
extern PyObject* pythoncmd_Puts(PyObject *self, PyObject *args);
extern PyObject* pythoncmd_Dir(PyObject *self, PyObject *args);
extern PyObject* pythoncmd_Exists(PyObject *self, PyObject *args);
extern PyObject* pythoncmd_Nprint(PyObject *self, PyObject *args);
extern PyObject* pythoncmd_SetTrigger(PyObject *self, PyObject *args);
extern PyObject* pythoncmd_RemoteGetAttr(PyObject *self, PyObject *args);
extern PyObject* CreatedObjectsList_weakref_callback(PyObject * /*self*/, PyObject *args);
  

// Python module definition table for Nebula commands
// NOTE: command 'dir' renamed to 'ndir' to avoid conflict with
//       Python's built-in keyword 'dir'

static PyMethodDef NebulaMethods[] = {
    {"new",           pythoncmd_New, METH_VARARGS, NULL},
    {"delete",        pythoncmd_Delete, METH_VARARGS, NULL},
    {"sel",           pythoncmd_Sel, METH_VARARGS, NULL},
    {"Lookup",        pythoncmd_Lookup, METH_VARARGS, NULL},
    {"SetCwd",        pythoncmd_Sel, METH_VARARGS, NULL},
    {"psel",          pythoncmd_Psel, METH_VARARGS, NULL},
    {"GetCwd",        pythoncmd_Psel, METH_VARARGS, NULL},
    {"GetCwdObject",  pythoncmd_GetCwdObject, METH_VARARGS, NULL},
    {"get",           pythoncmd_Get, METH_VARARGS, NULL},
    {"exit",          pythoncmd_Exit, METH_VARARGS, NULL},
    {"set",           pythoncmd_Set, METH_VARARGS, NULL},
    {"puts",          pythoncmd_Puts, METH_VARARGS, NULL},
    {"ndir",          pythoncmd_Dir, METH_VARARGS, NULL},     // Renamed
    {"exists",        pythoncmd_Exists, METH_VARARGS, NULL},
    {"nprint",        pythoncmd_Nprint, METH_VARARGS, NULL},  // Logging
    {"setTrigger",    pythoncmd_SetTrigger, METH_VARARGS},   // Trigger callback

    {"__CreatedObjectsList_weakref_callback__",  CreatedObjectsList_weakref_callback, METH_VARARGS}, 
    {NULL,NULL,0,NULL}    /* Sentinel */
};


// Python module initialization function
// Called implicitly to extend an external interpreter or
// called explicitly when embedding the interpreter.

void N_EXPORTFUNC initnpython()
{
    PyObject *m, *d, *gd;

    /* Initialize the type of the new type object here; doing it here
     * is required for portability to Windows without requiring C++. */
    Nebula_Type.ob_type = &PyType_Type;

    m = Py_InitModule("npython",  NebulaMethods);

    /* Add some symbolic constants to the module */
    d = PyModule_GetDict(m);
    Npy_ErrorObject = PyErr_NewException("npython.error", NULL, NULL);
    PyDict_SetItemString(d, "Npython_Error", Npy_ErrorObject);

    CreatedObjectsList_weakref_callback_ = PyDict_GetItemString(d, "__CreatedObjectsList_weakref_callback__");


    gd = PyDict_New();
    CreatedObjectsList_ = gd;
    PyModule_AddObject(m, "__created_objects__", gd);

    CreatedObjectsList_weak_refs_ = PyDict_New();
    PyModule_AddObject(m, "__created_objects_weak_refs__", CreatedObjectsList_weak_refs_ );
    

    // If Python is calling this function as part of an import
    if (!nPythonServer::Instance) {
      nPythonServer::kernelServer = new nKernelServer();
      n_assert(nPythonServer::kernelServer);

      nPythonServer *ps = (nPythonServer *) nPythonServer::kernelServer->Lookup("/sys/servers/script");
      if (!ps) {
        ps = (nPythonServer *) nPythonServer::kernelServer->New("npythonserver","/sys/servers/script");
        n_assert(ps);
        nPythonServer::Instance = ps;
      }
      nPythonServer::Instance->is_standalone_python = false;
    }
    else
      nPythonServer::Instance->is_standalone_python = true;
}


#ifdef __cplusplus
}
#endif


//--------------------------------------------------------------------
//  n_init()
//--------------------------------------------------------------------
bool N_EXPORT n_init(nClass *cl, nKernelServer *ks)
{
    nPythonServer::local_cl = cl;
    nPythonServer::kernelServer       = ks;
    ks->AddClass("nscriptserver",cl);
    return true;
}

//--------------------------------------------------------------------
//  n_fini()
//--------------------------------------------------------------------
void N_EXPORT n_fini(void)
{
    nPythonServer::kernelServer->RemClass(nPythonServer::local_cl);
}

//--------------------------------------------------------------------
//  n_create()
//--------------------------------------------------------------------
void *n_create(void)
{
    return new nPythonServer;
}

//--------------------------------------------------------------------
//  n_version()
//--------------------------------------------------------------------
N_EXPORT char *n_version(void)
{
    return npythonserver_version;
}


//--------------------------------------------------------------------
//  nPythonServer()
//  Initialize Python interpreter
//--------------------------------------------------------------------
nPythonServer::nPythonServer() :
    refFileServer(kernelServer, this)
{
    this->refFileServer        = "/sys/servers/file2";
    this->indent_level         = 0;
    this->indent_buf[0]        = 0;

    // Store a handy refernece to this instance
    nPythonServer::Instance = this;

    // Clear the Trigger() callback function
    this->callback = NULL;

    // Test to see if we are running inside an existing Python interpreter
    if (!Py_IsInitialized())
    {
      // setup interpreter
      Py_Initialize();

      // Explicitly initialize Nebula extensions
      initnpython();
    }

    // Store a handy reference to the nebula module
    this->nmodule = PyImport_ImportModule("npython");

    // And store a handy reference to the main module
    this->main_module = PyImport_ImportModule("__main__");

    // Install a mechanism to redirect stdout
    PyRun_SimpleString("import sys\n"
                       "import npython\n"
                       "sys.oldstdout = sys.stdout\n"
                       "sys.oldstderr = sys.stderr\n"
                       "class nwriter:\n"
                       "  def write(self, text):\n"
                       "    npython.nprint(text)\n"
                       "  def __del__(self):\n"
                       "    sys.stdout = sys.oldstdout\n"
                       "    sys.stderr = sys.oldstderr\n"
                       "sys.stdout = nwriter()\n"
                       "sys.stderr = nwriter()\n"
                      );


}

//--------------------------------------------------------------------
//  ~nPythonServer()
//  Shutdown Python interpreter
//--------------------------------------------------------------------
nPythonServer::~nPythonServer()
{
    Py_XDECREF(this->nmodule);
    Py_XDECREF(this->main_module);
    Py_XDECREF(this->callback);     // Clear Trigger() callback function

    this->nmodule     = NULL;
    this->main_module = NULL;
    this->callback    = NULL;

    if (is_standalone_python)
      Py_Finalize();
}


//------------------------------------------------------------------------------
/**
Begin writing a persistent object.
*/
nFile* 
nPythonServer::BeginWrite(const char* filename, nRoot* obj)
{
    n_assert(filename);
    n_assert(obj);

    this->indent_level = 0;

    nFile* file = this->refFileServer->NewFileObject();
    n_assert(file);
    if (file->Open(filename, "w"))
    {
        char buf[N_MAXPATH];
        sprintf(buf, "# $parser:npythonserver$ $class:%s$\n", obj->GetClass()->GetName());

        file->PutS("# ---\n");
        file->PutS(buf);
        file->PutS("# ---\n");

		file->PutS("\n__NDobj = sel('.')\n");

        return file;
    }
    else
    {
        n_printf("nPythonServer::WriteBegin(): failed to open file '%s' for writing!\n", filename);
        delete file;
        return 0;
    }
}

//--------------------------------------------------------------------
/**
    Finish writing a persistent object.
*/
bool 
nPythonServer::EndWrite(nFile* file)
{
    n_assert(file);

	file->PutS("del __NDobj\n\n");
    file->PutS("# ---\n");
    file->PutS("# Eof\n");

    file->Close();
    delete file;
    return (this->indent_level == 0);
}

//--------------------------------------------------------------------
//  _indent()
//
// TODO: Refactor indent logic as part of the object API wrapping phase.
//       For now, simply zero out any indention.
//--------------------------------------------------------------------
static void _indent(long i, char *buf)
{
    //long j;

    buf[0] = '\0';  // Cancel out the indent buffer

    //buf[0] = 0;
    //for (j=0; j<i; j++) strcat(buf,"  ");
}

//--------------------------------------------------------------------
//  WriteComment()
//--------------------------------------------------------------------
bool nPythonServer::WriteComment(nFile *file, const char *str)
{
    n_assert(file);
    n_assert(str);
    file->PutS("# ");
    file->PutS(str);
    file->PutS("\n");
    return true;
}

//--------------------------------------------------------------------
//  write_select_statement()
//  Write the statement to select an object after its creation
//  statement.
//--------------------------------------------------------------------
void nPythonServer::write_select_statement(nFile *file, nRoot *o, nRoot *owner)
{
    switch (this->GetSelectMethod()) {

        case SELCOMMAND:
            // get relative path from owner to o and write select statement
            char relpath[N_MAXPATH];
            _indent(++this->indent_level, this->indent_buf);
            owner->GetRelPath(o, relpath, sizeof(relpath));
            file->PutS(this->indent_buf);
            file->PutS("__NDobj = sel('");
            file->PutS(relpath);
            file->PutS("')\n");
            break;

        case NOSELCOMMAND:
            break;
    }
}

//--------------------------------------------------------------------
//  WriteBeginNewObject()
//  Write start of persistent object with default constructor.
//--------------------------------------------------------------------
bool nPythonServer::WriteBeginNewObject(nFile *file, nRoot *o, nRoot *owner)
{
    n_assert(file);
    n_assert(o);
    const char *o_name  = o->GetName();

    // write generic 'new' statement
    const char *o_class = o->GetClass()->GetName();
    _indent(this->indent_level,this->indent_buf);

    // NOTE: Generated in the form of a function call
    char buf[N_MAXPATH];
    sprintf(buf, "\n%s__NDobj = new('%s','%s')\nsel(__NDobj)\n", this->indent_buf, o_class, o_name);
    file->PutS(buf);
    
    // write select object statement
    this->write_select_statement(file,o,owner);
    return true;
}

//--------------------------------------------------------------------
//  WriteBeginNewObjectCmd()
//  Write start of persistent object with custom constructor
//  defined by command.
//--------------------------------------------------------------------
bool nPythonServer::WriteBeginNewObjectCmd(nFile *file, nRoot *o, nRoot *owner, nCmd *cmd)
{
    n_assert(file);
    n_assert(o);
    n_assert(cmd);

    // write constructor statement
    if (cmd) this->WriteCmd(file, cmd);

    // write select object statement
    this->write_select_statement(file, o, owner);
    return true;
}

//--------------------------------------------------------------------
//  WriteBeginSelObject()
//  Write start of persisting object without constructor, only
//  write the select statement.
//--------------------------------------------------------------------
bool nPythonServer::WriteBeginSelObject(nFile *file, nRoot *o, nRoot *owner)
{
    n_assert(file);
    n_assert(o);

    // write select object statement
    this->write_select_statement(file, o, owner);
    return true;
}

//--------------------------------------------------------------------
//  WriteEndObject()
//--------------------------------------------------------------------
bool nPythonServer::WriteEndObject(nFile *file, nRoot *o, nRoot *owner)
{
    n_assert(file);
    n_assert(o);

    // get relative path from owner to o and write select statement
    char relpath[N_MAXPATH];
    _indent(--this->indent_level, this->indent_buf);
    o->GetRelPath(owner, relpath, sizeof(relpath));
    file->PutS(this->indent_buf);
    file->PutS("__NDobj = sel('");
    file->PutS(relpath);
    file->PutS("')\n");

    return true;
}

//--------------------------------------------------------------------
//  WriteCmd()
//--------------------------------------------------------------------
bool nPythonServer::WriteCmd(nFile *file, nCmd *cmd)
{
    n_assert(file);
    n_assert(cmd);
    const char *name = cmd->GetProto()->GetName();
    n_assert(name);
    nArg *arg;
    char buf[N_MAXPATH];
    sprintf(buf,"%s__NDobj.%s(",this->indent_buf,name);
    file->PutS(buf);
    
    cmd->Rewind();
    int num_args = cmd->GetNumInArgs();

    const char* strPtr;
    ushort strLen;
    ushort bufLen;

    int i;
    for (i=0; i<num_args; i++) {

        arg=cmd->In();

        switch(arg->GetType()) {

            case nArg::ARGTYPE_INT:
                sprintf(buf,"%d",arg->GetI());
                break;

            case nArg::ARGTYPE_FLOAT:
                sprintf(buf,"%.6f",arg->GetF());
                break;

            case nArg::ARGTYPE_STRING:
                strPtr = arg->GetS();
                strLen = strlen(strPtr);
                bufLen = sizeof(buf)-1;
            
                file->PutS("r'");
                if (strLen > bufLen-1) 
                {
                    buf[bufLen] = 0; // Null terminator
                    for (int j=0; j<strLen-2; j+=bufLen)
                    {
                        memcpy((void*)&buf[0], strPtr, bufLen);
                        file->PutS(buf);
                        strPtr += bufLen;     
                    }
                    strPtr += bufLen;
                }
                sprintf(buf, "%s'", strPtr);
                break;

            case nArg::ARGTYPE_BOOL:
                sprintf(buf,"%s",(arg->GetB() ? "1" : "0"));
                break;

            /*
            case nArg::ARGTYPE_CODE:
                sprintf(buf," { %s }",arg->GetC());
                break;
            */

            case nArg::ARGTYPE_OBJECT:
                {
                    nRoot *o = (nRoot *) arg->GetO();
                    if (o) {
                        char buf[N_MAXPATH];
                        sprintf(buf, "'%s'", o->GetFullName(buf, sizeof(buf)));
                    } else {
                        sprintf(buf, " null");
                    }
                }
                break;

            default:
                sprintf(buf," ???");
                break;
        }
        file->PutS(buf);
        if (i < (num_args-1))
          file->PutS(", ");
     }
     file->PutS(")");

     return file->PutS("\n");
}

//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------

