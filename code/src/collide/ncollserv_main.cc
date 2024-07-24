#define N_IMPLEMENTS nCollideServer
//-------------------------------------------------------------------
//  ncollserv_main.cc
//  (C) 2000 RadonLabs GmbH -- A.Weissflog
//-------------------------------------------------------------------
#include "collide/ncollideserver.h"
#include "collide/ncollidecontext.h"
#include "collide/ncollideshape.h"

nNebulaScriptClass(nCollideServer, "nroot");

//-------------------------------------------------------------------
/**
     - 09-Jun-00   floh    created
*/
//-------------------------------------------------------------------
nCollideServer::nCollideServer()
{
    this->unique_id             = 0;
    this->default_context       = NULL;
    this->num_coll_classes      = 0;
    this->colltype_table        = NULL;
    this->in_begin_collclasses  = false;
    this->in_begin_colltypes    = false;
}

//-------------------------------------------------------------------
/**
     - 09-Jun-00   floh    created
*/
//-------------------------------------------------------------------
nCollideServer::~nCollideServer()
{
    // release collision type definitions
    nCollClassNode *ccn;
    while ((ccn = (nCollClassNode *) this->collclass_list.RemHead())) delete ccn;
    if (this->colltype_table) delete[] this->colltype_table;

    // release any shapes and contexts that may still be around...
    nCollideShape *cs;
    while ((cs = (nCollideShape *) this->shape_list.GetHead())) {
        this->ReleaseShape(cs);
    }
    nCollideContext *cc;
    while ((cc = (nCollideContext *) this->context_list.GetHead())) {
        this->ReleaseContext(cc);
    }

    // delete any 
}

//-------------------------------------------------------------------
/**
     - 15-Jun-00   floh    created
*/
//-------------------------------------------------------------------
nCollideContext *nCollideServer::NewContext(void)
{
    nCollideContext *cc = new nCollideContext(this);
    this->context_list.AddHead(cc);
    return cc;
}

//-------------------------------------------------------------------
/**
    @brief Create a new, possibly shared shape object.

     - 15-Jun-00   floh    created
*/
//-------------------------------------------------------------------
nCollideShape *nCollideServer::NewShape(const char *id)
{
    n_assert(id);

    char buf[N_MAXPATH];
    this->getResourceID(id,buf,sizeof(buf));

    // shape already existing?
    nCollideShape *cs = (nCollideShape *) this->shape_list.Find(id);
    if (!cs) {
        cs = new nCollideShape(id);
        this->shape_list.AddTail(cs);
    }
    cs->AddRef();
    return cs;
}

//-------------------------------------------------------------------
/**
     - 15-Jun-00   floh    created
*/
//-------------------------------------------------------------------
void nCollideServer::ReleaseContext(nCollideContext *cc)
{
    n_assert(cc);
    cc->Remove();
    delete cc;
}

//-------------------------------------------------------------------
/**
     - 15-Jun-00   floh    created
*/
//-------------------------------------------------------------------
void nCollideServer::ReleaseShape(nCollideShape *cs)
{
    n_assert(cs);
    cs->RemRef();
    if (0 == cs->GetRef()) {
        cs->Remove();
        delete cs;
    }
}

//-------------------------------------------------------------------
/**
     - 16-Jun-00   floh    created
*/
//-------------------------------------------------------------------
nCollideContext *nCollideServer::GetDefaultContext(void)
{
    if (!this->default_context) {
        this->default_context = this->NewContext();
    }
    return this->default_context;
}

//-------------------------------------------------------------------
/**
    Get a resource id string from a path name, or create a unique
    resource id string if no name is given.
  
     - 07-Jun-00   floh    created
*/
//-------------------------------------------------------------------
char *nCollideServer::getResourceID(const char *name, char *buf, int buf_size)
{
    if (!name) sprintf(buf,"res%d",(int)this->unique_id++);
    else {
        // cut name to 32 characters and convert illegal chars
        // to underscores
        char c;
        char *str;
        int len = strlen(name)+1;
        int off = len - buf_size;
        if (off < 0) off = 0;
        len -= off;
        strcpy(buf,&(name[off]));
        str = buf;
        while ((c = *str)) {
            if ((c=='.')||(c=='/')||(c=='\\')||(c==':')) *str='_';
            str++;
        }
    }
    return buf;
}

//-------------------------------------------------------------------
/**
     - 03-Jul-00   floh    created
*/
//-------------------------------------------------------------------
void nCollideServer::BeginCollClasses(void)
{
    n_assert(!this->in_begin_collclasses);

    // free any previous collision class definitions
    this->num_coll_classes = 0;
    nCollClassNode *ccn;
    while ((ccn = (nCollClassNode *) this->collclass_list.RemHead())) delete ccn;
    
    this->in_begin_collclasses = true;
}

//-------------------------------------------------------------------
/**
     - 03-Jul-00   floh    created
*/
//-------------------------------------------------------------------
void nCollideServer::AddCollClass(const char *cl_name)
{
    n_assert(this->in_begin_collclasses);
    n_assert(cl_name);

    // make sure the class doesn't exist already
    if (this->collclass_list.Find(cl_name)) {
        n_printf("nCollideServer() WARNING: collision class '%s' already defined!\n",cl_name);
        return;
    }

    // create a new node
    nCollClassNode *ccn = new nCollClassNode(cl_name,this->num_coll_classes++);
    this->collclass_list.AddTail(ccn);
}

//-------------------------------------------------------------------
/**
     - 03-Jul-00   floh    created
*/
//-------------------------------------------------------------------
void nCollideServer::EndCollClasses(void)
{
    n_assert(this->in_begin_collclasses);
    this->in_begin_collclasses = false;
}

//-------------------------------------------------------------------
/**
     - 03-Jul-00   floh    created
*/
//-------------------------------------------------------------------
void nCollideServer::BeginCollTypes(void)
{
    n_assert(!this->in_begin_collclasses);
    n_assert(!this->in_begin_colltypes);

    if (this->colltype_table) {
        delete[] this->colltype_table;
        this->colltype_table = NULL;
    }

    // create collision type table and initialize to 
    // "treat all collisions as exact checks"
    int table_size = this->num_coll_classes*this->num_coll_classes;
    this->colltype_table = new nCollType[table_size];
    int i;
    for (i=0; i<table_size; i++) {
        this->colltype_table[i] = COLLTYPE_EXACT;
    }

    this->in_begin_colltypes = true;
}

//-------------------------------------------------------------------
/**
    Important: Collision types MUST be bidirectional, if one object
    checks collision with another object, the collision type must
    be identical as if the check would be in the other direction.
    Due to the implementation of the top-level-collision check,
    one of the 2 checks may return false, although a collision may
    take place!

     - 03-Jul-00   floh    created
*/
//-------------------------------------------------------------------
void nCollideServer::AddCollType(const char *cl1, const char *cl2, nCollType ctype)
{
    n_assert(cl1);
    n_assert(cl2);
    n_assert(this->in_begin_colltypes);
    n_assert(this->colltype_table);

    nCollClassNode *ccn1 = (nCollClassNode *) this->collclass_list.Find(cl1);
    nCollClassNode *ccn2 = (nCollClassNode *) this->collclass_list.Find(cl2);

    if (!ccn1) {
        n_printf("nCollideServer(): collision class '%s' not defined!\n",cl1);
        return;
    }
    if (!ccn2) {
        n_printf("nCollideServer(): collision class '%s' not defined!\n",cl2);
        return;
    }

    nCollClass cc1 = ccn1->GetCollClass();
    nCollClass cc2 = ccn2->GetCollClass();

    // enforce bidirectional collision type
    int index;
    index = (int(cc1)*this->num_coll_classes) + int(cc2);
    this->colltype_table[index] = ctype;
    index = (int(cc2)*this->num_coll_classes) + int(cc1);
    this->colltype_table[index] = ctype;
}

//-------------------------------------------------------------------
/**
     - 03-Jul-00   floh    created
*/
//-------------------------------------------------------------------
void nCollideServer::EndCollTypes(void)
{
    n_assert(this->in_begin_colltypes);
    this->in_begin_colltypes = false;
}

//-------------------------------------------------------------------
/**
    - 03-Jul-00   floh    created
*/
//-------------------------------------------------------------------
nCollClass nCollideServer::QueryCollClass(const char *cc)
{
    n_assert(cc);
    nCollClassNode *ccn = (nCollClassNode *) this->collclass_list.Find(cc);
    if (!ccn) {
        n_error("nCollideServer: WARNING!!! Collision class '%s' not defined!\n",cc);
    }
    return ccn->GetCollClass();
}

//-------------------------------------------------------------------
/**
     - 03-Jul-00   floh    created
*/
//-------------------------------------------------------------------
nCollType nCollideServer::QueryCollType(const char *s_cc1, const char *s_cc2)
{
    n_assert(s_cc1);
    n_assert(s_cc2);
    nCollClass cc1 = this->QueryCollClass(s_cc1);
    nCollClass cc2 = this->QueryCollClass(s_cc2);
    return this->QueryCollType(cc1,cc2);
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
