#define N_IMPLEMENTS nRoot
#define N_KERNEL
//-------------------------------------------------------------------
//  nroot_main.cc
//  (C) 2000 A.Weissflog
//-------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "kernel/nroot.h"

//--------------------------------------------------------------------
/**
    - 08-Oct-98   floh    created
    - 11-Dec-98   floh    + Objekt-Flags
    - 26-May-99   floh    + ks->num_objects
    - 02-May-00   floh    + parse file name
*/
//--------------------------------------------------------------------
nRoot::nRoot(void)
{
    this->instance_cl = NULL;
    this->parent      = NULL;
    this->child_list  = NULL;
    this->ref_count   = 1;
    this->root_flags  = 0;
    this->import_file = NULL;
    ks->num_objects++;
}

//--------------------------------------------------------------------
/**
    - 08-Oct-98   floh    created
    - 01-Dec-98   floh    wiederholt jetzt Release() auf Child solange,
                          bis es wirklich tot ist (es kann sein, dass
                          dessen RefCount nicht 1 ist).
    - 13-May-99   floh    + Cwd-Safe-Code. Wenn dieses Objekt das
                            Cwd ist, wird das Cwd auf '/' gesetzt,
                            damit der Kernel keinen ungueltigen Pointer
                            mit sich rumschleppt
    - 17-May-99   floh    + Listenhandling umgeschrieben
    - 26-May-99   floh    + ks->num_objects
    - 14-May-99   floh    + gibt alle Childs frei, die noch uebrig
                            sind...
    - 02-May-00   floh    + release parse file name
    - 12-Sep-00   floh    + InvalidateAllRefs() moved here
    - 12-Dec-00   floh    + BUG! when removing child objects, the loop
                            used a RemHead() to remove the child object
                            from the list, which in turn invalidates
                            the parent pointer, this caused the "am I the
                            current cwd" check to fail!
*/
//--------------------------------------------------------------------
nRoot::~nRoot(void)
{
    n_assert(this->ref_count == 0);

    if (this->import_file) n_free(this->import_file);

    // verhindern, dass Cwd ungueltig wird...
    if (this==ks->GetCwd()) {
        n_printf("WARNING: Cwd orphaned, setting cwd to '/'\n");
        ks->SetCwd((nRoot*)NULL);
    }

    // alle Child-Objects freigeben
    nRoot *child;
    while ((child = this->GetHead())) {
        while (!child->Release());
    }

    // invalidate all refs to me
    this->InvalidateAllRefs();

    // wenn ich selbst Child, mich abkoppeln
    if (this->parent) this->Remove();   
        
    // Objekt abmelden
    this->instance_cl->DelObject(this);

    // Child-List-Objekt killen
    if (this->child_list) {
        n_delete this->child_list;
        this->child_list = NULL;
    }
    ks->num_objects--;
}

//--------------------------------------------------------------------
/**
    @brief Will be called by the kernel after the object has been
    constructed and linked into the name space.

    - 13-Nov-00   floh    created
*/
//--------------------------------------------------------------------
void nRoot::Initialize(void) {

}

//--------------------------------------------------------------------
/**
    @brief Increment the reference count.

    - 08-Oct-98   floh    created
*/
//--------------------------------------------------------------------
long nRoot::AddRef(void)
{
    return ++this->ref_count;
}

//--------------------------------------------------------------------
/**
    @brief Decrement the reference count and destroy the object
    if no references remain.

    - 08-Oct-98   floh    created
    - 11-Nov-98   floh    Rueckgabewert von long auf bool
    - 10-Dec-98   floh    + wertet jetzt zuerst Objekt-Flags aus
    - 24-Apr-99   floh    + Release() auf Childobjekts (war vorher
                            im Destruktor)
    - 14-Jun-99   floh    + Irgendwie ham se mir wahrscheinlich ins
                            Gehirn geschissen. Das Killen der Subobjekte
                            SOLLTE im Destruktor passieren, weil damit
                            Subklassen die Objekte selbst auf
                            spezialisierte Weise wegraeumen koennen. Das
                            sei hiermit definiert!
    - 16-Jul-99   floh    + N_FLAGS_PROTECTED gekillt
                          + InvalidateAllRefs()
    - 12-Sep-00   floh    + InvalidateAllRefs() has been moved to
                            destructor
*/
//--------------------------------------------------------------------
bool nRoot::Release(void)
{
    bool retval = false;
    this->ref_count--;
    if (this->ref_count == 0) {
        n_delete this;
        retval = true;
    }
    return retval;
}

//--------------------------------------------------------------------
/**
    @brief Set a flag on the object.

    Currently, only two flags are supported and used (by the
    serialization system):

       - @c N_FLAG_SAVEUPSIDEDOWN - Save children first, then own state.
       - @c N_FLAG_SAVESHALLOW - Do not save child objects

    - 10-Dec-98   floh    created
*/
//--------------------------------------------------------------------
void nRoot::SetFlags(int f)
{
    this->root_flags |= f;
}

//--------------------------------------------------------------------
/**
    @brief Unset a flag from an object.

    See SetFlags() for a discussion of the available flags.

    - 10-Dec-98   floh    created
*/
//--------------------------------------------------------------------
void nRoot::UnsetFlags(int f)
{
    this->root_flags &= ~f;
}

//--------------------------------------------------------------------
/**
    @brief Get the flags currently set on the object.

    See SetFlags() for a discussion of the available flags.

    - 10-Dec-98   floh    created
*/
//--------------------------------------------------------------------
int nRoot::GetFlags(void)
{
    return this->root_flags;
}

//--------------------------------------------------------------------
/**
    @brief Set the class object that this object is an instance of.

    - 08-Oct-98   floh    created
*/
//--------------------------------------------------------------------
void nRoot::SetClass(nClass *inst_cl)
{
    this->instance_cl = inst_cl;
}

//--------------------------------------------------------------------
/**
   @brief Return the class object that this object is an instance of.

   - 08-Oct-98   floh    created
*/
//--------------------------------------------------------------------
nClass *nRoot::GetClass(void)
{
    return this->instance_cl;
}

//--------------------------------------------------------------------
/**
    @brief Test whether or not this object inherits from the class
    @c cls or not.

    - 08-Oct-98   floh    created
    - 06-Mar-00   floh    slightly optimized
*/
//--------------------------------------------------------------------
bool nRoot::IsA(nClass *cls)
{
    nClass *act_cl = this->instance_cl;
    do {
        if (act_cl == cls) return true;
    } while ((act_cl = act_cl->GetSuperClass()));
    return false;
}

//--------------------------------------------------------------------
/**
    @brief Return whether or not the object is a direct instance
    of the class @c cls.

    - 08-Oct-98   floh    created
*/
//--------------------------------------------------------------------
bool nRoot::IsInstanceOf(nClass *cls)
{
    return (cls == this->instance_cl);
}

//--------------------------------------------------------------------
/**
    @brief Return the reference count.

    - 08-Oct-98   floh    created
*/
//--------------------------------------------------------------------
long nRoot::GetRefCount(void)
{
    return this->ref_count;
}

//--------------------------------------------------------------------
/**
    @brief Get the full name within the hierarchy of the nRoot object.

    - 08-Oct-98   floh    created
    - 23-Jan-01   floh    why the f*ck was this method recursive???
*/
//--------------------------------------------------------------------
char *nRoot::GetFullName(char *buf, long sizeof_buf)
{
    // build stack of pointers leading from me to root
    const int maxDepth = 128;
    nRoot *stack[maxDepth];
    nRoot *cur = this;
    int i = 0;
    do 
    {
        stack[i++] = cur;
    } while ((cur = cur->GetParent()) && (i<maxDepth));

    // traverse stack in reverse order and build filename    
    char tmp[N_MAXPATH];
    tmp[0] = 0;
    i--;
    for (; i>=0; i--) 
    {
        const char *curName = stack[i]->GetName();
        strcat(tmp,curName);
        
        // add slash if not hierarchy root, and not last element in path
        if ((curName[0] != '/') && (i>0)) 
        {
            strcat(tmp,"/");
        }
    }

    // copy result to provided buffer
    n_strncpy2(buf,tmp,sizeof_buf);
    return buf;
}

//--------------------------------------------------------------------
/**
    @brief Return shortest relative path leading from 'this' to 'other' object.

    This is a slow operation, unless one object is the parent of
    the other (this is a special case optimization).

    - 06-Mar-00   floh    created
*/
//--------------------------------------------------------------------
char *nRoot::GetRelPath(nRoot *other, char *buf, long sizeof_buf)
{
    n_assert(other);
    n_assert(other != this);
    n_assert(buf);
    n_assert(sizeof_buf > 0);

    buf[0] = 0;
    if (other == this->GetParent()) {

        // special case optimize: other is parent of this
        n_strcat(buf,"..",sizeof_buf);

    } else if (other->GetParent() == this) {

        // special case optimize: this is parent of other
        n_strcat(buf,other->GetName(),sizeof_buf);

    } else {

        // normal case
        nList this_hier;
        nList other_hier;
        nRoot *o;

        // for both objects, create lists of all parents up to root 
        o = this;
        do {
            nNode *n = n_new nNode(o);
            this_hier.AddHead(n);
        } while ((o=o->GetParent()));
        o = other;
        do {
            nNode *n = n_new nNode(o);
            other_hier.AddHead(n);
        } while ((o=o->GetParent()));

        // remove identical parents
        bool running = true;
        do {
            nNode *n0 = this_hier.GetHead();
            nNode *n1 = other_hier.GetHead();
            if (n0 && n1) {
                if (n0->GetPtr() == n1->GetPtr()) {
                    n0->Remove();
                    n1->Remove();
                    n_delete n0;
                    n_delete n1;
                } else running = false;
            } else running = false;
        } while (running);

        // create path leading upward from this to the identical parent
        nNode *n;
        while ((n=this_hier.RemTail())) {
            n_delete n;
            n_strcat(buf,"../",sizeof_buf);
        }
        // create path leading downward from parent to 'other'
        while ((n=other_hier.RemHead())) {
            o = (nRoot *) n->GetPtr();
            n_delete n;
            n_strcat(buf,o->GetName(),sizeof_buf);
            n_strcat(buf,"/",sizeof_buf);
        }

        // eliminate trailing '/'
        buf[strlen(buf)-1] = 0;
    }

    // done
    return buf;
}

//--------------------------------------------------------------------
/**
    @brief Given an nCmd, execute the appropriate script function
    on the target object.

    - 02-Jan-00   floh    created
*/
//--------------------------------------------------------------------
bool nRoot::Dispatch(nCmd *cmd)
{
    n_assert(cmd->GetProto());
    cmd->Rewind();
    cmd->GetProto()->Dispatch((void*)this, cmd);
    cmd->Rewind();
    return true;
}

//--------------------------------------------------------------------
/**
    @brief Get all of the nCmdProto's supported by this object, including
    those from all super classes.

    - 19-Oct-98   floh    created
    - 03-Nov-98   floh    umbenannt nach GetCmdProtos()
    - 17-May-99   floh    neues Listenhandling
    - 05-Feb-01   floh    + simplified: no longer checks
                            for duplicate names, this is illegal
                            anyway
    - 30-Aug-03   vadim   cloning of cmd protos is no longer
                          feasable since their exact type can't be
                          known without C++ RTTI. so instead hashlist
                          is populated with plain hashnodes that
                          have the same name as the cmd proto and hold
                          a pointer to the real cmd proto.
*/
//--------------------------------------------------------------------
void nRoot::GetCmdProtos(nHashList *cmd_list)
{
    // for each superclass attach it's command proto names
    // to the list
    nClass *cl = this->instance_cl;
    
    // for each superclass...
    do {
        nHashList *cl_cmdprotos = cl->GetCmdList();
        nCmdProto *cmd_proto;
        for (cmd_proto=(nCmdProto *) cl_cmdprotos->GetHead(); 
             cmd_proto; 
             cmd_proto=(nCmdProto *) cmd_proto->GetSucc()) 
        {
            nHashNode* node = new nHashNode(cmd_proto->GetName());
            node->SetPtr((void*)cmd_proto);
            cmd_list->AddTail(node);
        }
    } while ((cl = cl->GetSuperClass()));
}

//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------

