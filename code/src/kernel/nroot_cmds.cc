#define N_IMPLEMENTS nRoot
#define N_KERNEL
//-------------------------------------------------------------------
//  kernel/nrootscript.cc
//  This file was machine generated.
//  (C) 2000 A.Weissflog/A.Flemming
//-------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "kernel/ncmdproto.h"
#include "kernel/nroot.h"
#include "kernel/nref.h"

static void n_getrefcount(void *, nCmd *);
static void n_getrefs(void *, nCmd *);
static void n_getclass(void *, nCmd *);
static void n_isa(void *, nCmd *);
static void n_isinstanceof(void *, nCmd *);
static void n_getcmds(void *, nCmd *);
static void n_save(void *, nCmd *);
static void n_saveas(void *, nCmd *);
static void n_clone(void *, nCmd *);
static void n_parse(void *, nCmd *);
static void n_getparent(void *, nCmd *);
static void n_gethead(void *, nCmd *);
static void n_gettail(void *, nCmd *);
static void n_getsucc(void *, nCmd *);
static void n_getpred(void *, nCmd *);
static void n_getchildren(void *, nCmd *);
static void n_setname(void *, nCmd *);
static void n_getname(void *, nCmd *);
static void n_getfullname(void *, nCmd *);
static void n_getrelpath(void *, nCmd *);
static void n_find(void *, nCmd *);
static void n_sort(void *, nCmd *);
static void n_import(void *, nCmd *);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nroot

    @superclass
    ---

    @classinfo
    nroot is the superclass of all higher level Nebula class
    and defines the basic behaviour and properties for all 
    nroot derived classes:
    - linkage into hierarchical name space of objects
    - runtime type information
    - object persistency
    - language independent scripting interface 
*/
// This is named differently to work around a flaw in the kernel/build system.
void nroot_initcmds(nClass *cl)
{
    cl->BeginCmds();
    cl->AddCmd("i_getrefcount_v",   'GRCT', n_getrefcount);
    cl->AddCmd("l_getrefs_v",       'GREF', n_getrefs);
    cl->AddCmd("s_getclass_v",      'GCLS', n_getclass);
    cl->AddCmd("b_isa_s",           'ISA_', n_isa);
    cl->AddCmd("b_isinstanceof_s",  'ISIO', n_isinstanceof);
    cl->AddCmd("l_getcmds_v",       'GMCD', n_getcmds);
    cl->AddCmd("b_save_v",          'SAVE', n_save);
    cl->AddCmd("b_saveas_s",        'SVAS', n_saveas);
    cl->AddCmd("o_clone_s",         'CLON', n_clone);
    cl->AddCmd("b_parse_s",         'PARS', n_parse);
    cl->AddCmd("o_getparent_v",     'GPRT', n_getparent);
    cl->AddCmd("o_gethead_v",       'GHED', n_gethead);
    cl->AddCmd("o_gettail_v",       'GTAL', n_gettail);
    cl->AddCmd("o_getsucc_v",       'GSUC', n_getsucc);
    cl->AddCmd("o_getpred_v",       'GPRD', n_getpred);
    cl->AddCmd("l_getchildren_v",   'GCHD', n_getchildren);
    cl->AddCmd("v_setname_s",       'SNAM', n_setname);
    cl->AddCmd("s_getname_v",       'GNAM', n_getname);
    cl->AddCmd("s_getfullname_v",   'GFNM', n_getfullname);
    cl->AddCmd("s_getrelpath_o",    'GRPH', n_getrelpath);
    cl->AddCmd("o_find_s",          'FIND', n_find);
    cl->AddCmd("v_sort_v",          'SORT', n_sort);
    cl->AddCmd("b_import_s",        'IMPO', n_import);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    getrefcount

    @input
    v

    @output
    i (Refcount)

    @info
    Return current ref count of object.
*/
static void n_getrefcount(void *o, nCmd *cmd)
{
    nRoot *self = (nRoot *) o;
    cmd->Out()->SetI(self->GetRefCount());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getrefs

    @input
    v

    @output
    l (RefList)

    @info
    Return the list of client objects which keep references to this
    object.
*/
static void n_getrefs(void *o, nCmd *cmd)
{
    nRoot *self = (nRoot *) o;
    {
        int num_refs = 0;
    
        nRef<nRoot> *r;
        for (r = (nRef<nRoot> *) self->GetRefs()->GetHead();
             r;
             r = (nRef<nRoot> *) r->GetSucc())
        {
             num_refs++;
        }
        nArg* args = n_new nArg[num_refs];
        n_assert(args);
        int i = 0;
        for (r = (nRef<nRoot> *) self->GetRefs()->GetHead();
             r;
             r = (nRef<nRoot> *) r->GetSucc())
        {
            nRoot *owner = r->getowner();
            args[i++].SetO(owner);
        }
        cmd->Out()->SetL(args, num_refs);
    }
}

//------------------------------------------------------------------------------
/**
    @cmd
    getclass

    @input
    v

    @output
    s (Classname)

    @info
    Return name of class which the object is an instance of.
*/
static void n_getclass(void *o, nCmd *cmd)
{
    nRoot *self = (nRoot *) o;
    cmd->Out()->SetS(self->GetClass()->GetName());
}

//------------------------------------------------------------------------------
/**
    @cmd
    isa

    @input
    s (Classname)

    @output
    b (Success)

    @info
    Check whether the object is instantiated or derived from the
    class given by 'Classname'.
*/
static void n_isa(void *o, nCmd *cmd)
{
    nRoot *self = (nRoot *) o;
    {
        const char *arg0 = cmd->In()->GetS();
        nClass *cl = nRoot::ks->FindClass(arg0);
        if (cl) cmd->Out()->SetB(self->IsA(cl));
        else    cmd->Out()->SetB(FALSE);
    }
}

//------------------------------------------------------------------------------
/**
    @cmd
    isinstanceof

    @input
    s (Classname)

    @output
    b (Success)

    @info
    Check whether the object is an instance of the class given
    by 'Classname'.
*/
static void n_isinstanceof(void *o, nCmd *cmd)
{
    nRoot *self = (nRoot *) o;
    {
        const char *arg0 = cmd->In()->GetS();
        nClass *cl = nRoot::ks->FindClass(arg0);
        if (cl) cmd->Out()->SetB(self->IsInstanceOf(cl));
        else    cmd->Out()->SetB(FALSE);
    }
}

//------------------------------------------------------------------------------
/**
    @cmd
    getcmds

    @input
    v

    @output
    s (Commands)

    @info
    Return a list of all script command prototypes the object accepts.
*/
static void n_getcmds(void *o, nCmd *cmd)
{
    nRoot *self = (nRoot *) o;
    {
        nHashList *cmd_list = n_new nHashList;
        nHashNode *node;
        int num_cmds = 0;
        
        self->GetCmdProtos(cmd_list);
        // count commands
        for (node = cmd_list->GetHead();
             node;
             node = node->GetSucc())
        {
            num_cmds++;
        }
        
        nArg* args = n_new nArg[num_cmds];
        int i = 0;
        while ((node = cmd_list->RemHead())) {
            args[i++].SetS(((nCmdProto*) node->GetPtr())->GetProtoDef());
            n_delete node;
        }
        cmd->Out()->SetL(args, num_cmds);
        n_delete cmd_list;
    }
}

//------------------------------------------------------------------------------
/**
    @cmd
    save

    @input
    v

    @output
    b (Success)

    @info
    Save the object under its current name into a filesystem
    file. The filename will be 'name_of_object.n'. Such a
    persistent object file must be read back with the
    nsh 'get' command.
*/
static void n_save(void *o, nCmd *cmd)
{
    nRoot *self = (nRoot *) o;
    cmd->Out()->SetB(self->Save());                    
}

//------------------------------------------------------------------------------
/**
    @cmd
    saveas

    @input
    s (Name)

    @output
    b (Success)

    @info
    Save the object under a given name into a file. A trailing
    '.n' will be appended.
*/
static void n_saveas(void *o, nCmd *cmd)
{
    nRoot *self = (nRoot *) o;
    cmd->Out()->SetB(self->SaveAs(cmd->In()->GetS()));
}

//------------------------------------------------------------------------------
/**
    @cmd
    clone

    @input
    s (CloneName)

    @output
    o (CloneHandle)

    @info
    Creates a clone with the name 'CloneName' of the object.
    If the original has child objects they will be cloned as well.
*/
static void n_clone(void *o, nCmd *cmd)
{
    nRoot *self = (nRoot *) o;
    cmd->Out()->SetO(self->Clone(cmd->In()->GetS()));
}

//------------------------------------------------------------------------------
/**
    @cmd
    parse

    @input
    s (Filename)

    @output
    b (Success)

    @info
    Invoke all commands in the given script on the object.
*/
static void n_parse(void *o, nCmd *cmd)
{
    nRoot *self = (nRoot *) o;
    cmd->Out()->SetB(self->Parse(cmd->In()->GetS()));
}

//------------------------------------------------------------------------------
/**
    @cmd
    getparent

    @input
    v

    @output
    o (ObjectHandle)

    @info
    Return the full pathname of the parent object. If the object
    doesn't have a parent object (this is only valid for the
    root object '/'), 'null' will be returned.
*/
static void n_getparent(void *o, nCmd *cmd)
{
    nRoot *self = (nRoot *) o;
    cmd->Out()->SetO(self->GetParent());
}

//------------------------------------------------------------------------------
/**
    @cmd
    gethead

    @input
    v

    @output
    o (ObjectHandle)

    @info
    Return full pathname of first child object, or 'null'
    if no child objects exist.
*/
static void n_gethead(void *o, nCmd *cmd)
{
    nRoot *self = (nRoot *) o;
    cmd->Out()->SetO(self->GetHead());
}

//------------------------------------------------------------------------------
/**
    @cmd
    gettail

    @input
    v

    @output
    o (ObjectHandle)

    @info
    Return full pathname of last child object, or 'null'
    if no child objects exist.
*/
static void n_gettail(void *o, nCmd *cmd)
{
    nRoot *self = (nRoot *) o;
    cmd->Out()->SetO(self->GetTail());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getsucc

    @input
    v

    @output
    o (ObjectHandle)

    @info
    Return full pathname of next object in the same hierarchy
    level, zero if no next object exists.
*/
static void n_getsucc(void *o, nCmd *cmd)
{
    nRoot *self = (nRoot *) o;
    cmd->Out()->SetO(self->GetSucc());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getpred

    @input
    v

    @output
    o (ObjectHandle)

    @info
    Return full pathname of previous object in the same
    hierarchy level, zero if no previous object exists. 
*/
static void n_getpred(void *o, nCmd *cmd)
{
    nRoot *self = (nRoot *) o;
    cmd->Out()->SetO(self->GetPred());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getchildren

    @input
    v

    @output
    l (RefList)

    @info
    Return the list of children objects in the hierarchy.
*/
static void n_getchildren(void *o, nCmd *cmd)
{
    nRoot *self = (nRoot *) o;
    {
        int num_children = 0;

        nRoot *child;
        for (child = (nRoot *) self->GetHead();
             child;
             child = (nRoot *) child->GetSucc())
        {
             num_children++;
        }
        nArg* children = n_new nArg[num_children];
        n_assert(children);
        int i = 0;
        for (child = (nRoot *) self->GetHead();
             child;
             child = (nRoot *) child->GetSucc())
        {
            children[i++].SetO(child);
        }
        cmd->Out()->SetL(children, num_children);
    }
}

//------------------------------------------------------------------------------
/**
    @cmd
    setname

    @input
    s (Name)

    @output
    v

    @info
    Give the object a new name ('Name' may not contain any path
    components!).
*/
static void n_setname(void *o, nCmd *cmd)
{
    nRoot *self = (nRoot *) o;
    {
        const char *arg0 = cmd->In()->GetS();
        self->SetName(arg0);
    }                
}

//------------------------------------------------------------------------------
/**
    @cmd
    getname

    @input
    v

    @output
    s (Name)

    @info
    Return the object's name without path.
*/
static void n_getname(void *o, nCmd *cmd)
{
    nRoot *self = (nRoot *) o;
    cmd->Out()->SetS(self->GetName());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getfullname

    @input
    v

    @output
    s (Name)

    @info
    Return the full pathname of the object.
*/
static void n_getfullname(void *o, nCmd *cmd)
{
    nRoot *self = (nRoot *) o;
    char buf[N_MAXPATH];
    cmd->Out()->SetS(self->GetFullName(buf,sizeof(buf)));
}

//------------------------------------------------------------------------------
/**
    @cmd
    getrelpath

    @input
    o (OtherObject)

    @output
    s (Path)

    @info
    Return the shortest relative path leading from this object
    to the given object. 
*/
static void n_getrelpath(void *o, nCmd *cmd)
{
    nRoot *self = (nRoot *) o;
    char buf[N_MAXPATH];
    cmd->Out()->SetS(self->GetRelPath((nRoot *)cmd->In()->GetO(),buf,sizeof(buf)));
}

//------------------------------------------------------------------------------
/**
    @cmd
    find

    @input
    s (Name)

    @output
    o (PathName)

    @info
    Find a child object with name 'Name'. Return full path
    of child if found, or 'null' if not found.
*/
static void n_find(void *o, nCmd *cmd)
{
    nRoot *self = (nRoot *) o;
    cmd->Out()->SetO(self->Find(cmd->In()->GetS()));
}

//------------------------------------------------------------------------------
/**
    @cmd
    sort

    @input
    v

    @output
    v

    @info
    Sort all children objects alphabetically.
*/
static void n_sort(void *o, nCmd *)
{
    nRoot *self = (nRoot *) o;
    self->Sort();
}

//------------------------------------------------------------------------------
/**
    @cmd
    import

    @input
    s (Filename)

    @output
    v

    @info
    Parse an external persistent object exactly like 'parse',
    but remember the filename. When saving the object, instead
    of writing a sequence of commands, the object will only
    save a single 'import' statement with the original
    filename. This is useful when working with libraries of objects.
*/
static void n_import(void *o, nCmd *cmd)
{
    nRoot *self = (nRoot *) o;
    cmd->Out()->SetB(self->Import(cmd->In()->GetS()));
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
